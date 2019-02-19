
#ifndef COOKIE_H
#define COOKIE_H
#include <string>
#include <ctime>
#include "boost\shared_ptr.hpp"
#include "boost\date_time.hpp"
#include "boost\format.hpp"
#include "utils.h"
namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;
	using namespace boost::posix_time;
	class cookie
	{
	private:
		boost::shared_ptr<std::string>	m_cookieName;
		boost::shared_ptr<std::string>  m_cookieval;
		boost::shared_ptr<std::string>  m_path;
		int								m_maxAge;
		bool							m_IsSecure;
		int								m_version;
		boost::shared_ptr<std::string>  m_domain;
		boost::shared_ptr<std::string>  m_comment;
		boost::shared_ptr<std::string>  m_expires;
		std::string						m_null;
	public:
		cookie(const std::string& key,const std::string& val);
		~cookie();
	public:
		const std::string& getAgeStr();
		void	setMaxAge(int age);
		void	setPath(const std::string& path);
		void	setIsSecure(bool secure);
		void	setDomain(const std::string& domain);
		void	setComment(const std::string& comment);
		void	setVersion(int version);
		int		getMaxAge();
		const std::string& getPath();
		bool	getIsSecure();
		const std::string& getDomain();
		const std::string& getComment();
		const std::string& getName();
		const std::string& getVal();
		int		getVersion();
	};
}

#endif