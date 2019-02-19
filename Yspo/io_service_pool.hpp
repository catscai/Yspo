
#ifndef IO_SERVICE_POOL_HPP
#define IO_SERVICE_POOL_HPP
#include <string>
#include <vector>
#include "boost\shared_ptr.hpp"
#include "boost\asio.hpp"
#include "boost\thread.hpp"
#include "boost\bind.hpp"
namespace yspo
{
	using namespace std;
	using namespace boost;
	class io_service_pool
	{
	private:
		typedef boost::shared_ptr<boost::asio::io_service>			io_service_ptr;
		typedef boost::shared_ptr<boost::asio::io_service::work>	io_work_ptr;
		volatile size_t												next_io_service;
		std::vector<io_service_ptr>									service_ptr_lst;
		std::vector<io_work_ptr>									work_ptr_lst;
	public:
		io_service_pool(size_t numOfPool):next_io_service(0){
			for(size_t i = 0; i < numOfPool; i++){
				io_service_ptr isp(new boost::asio::io_service);
				io_work_ptr    iwp(new boost::asio::io_service::work(*isp));
				service_ptr_lst.push_back(isp);
				work_ptr_lst.push_back(iwp);
			}
		}
		~io_service_pool(){
			for(auto io : service_ptr_lst)
				io->stop();
			service_ptr_lst.clear();
			work_ptr_lst.clear();
		}
	public:
		void	run(){
			//创建线程池、各自执行io_service::run
			std::vector<boost::shared_ptr<boost::thread>> thread_ptr_lst;
			for(size_t i = 0; i < service_ptr_lst.size(); i++){
				boost::shared_ptr<boost::thread> thread_ptr(
					new boost::thread(boost::bind(&boost::asio::io_service::run,
					service_ptr_lst[i].get())));
				thread_ptr_lst.push_back(thread_ptr);
			}

			//等待回收线程
			for(auto t : thread_ptr_lst)
				t->join();
		}
		void	stop(){
			for(auto io : service_ptr_lst)
				io->stop();
		}
		boost::asio::io_service& io_service(){
			boost::asio::io_service& ios = *service_ptr_lst[next_io_service];
			++next_io_service;
			if(next_io_service == service_ptr_lst.size()){
				next_io_service = 0;
			}
			return ios;
		}
	};
}

#endif