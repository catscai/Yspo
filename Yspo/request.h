
#ifndef REQUEST_H
#define REQUEST_H
#include <unordered_map>
#include <string>
#include "boost\shared_ptr.hpp"
#include "boost\asio.hpp"
#include "cookie.h"
#include "session.h"
#include "utils.h"
namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::asio::ip;
	class request
	{
	private:
		typedef std::unordered_map<std::string,std::string> param_map;
		boost::shared_ptr<std::string>	m_reqStr;
		std::string						m_path;
		std::string						m_method;
		std::string						m_version;
		std::vector<std::string>		m_keyLst;
		std::string						m_null;
		bool							m_isNull;
		param_map						 m_Param;
		param_map						m_Attribute;
		std::vector<boost::shared_ptr<cookie>>		m_cookies;
		tcp::socket&					m_sock;
	public:
		request(boost::shared_ptr<std::string> str_ptr,tcp::socket& s);
		~request();
	private:
		void	dealtext();
	public:
		const std::string& operator[](const std::string& key);
		const std::string& to_string();
		const std::string& version();
		const std::string& method();
		const std::vector<std::string>& keys();
		const std::string& value(const std::string& key);
		const std::string& path();
		bool	invalid();
		void	addAttribute(const std::string& key,const std::string& val);
		const std::string& getAttribute(const std::string& key);
		void	sendRequest(const std::string& url);
		boost::shared_ptr<cookie> getCookie(const std::string& key);
		const std::vector<boost::shared_ptr<cookie>>& getCookies();
		boost::shared_ptr<session> getSession();
	};
}

#endif