
#ifndef	SERVER_H
#define SERVER_H
#include "io_service_pool.hpp"
#include "boost\asio.hpp"
#include "connection.h"
namespace yspo
{
	using namespace std;
	using namespace boost;
	using namespace boost::asio::ip;
	class server
	{
	private:
		io_service_pool				m_service_pool;
		io_service_pool				m_work_pool;
		tcp::acceptor				m_accepter;
		boost::shared_ptr<boost::thread>	service_thread;
		boost::shared_ptr<boost::thread>	work_thread;
		//vector<boost::shared_ptr<connection>> conns;
	public:
		server(size_t pool_size,unsigned short port);
		~server();
	private:
		bool	handler_accept(const boost::system::error_code& error,
			boost::shared_ptr<connection> sess);
		bool	post_accept();
	public:
		void	run();
		void	stop();
		boost::asio::io_service&  getIoService();
	};
}

#endif
