#ifndef NOSQLDB_H
#define NOSQLDB_H
///////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database             //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
The NoSqlDb package is acted as a Non-SQL database. It is based
on the unorder-map to manage the data. And it support the operation 
of addition, edition, deletion and so on.

Class Element and NoSqlDb are implemented as templates supporting
any type of data. Element is the single records of the NoSqlDb.

Maintanence Information:
========================
Required files:
---------------
NoSqlDb.h, NoSqlDb.cpp
CppProperties.h, Convert.h
XmlDocument.h, Utilities.h
StrHelper.h
*/

#include <unordered_map>
#include <vector>
#include <iomanip>
#include <fstream> 
#include <io.h>
#include <direct.h> 
#include <windows.h>
#include <thread>
#include "../Utilities/Utilities.h"
#include "../CppProperties/CppProperties.h"
#include "../Convert/Convert.h"
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "StrHelper.h"

using namespace XmlProcessing;
/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
// - in our NoSql database that is just the value in a key/value pair
// - it needs to store child data, something for you to implement
//
template<typename Data>
class Element
{
public:
	using Key = std::string;
	using Name = std::string;
	using Category = std::string;
	using Description = std::string;
	using TimeDate = std::string;
	using Children = std::vector<std::string>;

	Property<Key> key;                    // metadata
	Property<Name> name;                  // metadata
	Property<Category> category;          // metadata
	Property<Description> description;    // metadata
	Property<TimeDate> timeDate;          // metadata
	Children children;                    // metadata
	Property<Data> data;                  // data

	std::string show();
};
//----< show content of Element<Data> >-------------------------------------

template<typename Data>
std::string Element<Data>::show()
{
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(11) << "key" << " : " << key;
	out << "\n    " << std::setw(11) << "name" << " : " << name;
	out << "\n    " << std::setw(11) << "category" << " : " << category;
	out << "\n    " << std::setw(11) << "description" << " : " << description;
	out << "\n    " << std::setw(11) << "children" << " : " << showItems(children);
	out << "\n    " << std::setw(11) << "timeDate" << " : " << timeDate;
	out << "\n    " << std::setw(11) << "data" << " : " << Convert<Data>::toString(data);
	out << "\n";
	return out.str();
}

/////////////////////////////////////////////////////////////////////
// NoSqlDb class is a key/value pair in-memory database
// - stores and retrieves elements
// - you will need to add query capability
//   That should probably be another class that uses NoSqlDb
//
template<typename Data>
class NoSqlDb
{
public:
	using Key = std::string;
	using Keys = std::vector<Key>;

	NoSqlDb() {}
	NoSqlDb(const std::string& name) :dbname(name) {}
	Keys keys();
	bool save(Key key, Element<Data> elem);
	bool edit(Key key, std::string properties, std::string content, bool isadd = false);
	bool del(Key key);
	Element<Data> value(Key key);
	size_t count();
	bool stringToTextFile(const std::string& content, const std::string path);
	std::string textFileToString(const std::string& path);
	bool updatedb();
	bool getFiles(std::string path = "../NoSqlDb/Database");
	void showData();
private:
	using Item = std::pair<Key, Element<Data>>;

	std::unordered_map<Key, Element<Data>> store;
	std::string dbname;
	std::string proot = "../NoSqlDb/Database";
};
//----< return all keys of the database >------------------------------------

