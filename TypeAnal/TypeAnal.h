#ifndef TypeAnal_H
#define TypeAnal_H
///////////////////////////////////////////////////////////////
// TypeAnal.h -  Utility for type analysis based on TypeTalbe//
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
The Utility-TypeAnal accept a AbstractSyntaxTree object and do
DFS on the root Node of this tree. When reaching a ASTNode, 
extract information from this node and store the information
into TableRecord, then storing the TableRecord into TypeTable

Maintanence Information:
========================
Required files:
---------------
TypeAnal.h, Typetable.h,
AbstrSynTree.h,ConfigurePaser.cpp,
FileSystem.h
*/
#include <iostream>
#include <functional>
#include "../TypeTable/TypeTable.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"

namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		TypeAnal(TypeTable& table);
		~TypeAnal();
		void doTypeAnal(); //do type analysis for TPtable
		void DFS(ASTNode* pNode); //DFS search done on ASTref
		bool doAction(ASTNode* node); //store information for 
		                              //every ASTNode into type table
		bool doDisplay(ASTNode* node);
	private:
		AbstrSynTree& ASTref_; // store the parsing result
		TypeTable& TPtable;
	};
}
#endif
