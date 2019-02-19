#include "cookie.h"
using namespace yspo;

cookie::cookie(const std::string& key,const std::string& val)
	:m_cookieName(new std::string(key)),m_cookieval(new std::string(val))
{
	m_maxAge = -1;	//默认保存时间为连接存在时间(浏览器关闭)
	m_IsSecure = false;
	m_path.reset(new std::string("/")); //携带路径默认为域名下的所有路径
	m_version = 0; //该Cookie使>用的版本号。0表示遵循Netscape的Cookie规范，1表示遵循W3C的RFC 2109规范。
}
cookie::~cookie(){}

const std::string& cookie::getAgeStr()
{
	return *m_expires;
}

void	cookie::setMaxAge(int age)
{
	m_maxAge = age;
	if(m_maxAge == -1)
		return;
	time_t seconds;
	time(&seconds);
	Tools::DateTimeDetail dt = Tools::getCurrentSystemDateTime(seconds + age);
	ptime t(date(dt.year,dt.month,dt.day),time_duration(dt.hour,dt.minute,dt.second));
	date d = t.date();
	format fmt("%1%, %2% %3% %4% %5% GMT");
	fmt % d.day_of_week() % d.day() % d.month() % d.year() % t.time_of_day();

	m_expires.reset(new std::string(fmt.str()));
}

void	cookie::setPath(const std::string& path)
{
	m_path.reset(new std::string(path));
}
void	cookie::setIsSecure(bool secure)
{
	m_IsSecure = secure;
}
void	cookie::setDomain(const std::string& domain)
{
	m_domain.reset(new std::string(domain));
}
void	cookie::setComment(const std::string& comment)
{
	m_comment.reset(new std::string(comment));
}
void	cookie::setVersion(int version)
{
	if(version == 1){
		m_version = 1;
	}else
		m_version = 0;
}
int		cookie::getMaxAge()
{
	return m_maxAge;
}
const std::string& cookie::getPath()
{
	if(m_path.get() == NULL)
		return m_null;
	else
		return *m_path;
}
bool	cookie::getIsSecure()
{
	return m_IsSecure;
}
const std::string& cookie::getDomain()
{
	if(m_domain.get() == NULL)
		return m_null;
	else
		return *m_domain;
}
const std::string& cookie::getComment()
{
	if(m_comment.get() == NULL)
		return m_null;
	else
		return *m_comment;
}
int cookie::getVersion(){
	return m_version;
}
		
const std::string& cookie::getName()
{
	return *m_cookieName;
}

const std::string& cookie::getVal()
{
	return *m_cookieval;
}