
#ifndef JSONREADER_H
#define JSONREADER_H
#include <string>
#include <fstream>
namespace yspo
{
	using namespace std;
	class JsonReader
	{
	private:
		string			m_str;
		size_t			m_len;
		size_t			m_pos;
	public:
		JsonReader(const string& text):m_str(text),m_len(text.size()),m_pos(0){}
		JsonReader(ifstream* const input){
			char buf[512] = {0};
			while(!input->eof()){
				input->getline(buf,512);
				m_str += buf;
			}
			m_len = m_str.size();
			m_pos = 0;
		}
		~JsonReader(){}
	public:
		bool		isEof(){
			if(m_pos < m_len)
				return false;
			return true;
		}
		char		getChar(){
			if(!isEof()){
				char c = m_str[m_pos++];
				return c;
			}else{
				throw string("JsonReader's buffer is null! Can't continue to read");
			}
		}
		char		nextChar(){
			if(!isEof()){
				char c = m_str[m_pos];
				return c;
			}else{
				throw string("JsonReader's buffer is null! Can't continue to read");
			}
		}
		void		ignoreSpace(){
			while(!isEof()){
				char c = nextChar();
				if(isspace(c)){
					getChar();
				}else
					break;
			}
		}

		void		jmpAll(char c){
				while(c != this->getChar());
		}
		void		match(char c,bool ignoreSpace){
			if(ignoreSpace)
				this->ignoreSpace();
			if(c == this->nextChar())
				this->getChar();
		}
		bool		matchStr(const string& s){
			string str = m_str.substr(m_pos,s.size());
			if(str == s)
				return true;
			return false;
		}
	};
}

#endif