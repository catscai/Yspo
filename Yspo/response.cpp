#include "response.h"
#include "connection.h"
#include "utils.h"
using namespace yspo;
response::response(connection* sess):m_sess(sess),m_contentLength(0)
{
	init();
}
response::~response(){}

void	response::init()
{
	m_file.reset(new std::string);
	m_statusLine = "HTTP/1.1 200 OK";

	str_ptr str(new std::string("Date: "));
	date d = day_clock::universal_day();
	ptime t = second_clock::local_time();
	format fmt("%1%, %2% %3% %4% %5% GMT");
	fmt % d.day_of_week() % d.day() % d.month() % d.year() % t.time_of_day();
	*str += fmt.str();

	m_param["Date"] = str;
	str.reset(new std::string("0"));
	//*str += fmt.str();
	m_param["Expires"] = str;
	str.reset(new std::string("Server: Yspo"));
	m_param["Server"] = str;

	str.reset(new std::string("Content-Type: text/html;charset=UTF-8"));
	m_param["Content-Type"] = str;
	str.reset(new std::string("Connection: keep-alive"));
	m_param["Connection"] = str;
	str.reset(new std::string("Access-Control-Allow-Origin: *"));
	m_param["Access-Control-Allow-Origin"] = str;
	str.reset(new std::string("Access-Control-Allow-Methods: GET,POST,OPTIONS"));
	m_param["Access-Control-Allow-Methods"] = str;
	str.reset(new std::string("Cache-Control: no-cache"));
	m_param["Cache-Control"] = str;
}

void	response::set_param(const std::string& key,const std::string val)
{
	if(m_param.count(key)){
		*m_param[key] = val;
	}else{
		m_param[key].reset(new std::string(val));
	}
}

void	response::send_file(const std::string& path)
{
	//1.检测文件类型、更改Content-Type 所以在调用该函数后不可以更改Content-Type,Content-Length
	auto pos = path.find_last_of('.');
	std::string fileType = path.substr(pos + 1);
	boost::to_lower(fileType);
	if(fileType == "css"){
		this->set_ContentType("text/css");
	}else if(fileType == "js"){
		this->set_ContentType("text/javascript");
	}else if(fileType == "png"){
		//this->set_CacheControl("max-age=2592000");
		this->set_ContentType("imgage/png");
		this->set_ContentTransferEncoding("binary");
	}else if(fileType == "jpg"){
		this->set_ContentType("imgage/jpeg");
		this->set_ContentTransferEncoding("binary");
	}else if(fileType == "ico"){
		this->set_ContentType("image/x-icon");
		this->set_ContentEncoding("gzip");
	}else if(fileType == "gif"){
		this->set_ContentType("image/gif");
		this->set_ContentTransferEncoding("binary");
	}else if(fileType == "mp4" || fileType == "avi" || 
		fileType == "flv"){
		this->set_ContentTransferEncoding("binary");
		this->set_ContentType("video/" + fileType);
	}else if(fileType == "mp3" || fileType == "wav"){
		this->set_ContentTransferEncoding("binary");
		this->set_ContentType("audio/" + fileType);
	}
	//2.得到文件长度、更改Content-Length、、发送文件要以二进制形式发送否则就会导致乱码长度不一
	ifstream fin(path,ios::binary);
	if(fin.is_open()){
		fin.seekg(0,ios::end);
		size_t size = fin.tellg();
		fin.seekg(0,ios::beg);
		this->set_ContentLength(size);
	}else
		return;
	//3.读取文件到m_file中
	//char c;
	while(!fin.eof()){
		//c = 
		////cout<<c;
		*m_file += fin.get();
		//fin>>*m_file;
	}
	int s = 0;
	//*m_file += "\r\n"
	//cout<<*m_file<<endl<<m_file->size()<<endl;
}

