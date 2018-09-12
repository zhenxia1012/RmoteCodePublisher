
/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database                 //
//                                                                 //
// Language:    Visual C++                                         //
// Platform:    Alienware15, Windows 10                            //
// Author:      Zhen Xia, CIS687, Syracuse University              //
/////////////////////////////////////////////////////////////////////
#include "NoSqlDb.h"
#include <iostream>

#ifdef NOSQLDB_H
#define NOSQLDB_H

using Utils = Utilities::StringHelper;
using StrData = std::string;
using Key = NoSqlDb<StrData>::Key;
using Keys = NoSqlDb<StrData>::Keys;

int main()
{
	Utils::Title("Demonstrate for Project1-Key/Value Database requirements");
	Utilities::putline();

	Utils::Title("Demonstrate a template class providing key/value Req#2");
	Utils::Title("Demonstrate structure of NoSqlDb elements with string data");
	NoSqlDb<StrData> dbOfStr("StrData");

	Utils::Title("Retrieving string elements from local memory to StrData database Req#5");
	dbOfStr.getFiles();
	dbOfStr.showData();
	Utilities::putline();

	Utils::Title("Demonstrate addtion, deletion, insertion of NoSqlDb<StrData> Req#3,4");
	Utils::Title("Demonstrate addtion of NoSqlDb<StrData> Req#3");
	std::cout << "\n  add the key/value \"elem7\" \n";
	Element<StrData> elem7;
	elem7.key = "elem7";
	elem7.name = "elem7";
	elem7.category = "exam1";
	elem7.description = "elem7's description";
	elem7.timeDate = "none";
	elem7.data = "elem7's StrData from exam1";
	dbOfStr.save(elem7.name, elem7);

	Utils::Title("Demonstrate deletion of NoSqlDb<StrData> Req#3");
	std::cout << "\n  delete the key/value \"elem8\" \n";
	dbOfStr.del("elem8");

	Utils::Title("Demonstrate edition of NoSqlDb<StrData> Req#4");
	std::cout << "\n  revise category to \"practice2\" for \"elem6\" \n";
	dbOfStr.edit("elem6", "category", "practice2");
	std::cout << "\n  add children \"elem5\" for \"elem4\" \n";
	dbOfStr.edit("elem4", "children", "elem5", true);
	std::cout << "\n  delete children \"elem2\" for \"elem4\" \n";
	dbOfStr.edit("elem4", "children", "elem2");
	std::cout << "\n  revise data to \"elem6's StrData from practice2\" for \"elem6\" \n";
	dbOfStr.edit("elem6", "data", "elem6's StrData from practice2");
	Utils::Title("Show elements after edition ");
	dbOfStr.showData();
	std::string xmldb = Persist::toXml(dbOfStr);
	std::cout << xmldb;
}
#endif