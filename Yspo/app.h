
#ifndef WEBAPP_H
#define WEBAPP_H
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "boost\format.hpp"
#include "boost\regex.hpp"
#include "server.h"
#include "request.h"
#include "response.h"
namespace yspo
{
	using namespace std;
	using namespace boost;
	class response;
	class WebApp final
	{
	private:
		enum METHOD{GET,POST};
		typedef std::function<void(boost::shared_ptr<request>,
			boost::shared_ptr<response>,boost::shared_ptr<std::vector<std::string>>)> HandlerType;
		unordered_map<METHOD,unordered_map<std::string,HandlerType>> RouteMap;
		boost::shared_ptr<server>	m_ser;
		bool						m_IsOpenFlag;
		unordered_map<std::string,std::string>	regexMap;
		std::string								m_homedirectory;
	public:
		WebApp(const string& homePath = ".");
		~WebApp();
	private:
		std::string	cal(const std::string& str,METHOD m);
	public:
		void	open_server(size_t queue_size,unsigned short port);
		void	deal_request(boost::shared_ptr<request> req,connection* sess);
		void	get(const std::string& route,HandlerType ff);
		void	post(const std::string& route,HandlerType ff);
	};
	class sessionmanager;
	namespace APP
	{
		extern boost::shared_ptr<WebApp> app;
		extern boost::shared_ptr<sessionmanager> sessManager;
	}
	
	typedef boost::shared_ptr<request> req_ptr;
	typedef boost::shared_ptr<response> res_ptr;
	typedef boost::shared_ptr<vector<std::string>> strs_ptr;
	typedef boost::shared_ptr<std::string> str_ptr;
	typedef boost::shared_ptr<cookie> cookie_ptr;
	typedef boost::shared_ptr<session> session_ptr;
	typedef unordered_map<std::string,std::string> strstrmap;
}
#endif