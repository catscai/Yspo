#include "connection.h"
#include "app.h"
#include "session.h"
using namespace yspo;

connection::connection(boost::asio::io_service& io,boost::asio::io_service& work_io)
			:m_socket(io),m_ios(work_io)
{
	m_reqStr.reset(new std::string);			
}
connection::~connection()
{
	//cout<<"服务器主动关闭连接....."<<endl;
	APP::sessManager->deleSessionInSocket(m_socket);
}

bool	connection::handler_recv(const boost::system::error_code& error,size_t bytes)
{
	if(handle_error(error) == false)
		return false;
	APP::sessManager->resetTimer(m_socket);
	//异步执行接收到消息的处理函数
	boost::shared_ptr<std::string> str_ptr(new std::string(m_buff.m_szbuf,bytes));
	m_ios.post(boost::bind(&connection::on_receive,shared_from_this(),str_ptr));

	//this->on_receive(str_ptr);

	m_buff.reset();
	m_socket.async_read_some(boost::asio::buffer(m_buff.m_szbuf,m_buff.m_len),
		boost::bind(&connection::handler_recv,shared_from_this(),
						_1, _2));
	return true;
}

bool	connection::handler_send(const boost::system::error_code& error,size_t bytes)
{
	if(handle_error(error) == false)
		return false;
	m_ios.post(boost::bind(&connection::on_send,shared_from_this(),bytes));
	return true;
}

bool	connection::handle_error(const boost::system::error_code& error)
{
	bool flag = true;
	if(error == boost::asio::error::connection_reset)
	{//客户端异常关闭socket

		flag = false;
	}else if(error == boost::asio::error::eof)
	{//经过四次挥手的正常关闭

		flag = false;
	}else if(error == boost::asio::error::bad_descriptor)
	{//无效套接字

		flag = false;
	}else if(error == boost::asio::error::operation_aborted)
	{//socket被关闭

		flag = false;
	}
	if(error == 0){

		flag = true;
	}	

	return flag;
}


void	connection::post_recv()
{
	m_buff.reset();
	m_socket.async_read_some(boost::asio::buffer(m_buff.m_szbuf,m_buff.m_len),
		boost::bind(&connection::handler_recv,shared_from_this(), _1,
					_2));
}
void	connection::post_send(boost::shared_ptr<std::string> buff)
{
	/*
	boost::asio::async_write(m_socket,boost::asio::buffer(
		buff->c_str(),buff->size()),boost::asio::transfer_all(),
		boost::bind(&connection::handler_send,shared_from_this(),
		_1,_2));
	*/
	//同步发送
	boost::system::error_code err;
	size_t bytes = m_socket.write_some(boost::asio::buffer(
		buff->c_str(),buff->size()),err);
	//this->handle_error(err);
	//cout<<bytes<<endl;
}
tcp::socket& connection::socket()
{
	return m_socket;
}
void	connection::on_receive(boost::shared_ptr<std::string> str_ptr)
{
	*m_reqStr += *str_ptr;
	auto ite = boost::find_first(*m_reqStr,"Content-Length");
	bool flag = false;
	boost::ends_with(*m_reqStr,"\r\n\r\n") ? flag = true:0;
	if(flag && ite.empty())
	{//只有请求头
		m_request.reset(new request(m_reqStr,m_socket));
	}else if(!ite.empty()){
		auto t = boost::find_first(*m_reqStr,"\r\n\r\n");
		std::string numStr;
		for(auto i = ite.end() + 1; i != m_reqStr->end(); i++){
			if(*i == '\r')
				break;
			numStr += *i;
		}
		auto num = atoi(numStr.c_str());
		auto ncount = m_reqStr->end() - t.end();
		if(ncount >= num){
			auto len = t.begin() - m_reqStr->begin();
			std::string tmp = m_reqStr->substr(len  + 1 + num + numStr.size());
			m_reqStr.reset(new string(m_reqStr->substr(0,len + 1 + num + numStr.size())));
			m_request.reset(new request(m_reqStr,m_socket));
			m_reqStr.reset(new string(tmp));
		}
	}
	if(m_request.get() && !m_request->invalid())
	{//调用处理函数
		APP::app->deal_request(m_request,this);
		m_request.reset();
		m_reqStr.reset(new string);
	}
	//cout<<*str_ptr<<endl;
}
void	connection::on_send(size_t bytes)
{
	//cout<<bytes<<endl;
	//m_socket.close();
}