void	response::reply()
{//每次响应必须调用的函数
	std::string s;
	s += m_statusLine;
	s += "\r\n";
	auto ite = m_param.begin();
	while(ite != m_param.end())
	{
		s += *(ite->second);
		s += "\r\n";
		++ite;
	}
	for(size_t i = 0; i < m_cookies.size(); i++){
		s += "Set-Cookie: ";
		s += m_cookies[i]->getName();
		s += "=";
		s += m_cookies[i]->getVal();
		s += "; path=" + m_cookies[i]->getPath();
		s += "; domain=" + m_cookies[i]->getDomain();
		if(m_cookies[i]->getMaxAge() > 0)
			s += "; expires=" + m_cookies[i]->getAgeStr();
		s += "; version=" + boost::lexical_cast<std::string>(m_cookies[i]->getVersion());
		s += "; \r\n";
	}
	s += "\r\n";
	if(m_contentLength > 0){
		//
		if(m_file.get() != NULL && m_contentLength == m_file->size() - 1){
			s += *m_file;
		}else if(m_body.get() != NULL && m_contentLength == m_body->size() - 1){
			s += *m_body;
		}
	}
	boost::shared_ptr<std::string> info(new std::string(s));
	m_sess->post_send(info);
}

void	response::addBodyData(const std::string& data)
{
	if(m_body.get() == NULL){
		m_body.reset(new std::string(data));
	}else{
		*m_body += data;
	}
	this->set_ContentLength(m_body->size());
}

void	response::addCookie(boost::shared_ptr<cookie> coo)
{
	m_cookies.push_back(coo);
}
void	response::addSession(boost::shared_ptr<session> sess)
{
	boost::shared_ptr<cookie> coo(new cookie("SESSIONID",sess->getSessionId()));
	m_cookies.push_back(coo);
}

//设置响应状态
void	response::set_statuscode(int code)
{
	format fmt("HTTP/1.1 %1% %2%");
	fmt % code;
	switch(code)
	{
	case 200:
		fmt % "OK";
		break;
	case 301://重定向：永久
		fmt % "Moved Permanently";
		break;
	case 302://重定向：临时
		fmt % "Move temporarily";
		break;
	case 400:
		fmt % "Bad Request";
		break;
	case 401:
		fmt % "Unauthonzed";
		break;
	case 403:
		fmt % "Forbidden";
		break;
	case 404:
		fmt % "Not Found";
		break;
	case 500:
		fmt % "Internal Server Error";
		break;
	case 503:
		fmt % "Service Unavailable";
		break;
	default:
		fmt % "";
		break;
	}
	m_statusLine = fmt.str();
}

void	response::set_ContentType(const std::string& contentType)
{
	std::string str = "Content-Type: " + contentType;
	this->set_param("Content-Type",str);
}

void	response::set_ContentLength(size_t len)
{
	boost::format fmt("Content-Length: %1%");
	fmt % len;
	m_contentLength = len;
	this->set_param("Content-Length",fmt.str());
}

void	response::set_Connection(const std::string& conn)
{
	std::string str = "Connection: " + conn;
	this->set_param("Connection",str);
}

void	response::set_ContentEncoding(const std::string& encode)
{
	std::string str = "Content-Encoding: " + encode;
	this->set_param("Content-Encoding",str);
}

void	response::set_ContentLanguage(const std::string& language)
{
	std::string str = "Content-Language: " + language;
	this->set_param("Content-Language",language);
}

void	response::set_CacheControl(const std::string& control)
{
	std::string str = "Cache-Control: " + control;
	this->set_param("Cache-Control",str);
}

void	response::set_ContentTransferEncoding(const std::string& transfer)
{
	std::string str = "Content-Transfer-Encoding: " + transfer;
	this->set_param("Content-Transfer-Encoding",str);
}
void	response::location(const std::string& url,int code)
{
	//重定向：更改状态码、默认临时重定向302
	this->set_statuscode(code);
	std::string str = "Location: " + url;
	this->set_param("Location",str);
}