template<typename Data>
inline typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
{
	Keys keys;
	for (Item item : store)
	{
		keys.push_back(item.first);
	}
	return keys;
}
//----< insert value >-------------------------------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::save(Key key, Element<Data> elem)
{
	if (store.find(key) != store.end())
	{
		std::cout << "\n  " << key << " has already in database \n";
		return false;
	}
	store[key] = elem;
	return true;
}
//----< delete specific key/value >------------------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::del(Key key)
{
	if (store.find(key) == store.end())
	{
		std::cout << "\n  " << key << " is not in database \n";
		return false;
	}

	Element<Data> tem_elem = value(key);
	std::string rmname = tem_elem.name;
	store.erase(key);

	Keys keys = (*this).keys();
	for (Key pkey : keys)
	{
		try
		{
			Element<Data> *tem_elem = &(store[pkey]);
			std::vector<std::string>::iterator iter = std::find(begin(tem_elem->children), end(tem_elem->children), rmname);
			//std::cout << *iter << std::endl;
			if (iter != end(tem_elem->children))
				tem_elem->children.erase(iter);
		}
		catch (std::exception& ex)
		{
			std::cout << "\n  Delete child " << key << " of " << pkey << " failed \n";
			std::cout << "\n  " << ex.what();
			return false;
		}
	}

	return true;
}
//----< editting value for a specific key >----------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::edit(Key key, std::string properties, std::string content, bool isadd = false)
{
	if (store.find(key) == store.end()){
		std::cout << "\n  " << key << " is not in database \n";
		return false;}
	Element<Data> Elem = value(key);
	try{
		if (properties == "name")
			Elem.name = content;
		else if (properties == "category")
			Elem.category = content;
		else if (properties == "content")
			Elem.description = content;
		else if (properties == "timeDate")
			Elem.timeDate = content;
		else if (properties == "children")
			if (isadd){
				if (key == content)
					std::cout << "\n  can't add yourself as your own child \n";
				if (store.find(content) == store.end()){
					std::cout << "\n  " << content << " are not in database \n";
					return false;}
				std::vector<std::string>::iterator iter = std::find(begin(Elem.children), end(Elem.children), content);
				if (iter != end(Elem.children)){
					std::cout << "\n  " << content << " has been a child of " << Elem.name << std::endl;
					return false;}
				Elem.children.push_back(content);}
			else{
				std::vector<std::string>::iterator iter = std::find(begin(Elem.children), end(Elem.children), content);
				//std::cout << "\n  child:" << *iter << std::endl;
				if (iter == end(Elem.children)){
					std::cout << "\n  " << content << " did not exist \n";
					return false;}
				Elem.children.erase(iter);}
		else if (properties == "data"){
			Data data = Convert<Data>::fromString(content);
			Elem.data = data;}}
	catch (std::exception& ex){
		std::cout << "\n  Edition failed \n";
		std::cout << "\n  " << ex.what();}
	store[key] = Elem;
	return true;
}
//----< return value for a specific key >------------------------------------

template<typename Data>
inline Element<Data> NoSqlDb<Data>::value(Key key)
{
	if (store.find(key) != store.end())
		return store[key];
	return Element<Data>();
}
//----< return number of pairs "key/value" the database >-------------------

template<typename Data>
inline size_t NoSqlDb<Data>::count()
{
	return store.size();
}
//----< read string contents into text file >-------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::stringToTextFile(const std::string& content, const std::string path)
{
	std::ofstream out(path);
	try
	{
		if (out.is_open())
		{
			out << content;
			out.close();
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Write file into \"" + path + "\" failed \n";
		std::cout << "\n  " << ex.what();
		return false;
	}

	return true;
}
//----< read text file contents into string >-------------------------------

template<typename Data>
inline std::string NoSqlDb<Data>::textFileToString(const std::string& path)
{
	std::ostringstream out;
	try
	{
		std::ifstream in(path);
		if (!in.good())
			throw(std::exception(("can't open source file \"" + path + "\"").c_str()));
		out << in.rdbuf();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  " << ex.what() << std::endl;
	}

	return std::move(out.str());
}
//----< update database >---------------------------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::updatedb()
{
	std::string elems_xml = toXml(*this);
	//std::cout << "\n  " << elems_xml;
	std::string path = proot + "/" + dbname + ".xml";
	//std::cout << "\n  " <<path;
	if (!stringToTextFile(elems_xml, proot + "/" + dbname + ".xml"))
		return false;

	return true;
}
//----< get all files into NoSqlDb<Data> >----------------------------------

template<typename Data>
inline bool NoSqlDb<Data>::getFiles(std::string path = "../NoSqlDb/Database")
{
	std::string tem_name = dbname;
	path.append("/" + dbname + ".xml");
	//std::cout << "\n  " << path;
	std::string xml_data = textFileToString(path);
	//std::cout << xml_data;
	(*this) = Persist::fromXml<Data>(xml_data);
	dbname = tem_name;
	return true;
}
//----< show content of values >--------------------------------------------

template<typename Data>
inline void NoSqlDb<Data>::showData()
{
	std::cout << "\n  size of db = " << count();
	Keys keys = this->keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << value(key).show();
	}
}

/////////////////////////////////////////////////////////////////////
// Persistance class
// - you will need to implement that using the XmlDocument class
//   from Handouts/Repository
//
using SPtr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;

class Persist
{
public:
	template<typename Data>
	static std::string toXml(NoSqlDb<Data>& nosqldb);
	template<typename Data>
	static NoSqlDb<Data> fromXml(std::string& xml);
};

//----< transfor NoSqlDb into xml >-----------------------------------------

