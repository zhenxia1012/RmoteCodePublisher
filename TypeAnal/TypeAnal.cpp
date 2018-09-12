///////////////////////////////////////////////////////////////
// TypeAnal.h -  Utility for type analysis based on TypeTalbe//
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
#include "TypeAnal.h"
#include "../Parser/ConfigureParser.h"

using namespace CodeAnalysis;

///////////////////////////////////////////////////////////////
// class: TypeAnal - Utility for analyzing type information 
//                   
//----< constructor >---------------------------------------------------

TypeAnal::TypeAnal(TypeTable& table)
	: ASTref_(Repository::getInstance()->AST()), TPtable(table) {}
//----< deconstructor >-------------------------------------------------

TypeAnal::~TypeAnal() {}
//----< set AST inside TypeAnyal for analysis >-------------------------

void TypeAnal::doTypeAnal()
{
	std::cout << "\n  scanning AST and displaying important things:";
	std::cout << "\n -----------------------------------------------";

	ASTNode* pRoot = ASTref_.root();
	DFS(pRoot); // DFS start from root node
}
//----< walk through the entire AST by DFS>-----------------------------

void TypeAnal::DFS(ASTNode* pNode)
{
	static std::string path = "";
	if (pNode->path_ != path)
	{
		std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		path = pNode->path_;
	}
	doAction(pNode); // store the type information
	for (auto pChild : pNode->children_)//do DFS for every child of pChild
		DFS(pChild);
}
//----< save information for every node >-------------------------------

bool TypeAnal::doAction(ASTNode* node)
{
	// always remember the name of closest namespace
	static std::string lastnmsp_ = "Global Namespace";
	if (node->type_ == "namespace")
		lastnmsp_ = node->name_;
	//don't store type information of class function and main
	if ((node->type_ == "function" && node->parentType_ != "namespace") || node->name_ == "main")
		return false;

	std::vector<TableRecord *>& tem_table = TPtable.getTable();
	if (node->type_ == "namespace" || node->type_ == "class" || node->type_ == "struct" || node->type_ == "function" || node->type_=="enum")
	{
		//only store information for the type above
		TableRecord* tem_record = new TableRecord();
		tem_record->type_ = node->type_;
		tem_record->parentType_ = node->parentType_;
		tem_record->name_ = node->name_;
		tem_record->namespace_ = lastnmsp_;
		tem_record->package_ = node->package_;
		tem_record->path_ = node->path_;
		if (node->type_ == "function")
			tem_record->type_ = "globalfunction"; //set range to global for global function
		tem_table.push_back(tem_record);//store type information in table for one node
	}

	for (auto item : node->decl_)
		if ((item.declType_ == DeclType::dataDecl && node->type_ == "namespace") || item.declType_ == DeclType::usingDecl
			||item.declType_ == DeclType::typedefDecl)
		{
			TableRecord* tem_record = new TableRecord();
			tem_record->type_ = item.type_;
			if (item.declType_ == DeclType::dataDecl && node->type_ == "namespace")
				tem_record->type_ = "global" + tem_record->type_;
			tem_record->parentType_ = node->type_;
			tem_record->name_ = item.name_;
			tem_record->namespace_ = lastnmsp_;
			tem_record->package_ = item.package_;
			tem_record->path_ = item.path_;
			tem_table.push_back(tem_record);
		}
	return true;
}
//----< just for looking at the content >-------------------------------

bool TypeAnal::doDisplay(ASTNode* node)
{
	static std::string lastnmsp_ = "anonymous";
	if (node->type_ == "namespace")
		lastnmsp_ = node->name_;

	TableRecord* tem_record = new TableRecord();
	tem_record->type_ = node->type_;
	tem_record->parentType_ = node->parentType_;
	tem_record->name_ = node->name_;
	tem_record->namespace_ = lastnmsp_;
	tem_record->path_ = node->path_;
	std::vector<TableRecord *>& tem_table = TPtable.getTable();
	tem_table.push_back(tem_record);
	return true;
}
#ifndef TypeAnal_H
#define TypeAnal_H

#include "../FileSystem/FileSystem.h"

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Testing TypeAnal and TypeTable");
	//get all the ".h" and ".cpp" files from the current directory
	std::vector<std::string> currfiles = FileSystem::Directory::getFiles("../FileSystem", "*.h");
	std::vector<std::string> currfiles1 = FileSystem::Directory::getFiles("../FileSystem", "*.cpp");
	for (size_t i = 0; i < currfiles1.size(); ++i)
		currfiles.push_back(currfiles1[i]);
	for (size_t i = 0; i < currfiles.size(); ++i){
		FileSystem::Directory::setCurrentDirectory("../FileSystem");
		currfiles[i] = FileSystem::Path::getFullFileSpec(currfiles[i]);
		std::cout << "\n  Retreving file: " << currfiles[i].c_str();}
	Utilities::putline();
	Utils::title("Starting Type Analysis the files");
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build(); //build rules and actions
	for (size_t i = 0; i < currfiles.size(); ++i)
	{
		try
		{
			if (pParser)
			{
				if (!configure.Attach(currfiles[i]))
				{
					std::cout << "\n  could not open file " << currfiles[i] << std::endl;
				}
			}
			else{
				std::cout << "\n\n  Parser not built\n\n";
				return;}
			// now that parser is built, use it
			Repository* pRepo = Repository::getInstance();
			pRepo->currentPath() = currfiles[i];
			pRepo->package() = FileSystem::Path::getName(currfiles[i]);
			while (pParser->next())
				pParser->parse();}
		catch (std::exception& ex){
			std::cout << "\n\n    " << ex.what() << "\n\n";}
	}
	TypeTable tb("Project2 File");
	TypeAnal tpa(tb);
	tpa.doTypeAnal();
	std::cout << tb.show();
}
#endif