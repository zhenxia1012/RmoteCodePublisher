#ifndef DEPENDENCYANAL_H
#define DEPENDENCYANAL_H
///////////////////////////////////////////////////////////////
// DependencyAnal.h -  Utility for dependency analysis based //
//                     type table                            //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
The Utility-DependencyAnal if for dependency analysis based on
type table.It will compare table records from type table to 
tokens from every file. Every time it fetches one record from 
type table and compare its type name to tokens from every file 
in file collection. If it succeeds in finding the same token value
as the name of the type in a specific file, it then will try to 
find the same namespace with tokens in that file. Only it finds 
the same type name and same namespace, will it has the dependency 
relationship with that file.

Maintanence Information:
========================
Required files:
---------------
DependencyAnal.h, NoSqlDb.h
TypeAnal.h, Typetable.h,
ConfigurePaser.cpp
*/
#include <iostream>
#include <functional>
#include "../TypeTable/TypeTable.h"
#include "../Parser/ConfigureParser.h"
#include "../NoSqlDb/NoSqlDb.h"

namespace CodeAnalysis
{
	class DependencyAnal
	{
	public:
		DependencyAnal(TypeTable& table, NoSqlDb<std::string>& dtbs_);
		~DependencyAnal();
		void etractFiles(); // extract all the file paths from type table
		void DFS(ASTNode* pNode); // is for extracting all the file paths
		void iniDb(); // initialize the Non-SQL db with every file
		void doDepenAnal(); // do the dependency analysis
		bool doFind(std::string parent, std::string child); // find whether two files 
		                                                    // have been recorded for their dependency
		bool isDep(std::string token, std::string path);  // find whether two files have dependency
		bool doSave(std::string parent, std::string child); // save two dependency into db
	private:
		std::vector<std::string> files; // file paths
		AbstrSynTree& ASTref_; // store the parsing result
		TypeTable& TPtable; // based for dependency analysis
		NoSqlDb<std::string>& db_;// store the denpendency analysis result
	};
}
#endif


