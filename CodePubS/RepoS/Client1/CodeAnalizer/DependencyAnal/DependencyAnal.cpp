///////////////////////////////////////////////////////////////
// DependencyAnal.cpp - Utility for dependency analysis based//
//                     type table                            //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
#include <fstream>
#include <exception>
#include "DependencyAnal.h"

using namespace CodeAnalysis;

///////////////////////////////////////////////////////////////
// class: DependencyAnal - Do the Dependency Analysis based on
//                         TypeTable
//
//----< constructor >---------------------------------------------------

DependencyAnal::DependencyAnal(TypeTable& table, NoSqlDb<std::string>& dtbs_)
	: TPtable(table), db_(dtbs_), ASTref_(Repository::getInstance()->AST()) {}
//----< deconstructor >-------------------------------------------------

DependencyAnal::~DependencyAnal() {}
//----< extract all the paths from type table >-------------------------

void DependencyAnal::etractFiles()
{
	ASTNode* pRoot = ASTref_.root();
	DFS(pRoot); // DFS start from root node
}
//----< walk through the entire AST by DFS >----------------------------
void DependencyAnal::DFS(ASTNode* pNode)
{
	if (pNode->path_ != "")
	{
		//std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		//ensure whether the file has been included
		std::vector<std::string>::iterator iter = std::find(begin(files), end(files), pNode->path_);
		if (iter == end(files))
			files.push_back(pNode->path_);
		return;
	}
	for (auto pChild : pNode->children_)//do DFS for every child of pChild
		DFS(pChild);
}
//----< initialize database for every file >----------------------------

void DependencyAnal::iniDb()
{
	// for every file, initialize a element in db
	for (auto item : files)
	{
		std::string filename = FileSystem::Path::getName(item);
		Element<std::string> tem_file;
		tem_file.key = filename;
		tem_file.name = filename;
		tem_file.category = "Test";
		tem_file.description = item;
		tem_file.timeDate = "none";
		tem_file.data = filename + " data";
		db_.save(tem_file.name, tem_file);
	}
}
//----< do dependency analysis >----------------------------------------

void DependencyAnal::doDepenAnal()
{
	using namespace Scanner;
	std::vector<TableRecord*> temrecords = TPtable.getTable();
	//for every record in talbe, find type name at first
	for (auto item : temrecords)
	{
		//std::cout << "\n   Path: " << item->path_;
		//don't find dependency for namespace
		if (item->type_ == "namespace")
			continue;
		std::string typename_ = item->name_; // get the type name
		//std::cout << "\n  Need to find: " << typename_;
		for (auto file : files)
		{
			std::string srcfile = FileSystem::Path::getName(item->path_);
			std::string dstfile = FileSystem::Path::getName(file);
			//std::cout << "\n  Test dependency: " << dstfile << " with " << srcfile;
			if (srcfile == dstfile) // don't find dependency for the same file
				continue;
			// don't find dependency for two files having been recorded for their dependency
			if (doFind(srcfile, dstfile))
				continue;
			std::ifstream in(file);
			if (!in.good())
			{
				std::cout << "\n  can't open \"" << file << "\"\n\n";
				return;
			}
			{
				Toker toker;
				toker.returnComments(false);
				toker.attach(&in);
				do
				{
					std::string tok = toker.getTok();
					//std::cout << "\n   Type: " << typename_ << " Tok: " << tok;
					// if having find type name, then to find namespace
					if (typename_ != tok || !isDep(item->namespace_, file))
						continue;
					else
					{
						//std::cout << "\n  Dependency: " << dstfile << " on " << srcfile;
						doSave(srcfile, dstfile);
						break;
					}
				} while (in.good());}}}
}
//----< decide whether child file have depended on parent file >--------

