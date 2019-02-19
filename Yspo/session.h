
#ifndef SESSION_H
#define SESSION_H
#include "boost\shared_ptr.hpp"
#include "boost\regex.hpp"
#include "boost/asio.hpp"
#include "boost\thread\mutex.hpp"
#include "boost\tokenizer.hpp"
#include "boost\algorithm\string.hpp"
#include "boost\bind.hpp"
//#include "connection.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <io.h>
namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::asio;
	using namespace boost::asio::ip;
	class sessionmanager;
	class session
	{
	private:
		boost::shared_ptr<std::string>				m_sessionid;
		unordered_map<std::string,boost::shared_ptr<std::string>>  m_attributes;
		bool										m_IsNew;
		tcp::socket*								m_sock;
	public:
		session(const std::string& id);
		~session();
	private:
		friend sessionmanager;
	public:
		void	addAttribute(const std::string& key,const std::string& val);
		boost::shared_ptr<std::string> getAttribute(const std::string& key);
		const	std::string& getSessionId();
		vector<boost::shared_ptr<std::string>> getAttributeVals();
		vector<std::string> getAttributeKeys();
		bool	isNew();
		void	setSocketMark(tcp::socket* s);
	};

	class connection;
	class sessionmanager
	{
	private:
		unordered_map<std::string,boost::shared_ptr<session>>  m_sessMap;
		unordered_map<tcp::socket*,boost::shared_ptr<session>> m_sockSessMap;
		std::string											m_serializePath;
		size_t												m_SessionExistTimeSeconds;
		unordered_map<std::string,boost::shared_ptr<boost::asio::deadline_timer>>  m_timerMap;
		boost::asio::io_service&							m_ios;
	public:
		sessionmanager(boost::asio::io_service& io);
		~sessionmanager();
	private:
		friend connection;
		void	deleSessionInSocket(tcp::socket& sock);
		void	listFiles(const char * dir,vector<std::string>& files);
		bool	IsExist(tcp::socket& sock);
		void	resetTimer(tcp::socket& sock);
		void	timerStop(const boost::system::error_code& err,const string& sessionid);
	public:
		void	readSessionFromFile();
		void	addSession(boost::shared_ptr<session> sess);
		boost::shared_ptr<session>	getSession(const std::string& key);
		void	deleSession(const std::string& key);
		void	setPath(const std::string& home);
		void	serializeSession(boost::shared_ptr<session> sess);
		void	setSessionExistTimeSeconds(size_t seconds);
		size_t	getSessionExistTimeSeconds();
		bool	IsExist(const std::string& id);

	};
}

#endif