template<typename Data>
inline std::string Persist::toXml(NoSqlDb<Data>& nosqldb)
{
	std::string xml;
	XmlDocument doc;
	SPtr pRoot = makeTaggedElement("DependencyRelationship");
	doc.docElement() = pRoot;
	std::vector<std::string> pkeys = nosqldb.keys();
	for (std::string item : pkeys) {
		Element<Data> elem = nosqldb.value(item);
		SPtr pElement = makeTaggedElement("Element");
		pRoot->addChild(pElement);
		SPtr pKeyElem = makeTaggedElement("key");// make Property<key> name element and add to root
		pElement->addChild(pKeyElem);
		SPtr pKeyTextElem = makeTextElement(elem.key);
		pKeyElem->addChild(pKeyTextElem);
		SPtr pNameElem = makeTaggedElement("name");// make Property<string> name element and add to root
		pElement->addChild(pNameElem);
		SPtr pNameTextElem = makeTextElement(elem.name);
		pNameElem->addChild(pNameTextElem);
		SPtr pCateElem = makeTaggedElement("category");// make Property<string> category element and add to root
		pElement->addChild(pCateElem);
		SPtr pCateTextElem = makeTextElement(elem.category);
		pCateElem->addChild(pCateTextElem);
		SPtr pDesElem = makeTaggedElement("description");// make Property<string> description element and add to root
		pElement->addChild(pDesElem);
		SPtr pDesTextElem = makeTextElement(elem.description);
		pDesElem->addChild(pDesTextElem);
		SPtr pChildElem = makeTaggedElement("children");// make Property<string> children element and add to root
		pElement->addChild(pChildElem);
		for (std::string item : elem.children) {
			SPtr pSubChildElem = makeTaggedElement("childkey");
			pChildElem->addChild(pSubChildElem);
			SPtr pChildTextElem = makeTextElement(item);
			pSubChildElem->addChild(pChildTextElem);
		}
		SPtr pTimeElem = makeTaggedElement("timeDate");// make Property<string> timeDate element and add to root
		pElement->addChild(pTimeElem);
		SPtr pTimeTextElem = makeTextElement(elem.timeDate);
		pTimeElem->addChild(pTimeTextElem);
		std::string data = Convert<Data>::toString(elem.data);// make Property<string> Data element and add to root
		SPtr pDataElem = makeTaggedElement("data");
		pElement->addChild(pDataElem);
		SPtr pDataTextElem = makeTextElement(data);
		pDataElem->addChild(pDataTextElem);
	}
	xml = doc.toString();
	return xml;
}
//----< transfor xml into value >-------------------------------------------

template<typename Data>
inline NoSqlDb<Data> Persist::fromXml(std::string& xml)
{
	NoSqlDb<Data> tem_db;
	try {
		XmlDocument doc(xml, XmlDocument::str);
		std::vector<SPtr> desc_key = doc.descendents("key").select();
		size_t size = desc_key.size();
		for (size_t i = 0; i < size; i++){
			Element<Data> Elem;
			std::string value = desc_key[i]->children()[0]->value();
			value = trim(value);
			Elem.key = value;
			// fetch the name string from XML element
			std::vector<SPtr> desc = doc.descendents("name").select();
			value = desc[i]->children()[0]->value();
			value = trim(value);
			Elem.name = value;
			// fetch the category string from XML element
			desc = doc.descendents("category").select();
			value = desc[i]->children()[0]->value();
			value = trim(value);
			Elem.category = value;
			// fetch the description string from XML element
			desc = doc.descendents("description").select();
			value = desc[i]->children()[0]->value();
			value = trim(value);
			Elem.description = value;
			// fetch the children string from XML element
			desc = doc.descendents("children").select();
			for (auto item : desc[i]->children()){
				value = item->children()[0]->value();
				value = trim(value);
				Elem.children.push_back(value);}
			// fetch the timeDate string from XML element
			desc = doc.descendents("timeDate").select();
			value = desc[i]->children()[0]->value();
			value = trim(value);
			Elem.timeDate = value;
			// fetch the data string from XML element
			desc = doc.descendents("data").select();
			value = desc[i]->children()[0]->value();
			value = trim(value);
			Data dvalue = Convert<Data>::fromString(value);
			Elem.data = dvalue;

			tem_db.save(Elem.key, Elem);}}
	catch (std::exception& ex){
		std::cout << "\n  something bad happend";
		std::cout << "\n  " << ex.what();}
	return tem_db;
}
#endif