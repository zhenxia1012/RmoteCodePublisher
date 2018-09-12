///////////////////////////////////////////////////////////////
// CodePubC.cpp - Communication tools                        //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////

#include "../Comm/Comm.h"
#include "CodePubC.h"
#include <thread>
#include <windows.h>
#include <shellapi.h>

using namespace Utilities;

/////////////////////////////////////////////////////////////////////
// CodePubC class - CodePublisher Server

class CodePubC : public IClient
{
public:
	Comm commC_;

	CodePubC(const std::string& name, const std::string& ip = "localhost", const size_t& port = 9070)
		:client(name), repoPath_("../CodePubC/RepoC/" + name), ip_(ip), port_(port) {}
	std::string getName();
	void iniConn(const std::string& ip = "localhost", const size_t& port = 9070);
	HttpMessage getMessage();
	void sendFiles(const std::vector<std::string>& pfrom, const std::string& pto);
	void delFiles(const std::string& file);
	void askCata(const std::string& dir);
	std::vector<std::string> getCata(const std::string& cata);
	void viewHtml();
	void loadHtml(const std::vector<std::string>& files);
	void openHtml(const std::string& name);
private:
	SocketSystem ss_;
	SocketConnecter si_;
	std::string ip_;
	size_t port_;
	std::string client;
	std::string repoPath_; // default path of receive files
};

//------------------------< get client name >------------------------

std::string CodePubC::getName()
{
	return client;
}
//-------------------< initialize the connection >-------------------

void CodePubC::iniConn(const std::string& ip, const size_t& port)
{
	std::cout << "\n\n  Initialize connection resources";

	while (!si_.connect(ip, port))
	{
		std::cout << "\n  client waiting to connect ip: " << ip << " port: " << port;
		::Sleep(100);
	}
	std::cout << "\n  " << client << " Connection Built";

	// send client name and address to server
	si_.sendString(client);
	si_.sendString(ip_ + ":" + Converter<size_t>::toString(port_));
	FileSystem::Directory::create(repoPath_);

	commC_ = std::move(si_);
	commC_.start(repoPath_);
}
//--------------------------< dequeue message >--------------------------------

HttpMessage CodePubC::getMessage()
{
	return commC_.getMessage();
}
//-------------------------< send files >----------------------------

void CodePubC::sendFiles(const std::vector<std::string>& pfrom, const std::string& pto)
{
	std::cout << "\n\n  Send Files";

	std::string fromAddr = ip_ + ":" + Converter<size_t>::toString(port_);
	for (auto file : pfrom)
	{
		std::vector<std::string> body_ = { file, pto };
		HttpMessage msg = commC_.makeMessage(2, body_, fromAddr, "localhost:9070");
		//std::cout << "\n  file msg: \n  " << msg.toIndentedString();
		commC_.postMessage(msg);
	}
}
//------------------------< delete files >---------------------------

void CodePubC::delFiles(const std::string& file)
{
	std::cout << "\n\n  Ask serfver to delete files";

	std::string fromAddr = ip_ + ":" + Converter<size_t>::toString(port_);
	std::vector<std::string> body = { file };
	HttpMessage msg = commC_.makeMessage(4, body, fromAddr, "localhost:9070");
	std::cout << "\n  file msg: \n  " << msg.toIndentedString();
	commC_.postMessage(msg);
}
//-------------< get catatory of a specific directory >--------------

void CodePubC::askCata(const std::string& dir)
{
	std::cout << "\n\n  Ask server to return files under \"" << dir << "\"";

	std::string fromAddr = ip_ + ":" + Converter<size_t>::toString(port_);
	std::vector<std::string> dir_ = { dir };
	HttpMessage msg_ = commC_.makeMessage(3, dir_, fromAddr, "localhost:9070");
	std::cout << "\n  sending message:\n" << msg_.toIndentedString();
	commC_.postMessage(msg_);
}
//-------------< translate body of message into paths >---------------

std::vector<std::string> CodePubC::getCata(const std::string& cata)
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
//------------< ask server to return the html file name >------------

void CodePubC::viewHtml()
{
	std::cout << "\n\n  Ask Server to return the html file name";

	std::vector<std::string> body;
	std::string fromAddr = ip_ + ":" + Converter<size_t>::toString(port_);
	HttpMessage msg_ = commC_.makeMessage(6, body, fromAddr, "localhost:9070");
	commC_.postMessage(msg_);
}
//-----------------------< load html file >--------------------------

void CodePubC::loadHtml(const std::vector<std::string>& files)
{
	std::cout << "\n\n  Load HTML files";

	std::string fromAddr = ip_ + ":" + Converter<size_t>::toString(port_);
	HttpMessage msg_ = commC_.makeMessage(5, files, fromAddr, "localhost:9070");
	commC_.postMessage(msg_);
}
//----------------------< open html on web >-------------------------

void CodePubC::openHtml(const std::string& name)
{
	std::cout << "\n\n  open HTML file: " << name;

	std::string path = "..\\CodePubC\\RepoC\\Client1\\" + name;
	size_t size = path.length();
	wchar_t *buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;
	ShellExecute(NULL, NULL, buffer, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////
// ObjectFactory class - create client object

//--------------------< create client object >-----------------------

IClient* ObjectFactory::createCli(const std::string& name)
{
	return new CodePubC(name);
}

//----------------------------< main >-------------------------------

#ifdef TEST_CODEPUBC

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Demo functionality of package CodePubC");

	CodePubC client("Client1");
	client.iniConn();

	std::vector<std::string> files_;
	/*files_.push_back("../TypeTable/TypeTable.h");
	files_.push_back("../TypeTable/TypeTable.cpp");
	client.sendFiles(files_, "../CodePubS/RepoS/Client1/CodeAnalizer/TypeTable");
	files_.clear();*/
	/*files_.push_back("../TypeAnal/TypeAnal.h");
	files_.push_back("../TypeAnal/TypeAnal.cpp");
	client.sendFiles(files_, "../CodePubS/RepoS/Client1/CodeAnalizer/TypeAnal");*/
	/*files_.clear();
	files_.push_back("../DependencyAnal/DependencyAnal.h");
	files_.push_back("../DependencyAnal/DependencyAnal.cpp");
	client.sendFiles(files_, "../CodePubS/RepoS/Client1/CodeAnalizer/DependencyAnal");
	Sleep(30000);*/

	/*std::string dir_ = FileSystem::Path::getFullFileSpec("../CodePubS/RepoS");
	std::cout << "\n  dir_: " << dir_;
	client.askCata(dir_);*/

	files_ = { "../CodePubS/RepoS/Client1/CodeAnalizer/TypeAnal/TypeAnal.cpp", "../CodePubS/RepoS/Client1/CodeAnalizer/TypeAnal/TypeAnal.h" };
	client.delFiles(files_);

	//files_ = { "Cpp11-DependencyAnal.cpp.htm", "Cpp11-DependencyAnal.h.htm" };
	//client.loadHtml(files_);

	while (true) {}
}
#endif
