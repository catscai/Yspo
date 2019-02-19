
#ifndef JSON_H
#define JSON_H
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "JsonReader.hpp"
#include "JsonObject.hpp"

namespace yspo
{
	using namespace std;
	class Json
	{
	private:
		JsonObject*		m_root;
	public:
		Json():m_root(0){}
		Json(JsonObject* root):m_root(root){}
		Json(const string& text):m_root(0){
			JsonReader reader(text);
			m_root = parseObject(reader);
		}
		Json(ifstream* input):m_root(0){
			char buf[512] = {0};
			string text;
			while(!input->eof())
			{
				input->getline(buf,512);
				text += buf;
			}
			JsonReader reader(text);
			m_root = parseObject(reader);
		}
		~Json(){
			delete m_root;
			m_root = NULL;
		}
	private:
		JsonString*		parseString(JsonReader& reader){
			string str;
			//str += "\"";
			reader.match('"',true);
			char value = 0;
			while((value = reader.getChar()) != '"')
			{
			if (value == '\\')
				{
					value = reader.getChar();
					switch (value)
					{
					case '"':
						str += "\"";
						break;
					case '\\':
						str += "\\";
						break;
					case '/':
						str += "/";
						break;
					case 'b':
						str += "\b";
						break;
					case 'f':
						str += "\f";
						break;
					case 'n':
						str += "\n";
						break;
					case 'r':
						str += "\r";
						break;
					case 't':
						str += "\t";
						break;
					case 'u':
						throw string("Json parseObject unexpected char!");
						break;
					default:
						throw string("Json parseObject unexpected char!");
						break;
					}
				}
				else
				{
					str += value;
				}
			}
			//str += "\"";
			return new JsonString(str);
		}
		JsonList*		parseList(JsonReader& reader){
			JsonList* jl = new JsonList();
			JsonObject* obj = NULL;
			reader.match('[',true);
			char val = 0;
			while(1)
			{
				if((val = reader.nextChar()) == ']'){
					reader.getChar();
					break;
				}
				try{
					obj = this->parseObject(reader);
				}catch(string s){
					delete jl;
					jl = NULL;
					throw;
				}
				jl->append(obj);
				reader.match(',',true);
			}

			return jl;
		}
		JsonDictory*	parseDictory(JsonReader& reader)
		{
			JsonDictory* jd = new JsonDictory();
			JsonString* js = NULL;
			JsonObject* obj = NULL;
			reader.match('{',true);
			char value = 0;
			while(1)
			{
				if((value = reader.nextChar()) == '}'){
					reader.getChar();
					break;
				}
				try{
					js = parseString(reader);
				}catch(string s){
					delete jd;
					jd = NULL;
					throw;
				}
				reader.match(':',true);
				try{
					obj = parseObject(reader);
				}catch(string s){
					delete jd;
					delete js;
					jd = NULL;
					js = NULL;
					throw;
				}
				jd->append(js->getValue(),obj);
				delete js;
				js = NULL;
				reader.match(',',true);
			}

			return jd;
		}
		JsonNumber*		parseNumber(JsonReader& reader)
		{
			string str;
			reader.ignoreSpace();
			char value = reader.nextChar();
			while(isdigit(value) || value == '-' || value == '.' || value == 'e' || value == 'E')
			{
				reader.getChar();
				str += value;
				value = reader.nextChar();
			}

			return new JsonNumber(atof(str.c_str()));
		}
		JsonBool*		parseBool(JsonReader& reader)
		{
			JsonBool* jb = NULL;
			reader.ignoreSpace();
			if(reader.matchStr("true") || reader.matchStr("True")){
				for(int i = 0; i < 4; i++)
					char c = reader.getChar();
				jb = new JsonBool(true);
			}else if(reader.matchStr("false") || reader.matchStr("False")){
				for(int i = 0; i < 5; i++)
					char c = reader.getChar();
				jb = new JsonBool(false);
			}else{
				throw string("Json parseBool unexpected char");
			}
			return jb;
		}
		JsonNull*		parseNull(JsonReader& reader)
		{
			JsonNull* jn = NULL;
			reader.ignoreSpace();
			if(reader.matchStr("null") || reader.matchStr("Null")){
				for(int i = 0; i < 4; i++)
					reader.getChar();
				jn = new JsonNull();
			}else{
				throw string("Json parseNull unexpected char");
			}
			return jn;
		}
	public:
		JsonObject*		parseObject(JsonReader& reader)
		{
			JsonObject* obj = NULL;
			reader.ignoreSpace();
			char value = reader.nextChar();
			switch(value)
			{
			case '{':
				{
					obj = parseDictory(reader);
				}break;
			case '[':
				{
					obj = parseList(reader);
				}break;
			case '"':
				{
					obj = parseString(reader);
				}break;
			case 't':
			case 'T':
			case 'f':
			case 'F':
				{
					obj = parseBool(reader);
				}break;
			case 'n':
				{
					obj = parseNull(reader);
				}break;
			default:
				{
					if(value == '-' || (isdigit(value) && value != '0')){
						obj = parseNumber(reader);
					}else{
						throw string("Json parseObject unexpected char!");
					}
				}break;
			}
			
			return obj;
		}
		JsonObject*		getRoot(){
			return m_root;
		}
		void			setRoot(JsonObject* root){
			m_root = root;
		}

		void			toString(string& text){
			m_root->toString(text);
		}

		void			toStream(ofstream* output){
			string text;
			this->toString(text);
			output->write(text.c_str(),text.size());
		}
	};
}

#endif