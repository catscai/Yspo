
#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "boost\regex.hpp"
#include "boost\format.hpp"
#include "boost\bind.hpp"
#include "boost/enable_shared_from_this.hpp"
namespace yspo
{
	using namespace std;
	using namespace boost;
	
	class Template:public boost::enable_shared_from_this<Template>
	{
	private:
		typedef unordered_map<std::string,std::string>  replace_map;
		ifstream		input;
		static          replace_map*  remap;
	public:
		Template(){}
		~Template(){}
	private:
		void load_file(std::string& s, std::ifstream& is)
		{
			s.erase();
			if(is.bad()) return;
			s.reserve(static_cast<std::string::size_type>(is.rdbuf()->in_avail()));
			char c;
			while(is.get(c))
			{
				if(s.capacity() == s.size())
					s.reserve(s.capacity() * 3);
				s.append(1, c);
			}
		}
		static std::string regex_callback(boost::match_results<std::string::const_iterator> match){
			auto s = match[0].str();
			s = s.substr(2);
			s = s.substr(0,s.size() - 2);
			if((*remap).count(s)){
				return (*remap)[s];
			}else
				return match[0].str();
		}

		boost::shared_ptr<std::string> generate(const std::string& text,replace_map& pp)
		{
			remap = &pp;
			boost::regex reg("\\{\\{(\\w+)\\}\\}");
			//函数指针只能使用静态函数或全局函数、且不能被绑定boost::bind
			boost::shared_ptr<std::string> finish(new std::string(boost::regex_replace(text,reg,
				regex_callback,
				boost::match_default | boost::format_all)));
			return finish;
		}
	public:
		boost::shared_ptr<std::string> templates(const std::string& path,replace_map& pp){
			input.open(path,std::ios::in);
			if(input.bad())
				return boost::shared_ptr<std::string>();
			std::string inbuf;
			load_file(inbuf,input);
			input.close();
			return this->generate(inbuf,pp);
		}

		boost::shared_ptr<std::string> templates(const std::string& text,replace_map& pp)
		{
			return this->generate(text,pp);
		}
	};
	Template::replace_map* Template::remap = NULL;
}

#endif