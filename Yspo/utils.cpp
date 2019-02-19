#include "utils.h"
#include "boost\regex.hpp"
#include <iostream>
using namespace yspo;

Tools::TimeDetail Tools::getTimeDetailByInt(unsigned int time)
{
	TimeDetail detail = { 0, 0, 0, 0 };
 
	detail.day = (int)(time / (24*60*60));
	time -= detail.day * (24*60*60);
 
	detail.hour = (int)(time / (60*60));
	time -= detail.hour * (60*60);
 
	detail.minute = (int)(time / 60);
	detail.second = time - detail.minute * 60;
 
	return detail;
}
 
//秒转换为年，月，日，时，分，秒
Tools::DateTimeDetail Tools::getCurrentSystemDateTime(unsigned int time)
{
	TimeDetail timeDetail = getTimeDetailByInt(time);
	DateTimeDetail	dateTimeDetail;
	dateTimeDetail.year = 1970;
	dateTimeDetail.month = 1;
	dateTimeDetail.day = 1;
	dateTimeDetail.hour = timeDetail.hour + 8;//时区导致
	dateTimeDetail.minute = timeDetail.minute;
	dateTimeDetail.second = timeDetail.second;
 
	int daysLeft = timeDetail.day;
	int daysOfYear;
	while (daysLeft > 0)
	{
		if ((((dateTimeDetail.year % 4) == 0) && (dateTimeDetail.year % 100) != 0) || (dateTimeDetail.year % 400 == 0))
			daysOfYear = 366;
		else
			daysOfYear = 365;
 
		if (daysLeft < daysOfYear)
		{
			int daysOfMonth;
			while (daysLeft > 0)
			{
				switch (dateTimeDetail.month)
				{
				case 1:case 3:case 5:case 7:case 8:case 10:case 12:
					daysOfMonth = 31;
					break;
				case 4:case 6:case 9:case 11:
					daysOfMonth = 30;
					break;
				case 2:
					daysOfMonth = (daysOfYear == 365) ? 28 : 29;
				default:
					break;
				}
 
				if (daysLeft < daysOfMonth)
				{
					dateTimeDetail.day = daysLeft + 1;
					daysLeft = 0;
				}
				else
				{
					daysLeft -= daysOfMonth;
					dateTimeDetail.month++;
				}
			}
		}
		else
		{
			daysLeft -= daysOfYear;
			dateTimeDetail.year++;
		}
	}
 
	return	dateTimeDetail;
}

std::string Tools::getRandomString(size_t len)
{
	char selects[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	std::mt19937 rng;
	rng.seed(std::random_device()());
	uniform_int_distribution<unsigned> u(0, 61);
	std::string key;
	for(size_t i = 0; i < len; i++){
		key += selects[u(rng)];
	}
	return key;
}

void		Tools::doubleToStr(long double val,string& text)
{
	char buf[50] = {0};
	sprintf_s(buf,50,"%llf",val);
	text = buf;
	if(text[text.size() - 1] == '0'){
		auto pos = text.find_last_not_of('0');
		text = text.substr(0,pos + 1);
	}
	if(text[text.size() - 1] == '.'){
		text = text.substr(0,text.size() - 1);
	}
}

Tools::strstrmap_ptr Tools::dealRequestContentType(const string& text,const string& type)
{
	strstrmap_ptr result(new unordered_map<string,string>());
	if(type == "application/x-www-form-urlencoded"){
		boost::regex reg("([a-zA-Z]\\w+)=([^&]*)&?");
		boost::smatch match;
		auto  ite1 = text.cbegin();
		auto  ite2 = text.cend();
		while(boost::regex_search(ite1,ite2,match,reg))
		{
			(*result)[match[1].str()] = match[2].str();
			ite1 = match[1].second;
		}
	}
	return result;
}

string  Tools::getContentTypeOfFile(const string& fileType)
{

	return "";
}