#include "server.h"
using namespace yspo;

server::server(size_t pool_size,unsigned short port):m_service_pool(pool_size),
			m_work_pool(pool_size),m_accepter(m_service_pool.io_service(),
			tcp::endpoint(tcp::v4(),port))
{
			
}
server::~server(){
	this->stop();
}
static int ncount = 0;
bool	server::handler_accept(const boost::system::error_code& error,
	boost::shared_ptr<connection> sess)
{
	if(error == 0){
		sess->post_recv();
		cout<<"创建新连接。。。"<<++ncount<<endl;
		sess.reset(new connection(m_service_pool.io_service(),m_work_pool.io_service()));
		//conns.push_back(sess);
		m_accepter.async_accept(sess->socket(),boost::bind(
		&server::handler_accept,this,_1,sess));
		return true;
	}else{
		return false;
	}
}

bool	server::post_accept()
{
	for(int i = 0; i < 5; i++){
		cout<<"创建新连接。。。"<<++ncount<<endl;
		boost::shared_ptr<connection> sess(new connection(m_service_pool.io_service(),m_work_pool.io_service()));
		//conns.push_back(sess);
		m_accepter.async_accept(sess->socket(),boost::bind(
			&server::handler_accept,this,_1,sess));
	}
	return true;
}

void	server::run()
{
	this->post_accept();
	//使用两个线程执行各自的io_service_pool::run、避免阻塞
	service_thread.reset(new boost::thread(boost::bind(&io_service_pool::run,&m_service_pool)));
	work_thread.reset(new boost::thread(boost::bind(&io_service_pool::run,&m_work_pool)));
}

void	server::stop()
{
	m_service_pool.stop();
	m_work_pool.stop();

	service_thread->join();
	work_thread->join();
}

boost::asio::io_service&  server::getIoService()
{
	return this->m_service_pool.io_service();
}