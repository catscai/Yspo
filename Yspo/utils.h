
#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <random>
#include <cstdio>
#include <unordered_map>
#include "boost\shared_ptr.hpp"
namespace yspo
{
	using namespace boost;
	using namespace std;
	class Tools
	{
	private:
		typedef boost::shared_ptr<unordered_map<std::string,std::string>> strstrmap_ptr;
	public:
		struct TimeDetail
		{
			unsigned short day;
			unsigned short hour;
			unsigned short minute;
			unsigned short second;
		};
		struct DateTimeDetail
		{
			unsigned short year;
			unsigned short month;
			unsigned short day;
			unsigned short hour;
			unsigned short minute;
			unsigned short second;
		};
	public:
		//��ת��Ϊ�죬ʱ���֣���
		static TimeDetail getTimeDetailByInt(unsigned int time);
		//��ת��Ϊ�꣬�£��գ�ʱ���֣���
		static DateTimeDetail getCurrentSystemDateTime(unsigned int time);
		static std::string getRandomString(size_t len);
		static void	doubleToStr(long double val,string& text);
		static strstrmap_ptr dealRequestContentType(const string& text,const string& type);
		static string  getContentTypeOfFile(const string& fileType);
	};
	
}

#endif