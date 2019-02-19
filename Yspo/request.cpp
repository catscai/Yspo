#include "request.h"
#include "utils.h"
#include "app.h"
using namespace yspo;
request::request(boost::shared_ptr<std::string> str_ptr,tcp::socket& s):m_reqStr(str_ptr)
			,m_isNull(true),m_sock(s){
			dealtext();
		}
request::~request(){}

void	request::dealtext()
{
	auto p = m_reqStr->find_first_of('\r');
	if(p == std::string::npos){
		return;
	}
	std::string headline = m_reqStr->substr(0,p);
	std::string text = m_reqStr->substr(p + 2);
	std::vector<std::string> tmp;
	std::vector<std::string> tmp2;
	boost::split(tmp,text,boost::is_any_of("\r\n"),boost::token_compress_on);
	boost::split(tmp2,headline,boost::is_any_of(" "),boost::token_compress_on);
	if(tmp2.size() < 3){
		return;
	}else{
		m_isNull = false;
	}
	m_method = tmp2[0];	//请求方式
	m_path = tmp2[1];	//请求路径
	auto pos = tmp2[1].find_first_of('?');
	if(pos != std::string::npos){
		m_path = tmp2[1].substr(0,pos);
		auto str = tmp2[1].substr(pos + 1);
		std::vector<std::string> ls;
		std::vector<std::string> para;
		boost::split(ls,str,boost::is_any_of("&"));
		for(size_t i = 0; i < ls.size(); i++){
			boost::split(para,ls[i],boost::is_any_of("="));
			if(para.size() == 2){
				m_Param[para[0]] = para[1];
				m_keyLst.push_back(para[0]);
			}
			para.clear();
		}
	}
	m_version = tmp2[2];	//http版本

	std::vector<std::string> para;
	std::string cookieStr = "";
	for(size_t i = 3; i < tmp.size(); i++){
		boost::split(para,tmp[i],boost::is_any_of(":&"));
		if(para.size() == 2){
			if(boost::to_lower_copy(para[0]) == "cookie"){
				cookieStr = para[1];
			}else{
				m_Param[para[0]] = para[1];
				m_keyLst.push_back(para[0]);
			}
		}
		para.clear();
	}

	if(m_method == "POST")
	{
		auto rangeIte = boost::find_first(*m_reqStr,"\r\n\r\n");
		string s(rangeIte.end(),m_reqStr->end());
		m_Param["Body"] = s;
		if(this->m_Param.count("Content-Type"))
		{
			auto Map = Tools::dealRequestContentType(s,m_Param["Content-Type"]);
			if(Map.get() != NULL)
			{
				auto ite = Map->begin();
				while(ite != Map->end())
				{
					this->addAttribute(ite->first,ite->second);
					++ite;
				}
			}
		}
	}

	if(cookieStr != ""){
		boost::trim(cookieStr);
		vector<std::string> ls;
		vector<std::string> coos;
		boost::split(ls,cookieStr,boost::is_any_of(";"),boost::token_compress_on);
		for(size_t i = 0; i < ls.size(); i++){
			boost::split(coos,ls[i],boost::is_any_of("="));
			if(coos.size() == 2){
				boost::shared_ptr<cookie> co(new cookie(coos[0],coos[1]));
				m_cookies.push_back(co);
			}
			coos.clear();
		}
	}
}

const std::string& request::operator[](const std::string& key)
{
	if(m_Param.count(key))
		return m_Param[key];
	return m_null;
}
const std::string& request::to_string()
{
	return *m_reqStr;
}
const std::string& request::version()
{
	return m_version;
}
const std::string& request::method()
{
	return m_method;
}
const std::vector<std::string>& request::keys()
{
	return m_keyLst;
}
const std::string& request::value(const std::string& key)
{
	return (*this)[key];
}
const std::string& request::path()
{
	return m_path;
}
bool	request::invalid()
{
	return m_isNull;
}
void	request::addAttribute(const std::string& key,const std::string& val)
{
	m_Attribute[key] = val;
}
const std::string& request::getAttribute(const std::string& key)
{
	if(m_Attribute.count(key))
		return m_Attribute[key];
	return m_null;
}
void	request::sendRequest(const std::string& url)
{
	m_path = url;
}
boost::shared_ptr<cookie> request::getCookie(const std::string& key)
{
	for(boost::shared_ptr<cookie> c : m_cookies){
		if(c->getName() == key)
			return c;
	}
	return boost::shared_ptr<cookie>();
}
const std::vector<boost::shared_ptr<cookie>>& request::getCookies()
{
	return m_cookies;
}

boost::shared_ptr<session> request::getSession()
{
	for(auto c : m_cookies){
		if(c->getName() == "SESSIONID"){
			auto sess = APP::sessManager->getSession(c->getVal());
			if(sess.get() != NULL)
				return sess;
			else{
				boost::shared_ptr<session> s(new session(c->getVal()));
				APP::sessManager->addSession(s);
				s->setSocketMark(&m_sock);
				return s;
			}
		}
	}
	//默认随机数为20位的字符串
	boost::shared_ptr<session> s(new session(Tools::getRandomString(20)));
	APP::sessManager->addSession(s);
	s->setSocketMark(&m_sock);
	return s;
}