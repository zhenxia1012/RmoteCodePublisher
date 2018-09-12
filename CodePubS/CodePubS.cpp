///////////////////////////////////////////////////////////////
// CodePubS.h - Code Publisher Server                        //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////

#include "CodePubS.h"

using namespace CodeAnalysis;
using namespace CodePublisher;

/////////////////////////////////////////////////////////////////////
// CodePubS class - CodePublisher Server

//-----------------< get client name and address >-------------------

void CodePubS::iniCliInfo(Socket& socket)
{
	std::cout << "\n\n  Initialize client information";
	client = socket.recvString();
	repoPath_ = repoPath_ + "/" + client;
	FileSystem::Directory::create(repoPath_);
	FileSystem::Directory::create(repoPath_ + "/HTML");
	std::cout << "\n  Client name: " << client;
	cli_addr = socket.recvString();
	std::cout << "\n  Client address: " << cli_addr;
}
//---------------------< set client recources >----------------------

void CodePubS::iniCliResource(Comm& comm)
{
	std::cout << "\n\n  Initialize client information";

	// send ./js and .css file to client
	std::vector<std::string> body = { "../CodePubS/RepoS/Publisher.css", "" };
	HttpMessage msg_ = comm.makeMessage(2, body, "localhost:9070", cli_addr);
	comm.postMessage(msg_);
	body = { "../CodePubS/RepoS/script.js", "" };
	msg_ = comm.makeMessage(2, body, "localhost:9070", cli_addr);
	comm.postMessage(msg_);

	// copy ./js and .css file to client folder in server
	//std::cout << "\n  .js and .css file directory: " << repoPath_;
	copyFile("../CodePubS/RepoS/Publisher.css", repoPath_ + "/HTML/Publisher.css");
	copyFile("../CodePubS/RepoS/script.js", repoPath_ + "/HTML/script.js");
}
//--------------< copy file from frompath to topath >----------------

bool CodePubS::copyFile(const std::string& frompath, const std::string& topath)
{
	FileSystem::File me(frompath);
	me.open(FileSystem::File::in, FileSystem::File::binary);
	std::cout << "\n  copying " << me.name().c_str() << " to \"" + topath + "\"";
	if (!me.isGood()){
		std::cout << "\n\n  can't open executable \"" + frompath + "\" \n";
		return false;
	}
	else{
		FileSystem::File you(topath);
		you.open(FileSystem::File::out, FileSystem::File::binary);
		if (!you.isGood()){
			std::cout << "\n\n  can't open executable \"" + topath + "\" \n";
			return false;
		}
		else
		{
			while (me.isGood()){
				static size_t count = 0;
				FileSystem::Block b = me.getBlock(1024);
				you.putBlock(b);
				if (++count < 10){
					//std::cout << "\n    reading block of " << b.size() << " bytes";
					//std::cout << "\n    writing block of " << b.size() << " bytes";
				}
				if (b.size() < 1024){
					//std::cout << "\n\n    omitted " << count - 10 << " blocks from display\n\n";
					//std::cout << "\n    reading block of " << b.size() << " bytes";
					//std::cout << "\n    writing block of " << b.size() << " bytes";
				}
			}
			std::cout << "\n";
		}
		you.close();
	}
	me.close();
	return true;
}
//---------------< get file path from message body >-----------------

std::vector<std::string> CodePubS::getCata(const std::string& cata)
{
	std::cout << "\n\n  Get file path from message body";

	std::vector<std::string> files_;
	size_t last_index = 0;
	size_t curr_index = cata.find("\n");

	while (curr_index != std::string::npos)
	{
		std::string file = cata.substr(last_index, curr_index - last_index);
		files_.push_back(file);
		last_index = curr_index + 1;
		curr_index = cata.find("\n", last_index);
	}

	return files_;
}
//-------------------------< delete files >--------------------------

