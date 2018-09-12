#ifndef CodePublisher_H
#define CodePublisher_H
///////////////////////////////////////////////////////////////
// CodePublisher.h - Publish Html for files                  //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This package contains only one class-CodePub used for publishing
Html for source code files.

CodePub has two string variables: htmlMd and htmlCAT, which are 
respectively Html Model for Html of source code files and Catalog.
iniHtmlMd is used for initializing the htmlMd and html CAT.

The main functionality of CodePub is that make Html for every 
source code file and embed links, which denotes the dependency 
relashitionship, at the top of the web pages. The dependency is 
acquired from NoSqlDb which will be filled by the Dependency package.

Maintanence Information:
========================
Required files:
---------------
CodePublisher.h, CodePublisher.cpp
NoSqlDb.h, FileSystem.h
*/
#include <string>
#include <vector>
#include <sstream>
#include <fstream> 
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileSystem/FileSystem.h"

namespace CodePublisher
{
	class CodePub
	{
	public:
		CodePub(const NoSqlDb<std::string>& db_);
		void iniHtmlMd(std::string htmdPath = "../CodePublisher/HtmlModel/htmlModel.htm", std::string ctmdPath = "../CodePublisher/HtmlModel/CataModel.htm"); // initialize the HTML model of ".cpp" and ".h" file
		std::string makeHtml(const std::string name, const std::string path); // make Html for a specific file
		std::string mkCata(std::string path = "../CodePublisher/Repository"); // make Html for Catalog of Html files
		void mkAllHtml(std::string svDir = "../CodePublisher/Repository"); // make Html for all files included in NoSqlDb
		std::string rpMKUPChar(std::string file);
		std::string mkDiv(std::string file);
		std::string textFileToString(const std::string& path);
		bool stringToTextFile(const std::string& content, const std::string path);
	private:
		NoSqlDb<std::string> nosqldb; // store the dependency relationship
		std::string htmlMd; // Html Model for source code file
		std::string htmlCAT; // Html Model for Catalog of Html files
	};
}
#endif