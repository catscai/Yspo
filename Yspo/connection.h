
#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <vector>
#include "boost\enable_shared_from_this.hpp"
#include "boost\shared_ptr.hpp"
#include "boost\asio.hpp"
#include "boost\bind.hpp"
#include "request.h"

namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::asio::ip;
	#define BUFFER_SIZE 4096
	struct Buffer
	{
		char*	m_szbuf;
		size_t	m_len;
		Buffer(size_t len = BUFFER_SIZE):m_szbuf(0),m_len(len){
			m_szbuf = new char[len];
			ZeroMemory(m_szbuf,m_len);
		}
		~Buffer(){
			delete[] m_szbuf;
			m_szbuf = NULL;
			m_len = 0;
		}
		void	reset(){
			ZeroMemory(m_szbuf,m_len);
		}
	};
	class connection : public boost::enable_shared_from_this<connection>
	{
	private:
		tcp::socket					m_socket;
		Buffer						m_buff;
		boost::asio::io_service&	m_ios;
		boost::shared_ptr<request>	m_request;
		boost::shared_ptr<std::string> m_reqStr;
	public:
		connection(boost::asio::io_service& io,boost::asio::io_service& work_io);
		~connection();
	private:
		bool	handler_recv(const boost::system::error_code& error,size_t bytes);
		bool	handler_send(const boost::system::error_code& error,size_t bytes);
		bool	handle_error(const boost::system::error_code& error);
	public:
		void	post_recv();
		void	post_send(boost::shared_ptr<std::string> buff);
		tcp::socket& socket();
		void	on_receive(boost::shared_ptr<std::string> str_ptr);
		void	on_send(size_t bytes);
	};
}

#endif