bool CodePubS::delFile(const std::string& file)
{
	std::cout << "\n\n  Deleting File: " << file;

	try
	{
		if (!FileSystem::File::remove(file))
			throw std::exception("");
	}
	catch (std::exception ex)
	{
		std::cout << "\n\n  Couldn't delete file \"" << file << "\"";
		return false;
	}
	std::cout << "\n  file deleted: \"" << file << "\"";

	return true;
}
//------------< find all files under specific directory >------------

std::vector<std::string> CodePubS::findfiles(const std::string& dir, bool onlyName)
{
	std::cout << "\n\n  Find all files under \"" << dir << "\"";

	std::vector<std::string> files;
	std::string curDir = FileSystem::Directory::getCurrentDirectory();

	std::string fpath = FileSystem::Path::getFullFileSpec(dir);
	FileSystem::Directory::setCurrentDirectory(dir);

	std::vector<std::string> tem_files = FileSystem::Directory::getFiles(fpath, "*.*");
	for (auto tem_file : tem_files)
	{
		//std::cout << "\n  fffff: " << tem_file;
		if (onlyName)
			files.push_back(tem_file);
		else
			files.push_back(FileSystem::Path::getFullFileSpec(tem_file));
	}

	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		//std::cout << "\n  dddddd: " << d;
		if (onlyName)
			files.push_back(d);
		else
			files.push_back(FileSystem::Path::getFullFileSpec(d));
	}

	FileSystem::Directory::setCurrentDirectory(curDir);
	return files;
}

//--< find files with required patterns under specific directory >---

std::vector<std::string> CodePubS::findfiles(const std::string& dir, const std::vector<std::string> patterns, bool onlyName)
{
	std::cout << "\n\n  Find all files(include subdirectory) under \"" << dir << "\"";

	std::vector<std::string> files;
	std::string curDir = FileSystem::Directory::getCurrentDirectory();

	std::string fpath = FileSystem::Path::getFullFileSpec(dir);
	FileSystem::Directory::setCurrentDirectory(dir);
	for (auto patt : patterns)
	{
		//std::cout << "\n  find pattern: " << patt;
		std::vector<std::string> tem_files = FileSystem::Directory::getFiles(fpath, patt);
		for (auto tem_file : tem_files)
		{
			//std::cout << "\n  find file: " << tem_file;
			if (onlyName)
				files.push_back(tem_file);
			else
				files.push_back(FileSystem::Path::getFullFileSpec(tem_file));
		}
	}

	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;

		std::vector<std::string> tem_files = findfiles(dpath, patterns);
		for (auto tem_file : tem_files)
		{
			if (onlyName)
				files.push_back(tem_file);
			else
				files.push_back(FileSystem::Path::getFullFileSpec(tem_file));
		}
	}

	FileSystem::Directory::setCurrentDirectory(curDir);
	return files;
}
//----------< dependency analysis for given directory >--------------

void CodePubS::depAnal(const std::string& dir)
{
	std::cout << "\n\n  Dependency Analysis under \"" << dir << "\"";

	std::vector<std::string> patterns = { "*.h", "*.cpp" };
	std::vector<std::string> files = findfiles(dir, patterns);
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	for (size_t i = 0; i < files.size(); ++i) {
		try {
			if (pParser) {
				if (!configure.Attach(files[i])) {
					std::cout << "\n  could not open file\n" << files[i] << std::endl;
				}
			}
			else {
				std::cout << "\n\n  Parser not built\n";
				return;
			}
			Repository* pRepo = Repository::getInstance();
			pRepo->currentPath() = files[i];
			while (pParser->next())
				pParser->parse();
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n";
		}
	}
	//std::cout << "\n\n  Start Type Analysis";
	TypeTable tb("Type Table");
	NoSqlDb<std::string> db("NoSqlDb Database");
	TypeAnal tpa(tb);
	tpa.doTypeAnal();
	//std::cout << tb.show();
	//std::cout << "\n\n  Starting Dependency Analysis";
	DependencyAnal dpa(tb, db);
	dpa.etractFiles();
	dpa.iniDb();
	dpa.doDepenAnal();
	db.showData();
	//std::cout << "\n\n  Show the XML of database";
	std::string dep_xml = Persist::toXml(db);
	//std::cout << dep_xml;
	db.stringToTextFile(dep_xml, dir + "/dependency.xml");
}
//----------< dependency analysis for given directory >--------------

