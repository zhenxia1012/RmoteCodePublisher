#ifndef TypeTable_H
#define TypeTable_H
///////////////////////////////////////////////////////////////
// Typetable.h - structure for store the type information    //
//               from file collection and it is the base for //
//               dependency analysis                         //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
The Type table consists of two parts: TypeTable class and 
TableRecord structure.

For TypeTable, the main part is a vector which stores pointers
of TableRecord. And, it has a series of operations to operate 
the type table.

For TypeRecord, it stands for a record in type table. It stores
some type information, such as type, name, path and so on.

Maintanence Information:
========================
Required files:
---------------
Typetable.h, Typetable.cpp
*/
#include <string>
#include <vector>
#include <sstream>

namespace CodeAnalysis
{
	struct TableRecord
	{
		using Property = std::string;

		TableRecord();
		TableRecord(const Property& type, const Property& name);
		~TableRecord();
		Property type_;
		Property parentType_;
		Property name_;
		Property package_;
		Property namespace_;
		Property path_; //belong to which file
		std::string show(bool details = false); // show the information of one record
	};

	class TypeTable
	{
		using Property = std::string;

	public:
		TypeTable(const Property& name);
		~TypeTable();
		std::vector<TableRecord *>& getTable(); //return the tablerecords
		void addRecord(TableRecord* tablerecord);// add one record to tablerecords
		std::string show(); //show all the information of the type table-tablerecords
	private:
		Property tablename;
		std::vector<TableRecord*> tablerecords; //table for store records of
		                                        //type information of files
	};
}
#endif