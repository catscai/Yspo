#include "app.h"
#include "utils.h"
using namespace yspo;
int main()
{
	//Tools::dealRequestContentType("username=zhangsan&age=26","application/x-www-form-urlencoded");
	APP::app.reset(new WebApp("."));
	APP::app->get("/home/index.html",[](req_ptr req,res_ptr res,strs_ptr vec){
		cookie_ptr coo(new cookie("name","caiyanqing"));
		coo->setMaxAge(-1);
		res->addCookie(coo);
		res->send_file("./home/index.html");
		res->reply();
	});

	APP::app->post("/home",[](req_ptr req,res_ptr res,strs_ptr vec){
		cout<<req->value("Body")<<endl;
		//res->addSession(sess);
		res->send_file("./home/index.html");
		res->reply();
	});

	APP::app->get("/cookie",[](req_ptr req,res_ptr res,strs_ptr vec){
		auto coo = req->getCookie("name");
		if(coo.get() != NULL)
		{
			cout<<coo->getVal()<<endl;
		}
		res->location("https://www.baidu.com/");
		res->reply();
	});
	APP::app->open_server(8,8899);
	while(1)
		Sleep(2000);
	std::system("pause");
	return 0;
}