///////////////////////////////////////////////////////////////
// Typetable.cpp - structure for store the type information  //
//                 from file collection and it is the base   //
//                 for dependency analysis                   //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
#include <iomanip>
#include "TypeTable.h"

using namespace CodeAnalysis;

///////////////////////////////////////////////////////////////
// class: TableRecord - record analysis of a file,e.g, class, 
//                      function, enum, etc.
//
//----< default initialization for TableRecord >------------------------

TableRecord::TableRecord()
	: type_("anonymous"), parentType_("namespace"), name_("none"), namespace_("Global Namespace") {}

//----< initialization accepting type and name >------------------------

TableRecord::TableRecord(const Property& type, const Property& name)
	: type_(type), parentType_("namespace"), name_(name), namespace_("Global Namespace") {}

TableRecord::~TableRecord()
{
}
//----< returns string with TableRecord description >--------------------

std::string TableRecord::show(bool details)
{
	std::ostringstream temp;
	temp << "\n  ";
	temp.setf(_IOSleft);
	temp << std::setw(16) << type_;
	temp.setf(_IOSleft);
	temp << std::setw(18) << parentType_;
	temp.setf(_IOSleft);
	temp << std::setw(30) << name_;
	temp.setf(_IOSleft);
	temp << std::setw(16) << package_;
	temp.setf(_IOSleft);
	temp << std::setw(20) << namespace_;
	temp.setf(_IOSleft);
	temp << path_;
	return temp.str();
}
///////////////////////////////////////////////////////////////
// class: TypeTable - record files information with all file
//                    analysis
//
//----< constructor >----------------------------------------------------

TypeTable::TypeTable(const Property& name)
	: tablename(name) {}
//----< deconstructor >--------------------------------------------------

TypeTable::~TypeTable()
{
	for (auto item : tablerecords)
		delete item;
}
//----< get type table of records >--------------------------------------

std::vector<TableRecord *>& TypeTable::getTable()
{
	return tablerecords;
}
//----< add table record >-----------------------------------------------

void TypeTable::addRecord(TableRecord* tablerecord)
{
	tablerecords.push_back(tablerecord);
}
//----< show content of the table >--------------------------------------

std::string TypeTable::show()
{
	std::ostringstream temp;
	temp << "\n\n  Table Name: " << tablename << "\n  ";
	temp.setf(_IOSleft);
	temp << std::setw(16) << "Type";
	temp.setf(_IOSleft);
	temp << std::setw(18) << "Parent Type";
	temp.setf(_IOSleft);
	temp << std::setw(30) << "Name";
	temp.setf(_IOSleft);
	temp << std::setw(16) << "Package";
	temp.setf(_IOSleft);
	temp << std::setw(20) << "Name Space";
	temp.setf(_IOSleft);
	temp << std::setw(8) << "Path";
	temp << "\n  ";
	temp.fill('-');
	temp << std::setw(150) << "";
	for (auto item : tablerecords)
		temp << item->show();
	//TreeWalk(tablerecords[0]);
	return temp.str();
}
#ifndef TypeTable_H
#define TypeTable_H

#include "../Utilities/Utilities.h"

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Testing TypeTable, FileTable, TableRecord");

	TableRecord* pN = new TableRecord("namespace", "Global Namespace");
	pN->namespace_ = "Global Namespace";
	pN->package_ = "file1";
	pN->path_ = "./file1";
	TableRecord* pX = new TableRecord("class", "X");
	pX->namespace_ = "Global Namespace";
	pX->package_ = "file1";
	pX->path_ = "./file1";
	pX->parentType_ = pN->type_;
	TableRecord* pf1 = new TableRecord("function", "f1");
	pf1->namespace_ = "Global Namespace";
	pf1->package_ = "file1";
	pf1->path_ = "./file1";
	pf1->parentType_ = pX->type_;
	TableRecord* pc1 = new TableRecord("control", "if");
	pc1->namespace_ = "Global Namespace";
	pc1->package_ = "file1";
	pc1->path_ = "./file1";
	pc1->parentType_ = pf1->type_;
	TableRecord* pf2 = new TableRecord("function", "f2");
	pf2->namespace_ = "Global Namespace";
	pf2->package_ = "file1";
	pf2->path_ = "./file1";
	pf2->parentType_ = pX->type_;

	TypeTable* tb = new TypeTable("C++ Table");
	tb->addRecord(pN);
	tb->addRecord(pX);
	tb->addRecord(pf1);
	tb->addRecord(pc1);
	tb->addRecord(pf2);
	std::cout << "\n  " << tb->show();

}
#endif