bool DependencyAnal::doFind(std::string parent, std::string child)
{
	std::vector<std::string> setOfChild = db_.value(parent).children;
	//std::cout << "\n  Size of children: " << setOfChild.size();
	for (auto item : setOfChild)
	{
		if (child == item)
		{
			//std::cout << "\n  file \"" << child << "\" has been a child of file \"" << parent << "\"";
			return true;
		}
		//std::cout << "\n  two childs not: " << child << " and " << item;
	}
	return false;
}
//----< find token in child Type Table >--------------------------------

bool DependencyAnal::isDep(std::string token, std::string path)
{
	using namespace Scanner;

	std::ifstream in(path);
	if (!in.good())
	{
		std::cout << "\n  can't open \"" << path << "\"\n\n";
		return false;
	}
	{
		Toker toker;
		toker.returnComments(false);
		toker.attach(&in);
		do
		{
			std::string tok = toker.getTok();
			//std::cout << "\n   Type: " << typename_ << " Tok: " << tok;
			if (token != tok)
				continue;
			else
				return true;
		} while (in.good());
	}
	return false;
}

/*bool DependencyAnal::isDep(std::string token, std::string path)
{
	std::vector<TableRecord*> temrecords = TPtable.getTable();
	for (auto item : temrecords)
		if (item->path_ == path)
			if (item->name_ != token)
				return true;
	//std::cout << "\n  No dependency!";
	return false;
}*/
//----< save dependency into database >---------------------------------

bool DependencyAnal::doSave(std::string parent, std::string child)
{
	if (!db_.edit(parent, "children", child, true))
	{
		//std::cout << "\n  Couldn't save child file \"" << child << "\" to parent file \"" << parent << "\".";
		return false;
	}
	//std::cout << "\n  Saving child file \"" << child << "\" to parent file \"" << parent << "\".";
	return true;
}

#ifdef TEST_DEPENDENCYANAL

#include "../Utilities/Utilities.h"
#include "../TypeAnal/TypeAnal.h"


using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Testing DependencyAnal");
	Utils::title("Start fetching files");
	std::vector<std::string> currfiles = FileSystem::Directory::getFiles("../Analyzer", "*.h");
	std::vector<std::string> currfiles1 = FileSystem::Directory::getFiles("../Analyzer", "*.cpp");
	for (size_t i = 0; i < currfiles1.size(); ++i)
		currfiles.push_back(currfiles1[i]);
	for (size_t i = 0; i < currfiles.size(); ++i){
		FileSystem::Directory::setCurrentDirectory("../Analyzer");
		currfiles[i] = FileSystem::Path::getFullFileSpec(currfiles[i]);
		std::cout << "\n  Fetching file: " << currfiles[i].c_str();
	}Utilities::putline();
	Utils::title("Start parsing all the files");
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	for (size_t i = 0; i < currfiles.size(); ++i){
		std::cout << "\n  Processing file: " << currfiles[i];
		try{
			if (pParser){
				if (!configure.Attach(currfiles[i])){
					std::cout << "\n  could not open file " << currfiles[i] << std::endl;}}
			else{
				std::cout << "\n\n  Parser not built\n\n";
				return;}
			Repository* pRepo = Repository::getInstance();
			pRepo->currentPath() = currfiles[i];
			while (pParser->next())
				pParser->parse();}
		catch (std::exception& ex)
		{std::cout << "\n\n    " << ex.what() << "\n\n";}
	}
	Utils::Title("Start Type Analysis");
	TypeTable tb("Project2 Table");
	NoSqlDb<std::string> db("Project2 Database");
	TypeAnal tpa(tb);
	tpa.doTypeAnal();
	std::cout << tb.show();
	Utils::title("Starting Dependency Analysis");
	DependencyAnal dpa(tb, db);
	dpa.etractFiles();
	dpa.iniDb();
	dpa.doDepenAnal();
	db.showData();
	Utils::title("Show the XML of database");
	std::string data_xml = Persist::toXml(db);
	std::cout << "\n" << data_xml;
}
#endif