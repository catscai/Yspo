
#ifndef RESPONSE_H
#define	RESPONSE_H
#include <string>
#include <vector>
#include <unordered_map>
#include "boost\shared_ptr.hpp"
#include "boost\regex.hpp"
#include "boost\date_time.hpp"
#include "boost\date_time\posix_time\posix_time.hpp"
#include "boost\lexical_cast.hpp"
#include "boost\format.hpp"
#include "cookie.h"
#include "session.h"
namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;
	using namespace boost::posix_time;
	class connection;
	class response
	{
	private:
		typedef boost::shared_ptr<std::string> str_ptr;
		connection*						m_sess;
		str_ptr							m_file;
		str_ptr							m_body;
		size_t							m_contentLength;
		std::string						m_statusLine;
		std::unordered_map<std::string,str_ptr>	m_param;
		std::vector<boost::shared_ptr<cookie>>	m_cookies;
	public:
		response(connection* sess);
		~response();
	private:
		void	init();
		void	set_param(const std::string& key,const std::string val);
	public:
		void	send_file(const std::string& path);
		void	reply();
		void	addBodyData(const std::string& data);
		void	addCookie(boost::shared_ptr<cookie> coo);
		void	addSession(boost::shared_ptr<session> sess);
		//…Ë÷√œÏ”¶◊¥Ã¨
		void	set_statuscode(int code);
		void	set_ContentType(const std::string& contentType);
		void	set_ContentLength(size_t len);
		void	set_Connection(const std::string& conn);
		void	set_ContentEncoding(const std::string& encode);
		void	set_ContentLanguage(const std::string& language);
		void	set_CacheControl(const std::string& control);
		void	set_ContentTransferEncoding(const std::string& transfer);
		void	location(const std::string& url,int code = 302);
		
	};
}

#endif