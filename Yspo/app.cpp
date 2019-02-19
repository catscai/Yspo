#include "app.h"
#include "response.h"
using namespace yspo;

boost::shared_ptr<WebApp> APP::app;
boost::shared_ptr<sessionmanager> APP::sessManager;

WebApp::WebApp(const string& homePath):m_IsOpenFlag(false)
{
	regexMap["digital"] = "(\\d+)";
	regexMap["string"] = "(\\w+)";
	regexMap["alphabet"] = "([a-zA-Z]+)";
	regexMap["upper"] = "([A-Z]+)";
	regexMap["lower"] = "([a-z]+)";
	m_homedirectory = homePath;
}

WebApp::~WebApp()
{
	if(m_IsOpenFlag){
		m_ser->stop();
	}
}

std::string	WebApp::cal(const std::string& str,METHOD m)
{
	//先判断用户设定的路由是否为空
	if(str.empty())
		return "";
	boost::format fmt("%1%");
	char buf[100] = {0};
	//用正则表达式匹配用户在路由中设置的变量、例如<id:int>
	boost::regex reg("<([a-zA-Z]+:[a-zA-Z]+)>+");
	boost::smatch match;
	auto  ite1 = str.cbegin();
	auto  ite2 = str.cend();
	vector<std::string> tmp;
	std::string s;
	while(boost::regex_search(ite1,ite2,match,reg))
	{
		tmp.push_back(match[1].str());
		ite1 = match[1].second;
	}
	//将找到的变量对应的正则表达式的语法 拼接到s上
	int pos = 0;
	bool flag = false;
	for(size_t i = 0; i < str.size(); i++){
		if(str[i] == '<' && !flag){
			vector<std::string> ls;
			if(tmp.size() > pos){
				boost::split(ls,tmp[pos++],boost::is_any_of(":"));
			}
			if(ls.size() == 2){
				if(regexMap.count(ls[1]))
					s += regexMap[ls[1]];
				else
					s += ls[1];
			}
			flag = true;
		}
		if(!flag){
			s += str[i];
		}
		else if(str[i] == '>' && flag){
			flag = false;
		}
	}
			
	fmt % s;
	return fmt.str();
}

void	WebApp::open_server(size_t queue_size,unsigned short port)
{
	if(m_IsOpenFlag == false){
		m_ser.reset(new server(queue_size,port));
		m_ser->run();
		m_IsOpenFlag = true;
		APP::sessManager.reset(new sessionmanager(m_ser->getIoService()));
		APP::sessManager->setPath(this->m_homedirectory);
	}
}

void	WebApp::deal_request(boost::shared_ptr<request> req,connection* sess)
{
	boost::shared_ptr<response> res(new response(sess));
	METHOD m;
	bool   IsFile = false;
	req->method() == "GET" ? m = GET:m = POST;
	boost::regex reg("(/.*)+\\.[a-zA-Z0-9]+");
	IsFile = boost::regex_match(req->path(),reg);
	if(IsFile){
		ifstream fin(m_homedirectory + req->path());
		if(!fin.is_open()){//文件不存在
			res->set_statuscode(404);
			res->reply();
			return;
		}else
			fin.close();
	}
	HandlerType* pf = NULL;
			
	auto& mt = RouteMap[m];
	auto ite = mt.begin();
	while(ite != mt.end()){
		reg = ite->first;
		if(boost::regex_match(req->path(),reg)){
			pf = &(ite->second);
			break;
		}
		++ite;
	}
	if(ite == mt.end()){//没有找到匹配的路由、若是文件直接发送、如果不是则返回400
		if(IsFile){
			res->send_file(m_homedirectory + req->path());
		}else
			res->set_statuscode(400);
		res->reply();
	}else{//找到匹配的路由
		boost::shared_ptr<vector<std::string>> vec(new vector<std::string>);
		boost::smatch match;
		auto  ite1 = req->path().cbegin();
		auto  ite2 = req->path().cend();
		while(boost::regex_search(ite1,ite2,match,reg)){
			vec->push_back(match[1].str());
			ite1 = match[1].second;
		}
		(*pf)(req,res,vec);
	}
}

void	WebApp::get(const std::string& route,HandlerType ff)
{
	RouteMap[GET][cal(route,GET)] = ff;
}

void	WebApp::post(const std::string& route,HandlerType ff)
{
	RouteMap[POST][cal(route,POST)] = ff;
}

