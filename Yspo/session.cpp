#include "session.h"
#include <iostream>
using namespace std;
using namespace yspo;

session::session(const std::string& id):m_IsNew(true),m_sock(NULL)
{
	m_sessionid.reset(new std::string(id));
}
session::~session(){}

void	session::addAttribute(const std::string& key,const std::string& val)
{
	m_attributes[key] = boost::shared_ptr<std::string>(new std::string(val));
}

boost::shared_ptr<std::string> session::getAttribute(const std::string& key)
{
	if(m_attributes.count(key))
		return m_attributes[key];
	else
		return boost::shared_ptr<std::string>();
}

const	std::string& session::getSessionId()
{
	return *m_sessionid;
}

vector<boost::shared_ptr<std::string>> session::getAttributeVals()
{
	vector<boost::shared_ptr<std::string>> vec;
	auto ite = m_attributes.begin();
	while(ite != m_attributes.end()){
		vec.push_back(ite->second);
		++ite;
	}
	return vec;
}

vector<std::string> session::getAttributeKeys()
{
	vector<std::string> vec;
	auto ite = m_attributes.begin();
	while(ite != m_attributes.end()){
		vec.push_back(ite->first);
		++ite;
	}
	return vec;
}

bool	session::isNew()
{
	return m_IsNew;
}

void	session::setSocketMark(tcp::socket* s)
{
	m_sock = s;
}


sessionmanager::sessionmanager(boost::asio::io_service& io):m_SessionExistTimeSeconds(30*60),m_ios(io)
{}

sessionmanager::~sessionmanager()
{}

void	sessionmanager::deleSessionInSocket(tcp::socket& sock)
{		
	if(m_sockSessMap.count(&sock)){
		m_sockSessMap.erase(&sock);
	}
}

void	sessionmanager::listFiles(const char * dir,vector<std::string>& files)
{
	intptr_t	handle;
	_finddata_t findData;

	handle = _findfirst(dir, &findData);    // 查找目录中的第一个文件
	if (handle == -1)
	{
		//cout << "Failed to find first file!\n";
		return;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			&& strcmp(findData.name, ".") == 0
			&& strcmp(findData.name, "..") == 0
			)    // 是否是子目录并且不为"."或".."
		{
			//cout << findData.name << "\t<dir>\n";
		}
		else
		{
			files.push_back(std::string(findData.name));
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件

	//cout << "Done!\n";
	_findclose(handle);    // 关闭搜索句柄
}

bool	sessionmanager::IsExist(tcp::socket& sock)
{
	if(m_sockSessMap.count(&sock))
		return true;
	return false;
}

void	sessionmanager::readSessionFromFile()
{
	vector<std::string> files;
	listFiles(m_serializePath.c_str(),files);
	for(size_t i = 0; i < files.size(); i++){
		ifstream input(files[i],ios::in);
		if(input.is_open()){
			boost::shared_ptr<session> sess;
			std::string str;
			input>>str;
			sess.reset(new session(str));
			vector<std::string> ls;
			while(!input.eof())
			{
				input>>str;
				boost::split(ls,str,boost::is_any_of(":"));
				if(ls.size() == 2)
					sess->addAttribute(ls[0],ls[1]);
				ls.clear();
			}
			this->addSession(sess);
			input.close();
		}
	}
}

void	sessionmanager::addSession(boost::shared_ptr<session> sess)
{
	m_sessMap[sess->getSessionId()] = sess;
	m_sockSessMap[sess->m_sock] = sess;
	if(!m_timerMap.count(sess->getSessionId())){
		m_timerMap[sess->getSessionId()].reset(new boost::asio::deadline_timer
			(m_ios,boost::posix_time::seconds(m_SessionExistTimeSeconds)));
		m_timerMap[sess->getSessionId()]->async_wait(boost::bind
			(&sessionmanager::timerStop,this,_1,sess->getSessionId()));
	}
}

boost::shared_ptr<session>	sessionmanager::getSession(const std::string& key)
{
	if(m_sessMap.count(key)){
		m_sessMap[key]->m_IsNew = false;
		return m_sessMap[key];
	}else{
		return boost::shared_ptr<session>();
	}
}

void	sessionmanager::deleSession(const std::string& key)
{
	if(m_sessMap.count(key)){
		if(m_sockSessMap.count(m_sessMap[key]->m_sock))
			m_sockSessMap.erase(m_sessMap[key]->m_sock);
		m_sessMap.erase(key);
		m_timerMap.erase(key);	
	}
}

void	sessionmanager::setPath(const std::string& home)
{
	m_serializePath = home + "/session/";
}

void	sessionmanager::serializeSession(boost::shared_ptr<session> sess)
{
	ofstream out(m_serializePath + sess->getSessionId(),ios::out);
	if(out.is_open()){
		out<<sess->getSessionId()<<endl;
		auto ite = sess->m_attributes.begin();
		while(ite != sess->m_attributes.end())
		{
			out<<ite->first<<':'<<*(ite->second)<<endl;
			++ite;
		}
		out.close();
	}
}

void	sessionmanager::setSessionExistTimeSeconds(size_t seconds)
{
	m_SessionExistTimeSeconds = seconds;
}

size_t	sessionmanager::getSessionExistTimeSeconds()
{
	return m_SessionExistTimeSeconds;
}

bool	sessionmanager::IsExist(const std::string& id)
{
	if(m_sessMap.count(id)){
		return true;
	}
	return false;
}

void	sessionmanager::resetTimer(tcp::socket& sock)
{
	if(m_sockSessMap.count(&sock)){
		m_timerMap[m_sockSessMap[&sock]->getSessionId()]->expires_from_now(boost::posix_time::seconds(m_SessionExistTimeSeconds));
		m_timerMap[m_sockSessMap[&sock]->getSessionId()]->async_wait(boost::bind
			(&sessionmanager::timerStop,this,_1,m_sockSessMap[&sock]->getSessionId()));
	}
}

void	sessionmanager::timerStop(const boost::system::error_code& err,const string& sessionid)
{
	if(err){
		cout<<"定时器被取消"<<endl;
	}else{
		if(m_timerMap[sessionid]->expires_at() <= boost::asio::deadline_timer::traits_type::now()){
			cout<<"定时时间结束"<<endl;
			this->deleSession(sessionid);
		}else{
			cout<<"时间还没到"<<endl;
		}
	}
			
}