void CodePubS::htmlGen(const std::string& path)
{
	std::cout << "\n\n  Generate HTML Files for \"" << path << "\"";
	NoSqlDb<std::string> db;
	std::string dep_xml = db.textFileToString(path);
	db = Persist::fromXml<std::string>(dep_xml);
	//std::cout << "\n\n  Show the XML of database\n" << dep_xml;
	//db.showData();

	CodePub cdpb_(db);
	cdpb_.iniHtmlMd("../CodePubS/RepoS/htmlModel.htm", "../CodePubS/RepoS/CataModel.htm");
	//std::cout << "\n  save directory: " << repoPath_ + "/HTML";
	cdpb_.mkAllHtml(repoPath_ + "/HTML");
}
//-------------------------< send files >----------------------------

void CodePubS::sendFiles(Comm& comm, const std::vector<std::string>& files)
{
	std::cout << "\n\n  Send Files";

	for (auto file : files)
	{
		std::vector<std::string> body_ = { file, "" };
		HttpMessage msg = comm.makeMessage(2, body_, "localhost:9070", cli_addr);
		std::cout << "\n  file msg: \n  " << msg.toIndentedString();
		comm.postMessage(msg);
	}
}
//----< receiver functionality is defined by this function >---------

void CodePubS::operator()(Socket socket)
{
	iniCliInfo(socket);
	Comm commS_ = std::move(socket);
	commS_.start(repoPath_);
	iniCliResource(commS_);
	HttpMessage msg_;
	std::vector<std::string> body;
	while (true){
		msg_ = commS_.getMessage();
		std::cout << "\n  Server Thread#" << std::this_thread::get_id() << ": receive msg from client1\n" + msg_.toIndentedString();
		std::string cmd_ = msg_.findValue("command");
		if (cmd_ == "FILE"){
			depAnal(repoPath_);
			htmlGen(repoPath_ + "/dependency.xml");}
		else if (cmd_ == "CATAGORY"){
			std::string dir_ = msg_.bodyString();
			dir_ = dir_.substr(0, dir_.length() - 1);
			std::vector<std::string> files = findfiles(dir_, true);
			msg_ = commS_.makeMessage(3, files, "localhost:9070", cli_addr);std::cout << "\n  sending message:\n" << msg_.toIndentedString();
			commS_.postMessage(msg_);}
		else if (cmd_ == "DELETE"){
			std::string file = msg_.findValue("file");
			/*if (!delFile(file))
				body = { "<msg>files deleting failed<msg>" };
			else
				body = { "<msg>files deleting succeeeded<msg>" };
			msg_ = commS_.makeMessage(1, body, "localhost:9070", cli_addr);
			commS_.postMessage(msg_);*/
			std::string ext_ = FileSystem::Path::getExt(file);
			if (ext_ == ".cpp" || ext_ == ".h"){
				depAnal(repoPath_);
				htmlGen(repoPath_ + "/dependency.xml");}}
		else if (cmd_ == "LOADFILES"){
			std::string cata_ = msg_.bodyString();
			std::vector<std::string> files = getCata(cata_);
			for (size_t i = 0; i < files.size(); i++)
				files[i] = repoPath_ + "/HTML/" + files[i];
			sendFiles(commS_, files);}
		else if (cmd_ == "HTML"){
			std::vector<std::string> patt_ = { "*.htm" };
			std::vector<std::string> files = findfiles(repoPath_ + "/HTML", patt_, true);
			msg_ = commS_.makeMessage(6, files, "localhost:9070", cli_addr);std::cout << "\n  sending message:\n" << msg_.toIndentedString();
			commS_.postMessage(msg_);}::Sleep(100);}
}

#ifndef TEST_CODEPUBS

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Demo functionality of package CodePubS");

	SocketSystem ss;
	SocketListener sl(9070, Socket::IP6);
	CodePubS server;
	sl.start(server);

	while (true) {}
}
#endif