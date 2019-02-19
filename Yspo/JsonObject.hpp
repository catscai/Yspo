
#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include <string>
#include <vector>
#include <unordered_map>
#include "utils.h"
namespace yspo
{
	using namespace std;
	enum JSONVALUE_TYPE{
		JSON_NULL,
		JSON_NUM,
		JSON_STRING,
		JSON_LIST,
		JSON_DICT,
		JSON_BOOL
	};

	class JsonDictory;
	class JsonList;
	class JsonString;
	class JsonBool;
	class JsonNumber;
	class JsonNull;

	class JsonObject	//×÷Îª¸¸Àà
	{
	private:
		JSONVALUE_TYPE  m_objType;
	public:
		explicit JsonObject(JSONVALUE_TYPE type):m_objType(type){}
		virtual ~JsonObject(){}
	public:
		virtual	void			toString(string& text){}
	public:
		virtual JsonString*		toJsonString(){
			return NULL;
		}
		virtual JsonList*		toJsonList(){
			return NULL;
		}
		virtual JsonDictory*	toJsonDictory(){
			return NULL;
		}
		virtual JsonBool*		toJsonBool(){
			return NULL;
		}
		virtual JsonNumber*		toJsonNumber(){
			return NULL;
		}
		virtual JsonNull*		toJsonNull(){
			return NULL;
		}
	public:
		JSONVALUE_TYPE	getObjType(){
			return m_objType;
		}
	};

	class JsonString : public JsonObject
	{
	private:
		string			m_str;
	public:
		JsonString():JsonObject(JSON_STRING){}
		JsonString(const string& text):m_str(text),JsonObject(JSON_STRING){}
		~JsonString(){}
	public:
		virtual JsonString*		toJsonString(){
			return this;
		}
		virtual	void			toString(string& text){
			text = "\"";
			for(size_t i = 0; i < m_str.size(); i++)
			{
				char value = m_str[i];
				switch(value)
				{
				case '"':
					text += "\"";
					break;
				case '\\':
					text += "\\";
					break;
				case '/':
					text += "/";
					break;
				case '\b':
					text += "\b";
					break;
				case '\f':
					text += "\f";
					break;
				case '\n':
					text += "\n";
					break;
				case '\r':
					text += "\r";
					break;
				case '\t':
					text += "\t";
					break;
				default:
					text += value;
					break;
				}
			}
			text += "\"";
		}
	public:
		string&		getValue(){
			return m_str;
		}
		const string&	getValue() const{
			return m_str;
		}
		void		setValue(const string& text){
			m_str = text;
		}
		void		append(char c){
			m_str += c;
		}
		void		append(const string& s){
			m_str += s;
		}
		void		clear(){
			m_str.clear();
		}
	};

	class JsonList : public JsonObject
	{
	private:
		vector<JsonObject*>		m_objList;
	public:
		JsonList():JsonObject(JSON_LIST){}
		JsonList(const vector<JsonObject*>& lst):m_objList(lst),JsonObject(JSON_LIST){}
		~JsonList(){
			this->clear();
		}
	public:
		virtual JsonList*		toJsonList(){
			return this;
		}
		virtual	void			toString(string& text){
			text = "";
			string tmp = "";
			text += "[";
			for(size_t i = 0; i < m_objList.size(); i++)
			{
				m_objList[i]->toString(tmp);
				text += tmp;
				if(i != m_objList.size() - 1)
					text += ",";
			}
			text += "]";
		}
	public:
		vector<JsonObject*>& getValue(){
			return m_objList;
		}
		const vector<JsonObject*>&	getValue() const{
			return m_objList;
		}
		void				setValue(vector<JsonObject*>& lst){
			m_objList = lst;
		}
		void				append(JsonObject* obj){
			m_objList.push_back(obj);
		}
		void				clear(){
			for(size_t i = 0; i < m_objList.size(); i++)
			{
				delete m_objList[i];
				m_objList[i] = NULL;
			}
			m_objList.clear();
		}
	};

	class JsonDictory : public JsonObject
	{
	private:
		unordered_map<string,JsonObject*>	m_objDict;
	public:
		JsonDictory():JsonObject(JSON_DICT){}
		JsonDictory(const unordered_map<string,JsonObject*>& dict):m_objDict(dict),
			JsonObject(JSON_DICT){}
		~JsonDictory(){
			this->clear();
		}
	public:
		virtual	JsonDictory*	toJsonDictory(){
			return this;
		}
		virtual	void			toString(string& text){
			text = "";
			string tmp = "";
			text += "{";
			auto ite = m_objDict.begin();
			while(ite != m_objDict.end())
			{
				ite->second->toString(tmp);
				text += "\"" + ite->first + "\"";
				text += ":";
				text += tmp;
				++ite;
				if(ite != m_objDict.end())
					text += ",";
			}
			text += "}";
		}
	public:
		bool			isExist(const string& key){
			if(m_objDict.count(key))
				return true;
			return false;
		}
		JsonObject*		getValue(const string& key){
			if(isExist(key)){
				return m_objDict[key];
			}
			return NULL;
		}
		unordered_map<string,JsonObject*>&		getValue(){
			return m_objDict;
		}
		const unordered_map<string,JsonObject*>&	getValue() const{
			return m_objDict;
		}
		void			setValue(unordered_map<string,JsonObject*> pp){
			m_objDict = pp;
		}
		void			append(const string& key,JsonObject* obj){
			m_objDict[key] = obj;
		}
		void			clear(){
			auto ite = m_objDict.begin();
			while(ite != m_objDict.end())
			{
				delete ite->second;
				ite->second = NULL;
				++ite;
			}
			m_objDict.clear();
		}
	};

	class JsonBool : public JsonObject
	{
	private:
		bool		m_bFlag;
	public:
		JsonBool():m_bFlag(false),JsonObject(JSON_BOOL){}
		JsonBool(bool val):m_bFlag(val),JsonObject(JSON_BOOL){}
		~JsonBool(){}
	public:
		virtual	JsonBool*		toJsonBool(){
			return this;
		}
		virtual	void			toString(string& text){
			if(m_bFlag){
				text = "true";
			}
			text = "false";
		}
	public:
		bool		getValue(){
			return m_bFlag;
		}
		bool		getValue() const{
			return m_bFlag;
		}
		void		setValue(bool flag){
			m_bFlag = flag;
		}
	};

	class JsonNumber : public JsonObject
	{
	private:
		long double			m_number;
	public:
		JsonNumber():m_number(0),JsonObject(JSON_NUM){}
		JsonNumber(long double val):m_number(val),JsonObject(JSON_NUM){}
		~JsonNumber(){}
	public:
		virtual JsonNumber*		toJsonNumber(){
			return this;
		}
		virtual	void			toString(string& text){
			doubleToStr(m_number,text);
		}
	public:
		long double			getValueLDouble(){
			return m_number;
		}
		double				getValueDouble(){
			return static_cast<double>(m_number);
		}
		int					getValueInt(){
			return static_cast<int>(m_number);
		}
		long double			getValueLDouble() const{
			return m_number;
		}
		double				getValueDouble() const{
			return static_cast<double>(m_number);
		}
		int					getValueInt() const{
			return static_cast<int>(m_number);
		}
		void				setValue(long double val){
			m_number = val;
		}
	};

	class JsonNull : public JsonObject
	{
	public:
		JsonNull():JsonObject(JSON_NULL){}
		~JsonNull(){}
	public:
		virtual JsonNull*	toJsonNull(){
			return this;
		}
		virtual void		toString(string& text){
			text = "null";
		}
	public:
		string		getValue(){
			return "null";
		}
	};
}

#endif