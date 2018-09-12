///////////////////////////////////////////////////////////////
// Comm.cpp - Communication tools                            //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////

#include "Comm.h"

using namespace Utilities;

/////////////////////////////////////////////////////////////////////////
// Sender class - for sending message

//----------------------< start sending messages >-----------------------

void Sender::start(Socket& sock_, bool& connectionClosed)
{
	threadS_ = std::thread(&Sender::ThreadProc_S, this, std::ref(sock_), std::ref(connectionClosed));
	threadS_.detach();
}
//------------------------< enque message >------------------------------

void Sender::postMessage(const HttpMessage& msg)
{
	sendQ_.enQ(msg);
}
//--------------------< send message to server >-------------------------

void Sender::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString;

	if (msg.findValue("command") != "FILE")
	{
		//std::cout << "\n sending msg string";
		std::string msgString = msg.toString();
		socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
		return;
	}

	//std::cout << "\n  sending msg file";
	std::string path_ = msg.findValue("frompath");
	msg.removeAttribute("frompath");
	msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str()); // send this before sending file to inform the size of file
	sendFile(path_, socket);
}

//------------< send a binary file from socket and save >----------------

bool Sender::sendFile(const std::string& path, Socket& sock_)
{
	//std::cout << "\n  sending file \"" << path << "\"";
	FileSystem::File file(path);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	const size_t BlockSize = 2048; // unit of sending file bytes at once
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		sock_.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

/////////////////////////////////////////////////////////////////////////
// Receiver class - for receiving message

// ------------------------< create directory >--------------------------

bool Receiver::creatDir(std::string dir)
{
	std::string curP_ = path_;
	//std::cout << "\n  current directory: " << curP_ << "\n  target directory: " << dir;

	size_t cur_index;
	for (cur_index = 0; cur_index < curP_.length(); cur_index++)
		if (curP_[cur_index] != dir[cur_index])
		{
			std::cout << "\n\n  directory out of range\n";
			return false;
		}

	// is it the root directory of the corresponsing client in server repository
	if (cur_index == dir.length())
		return true;

	//recursivly create the directories within the path
	cur_index = dir.find("/", cur_index + 1);
	while (cur_index != std::string::npos)
	{
		std::string tem_dir = dir.substr(0, cur_index);
		FileSystem::Directory::create(tem_dir);
		//std::cout << "\n  directory created: " << tem_dir;
		cur_index++;
		cur_index = dir.find("/", cur_index);
	}
	FileSystem::Directory::create(dir); // create the aimed directory
	//std::cout << "\n  directory created: " << dir;

	return true;
}

//---------------------< start receiving messages >----------------------

void Receiver::start(Socket& sock_, bool& connectionClosed, const std::string& path)
{
	path_ = path;
	threadR_ = std::thread(&Receiver::ThreadProc_R, this, std::ref(sock_), std::ref(connectionClosed));
	threadR_.detach();
}
//--------------------------< enque message >--------------------------------

HttpMessage Receiver::getMessage()
{
	return recvQ_.deQ();
}
//----------------------< send message to server >---------------------------

HttpMessage Receiver::recvMessage(Socket& socket, bool& connectionClosed_)
{
	HttpMessage msg;
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);} else break;}
	if (msg.attributes().size() == 0){
		connectionClosed_ = true; return msg;}
	std::string cmd_ = msg.findValue("command");
	if (cmd_ == "FILE"){
		std::string filepath = msg.findValue("todirectory");
		std::string filename = msg.findValue("file");
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "") contentSize = Converter<size_t>::toValue(sizeString);
		else return msg;
		if (filepath == "") filepath = path_ + "/" + filename;
		else{creatDir(filepath);filepath = filepath + "/" + filename;}
		readFile(filepath, contentSize, socket);
		msg.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		sizeString = Converter<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);}
	else{size_t numBytes = 0;size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size()){
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);delete[] buffer;}}
	return msg;
}
//------------< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool Receiver::readFile(const std::string& fqname, size_t fileSize, Socket& socket)
{
	//std::cout << "\n  receiving file \"" << fqname << "\"";
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood()){
		std::cout << "\n\n  can't open file " << fqname;
		return false;}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true){
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;}
	file.close();
	return true;
}

/////////////////////////////////////////////////////////////////////////
// Comm class - Communication tools for Client

//------------< reload operate = to move the Socket object >-------------

Comm& Comm::operator=(Socket&& s)
{
	this->sock_ = std::move(s);
	return *this;
}
//-----------------< start sending and receiving messages >------------------

void Comm::start(const std::string& path)
{
	sndr_.start(sock_, connectionClosed_);
	rcvr_.start(sock_, connectionClosed_, path);
}
//--------------------------< enqueue message >--------------------------------

void Comm::postMessage(const HttpMessage& msg)
{
	sndr_.postMessage(msg);
}
//--------------------------< dequeue message >--------------------------------

HttpMessage Comm::getMessage()
{
	return rcvr_.getMessage();
}
//------------------< factory for creating messages >------------------------
HttpMessage Comm::makeMessage(size_t n, const std::vector<std::string>& body, const std::string& ep_to, const std::string& ep_from)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;

	msg.clear();
	msg.addAttribute(HttpMessage::attribute("toAddr", ep_to));
	msg.addAttribute(HttpMessage::attribute("fromAddr", ep_from));

	switch (n){
	case 1: // send string message
		msg.addAttribute(HttpMessage::attribute("command", "MESSAGE"));msg.addBody(body[0]);
		if (body[0].size() > 0){
			attrib = HttpMessage::attribute("content-length", Utilities::Converter<size_t>::toString(body[0].size()));
			msg.addAttribute(attrib);}break;
	case 2: {// send file
		msg.addAttribute(HttpMessage::attribute("command", "FILE"));
		std::string filename = FileSystem::Path::getName(body[0]);
		FileSystem::FileInfo fi(body[0]);size_t fileSize = fi.size();
		std::string sizeString = Converter<size_t>::toString(fileSize);
		msg.addAttribute(HttpMessage::attribute("file", filename));
		msg.addAttribute(HttpMessage::attribute("frompath", body[0]));
		if (body[1] != "") msg.addAttribute(HttpMessage::attribute("todirectory", body[1]));
		msg.addAttribute(HttpMessage::attribute("content-length", sizeString));break;}
	case 3: {// search all the files in a specific catagory
		msg.addAttribute(HttpMessage::attribute("command", "CATAGORY"));std::string msgBody = "";
		for (auto file : body) msgBody += file + "\n";msg.addBody(msgBody);
		attrib = HttpMessage::attribute("content-length", Utilities::Converter<size_t>::toString(msgBody.size()));msg.addAttribute(attrib);break;}
	case 4: {// delete files
		msg.addAttribute(HttpMessage::attribute("command", "DELETE"));
		msg.addAttribute(HttpMessage::attribute("file", body[0]));break;}
	case 5: {// load htmlfiles
		msg.addAttribute(HttpMessage::attribute("command", "LOADFILES"));std::string msgBody = "";
		for (auto item : body) msgBody += item + "\n";msg.addBody(msgBody);
		msg.addAttribute(HttpMessage::attribute("content-length", Converter<size_t>::toString(msgBody.size())));break;}
	case 6: {// view html files
		msg.addAttribute(HttpMessage::attribute("command", "HTML"));std::string msgBody = "";
		for (auto item : body) msgBody += item + "\n";msg.addBody(msgBody);
		msg.addAttribute(HttpMessage::attribute("content-length", Converter<size_t>::toString(msgBody.size())));break;}
	default:
		msg.clear();msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));}
	return msg;
}

#ifdef TEST_COMM

using Utils = Utilities::StringHelper;

class Test
{
public:
	void operator()(Socket socket)
	{
		Comm commT_(std::move(socket));
		commT_.start(repoPath_);

		HttpMessage msg;
		for (size_t i = 0; i < 2; ++i)
		{
			msg = commT_.getMessage();
			std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": receive msg from client1\n" + msg.toIndentedString();
		}

		for (size_t i = 0; i < 2; ++i)
		{
			std::vector<std::string> body_;
			std::string msgBody =
				"<msg> Message #" + Converter<size_t>::toString(i + 1) +
				" from server </msg>";
			body_.push_back(msgBody);
			msg = commT_.makeMessage(1, body_, "localhost:9070", "localhost:9070");

			commT_.postMessage(msg);
			std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": server sent\n" + msg.toIndentedString();
			::Sleep(100);
		}
	}
private:
	std::string repoPath_ = "../CodePubS/RepoS/";
};

void main()
{
	Utils::Title("Demo functionality of package Comm");

	Test server;
	SocketSystem ss;
	SocketConnecter si;
	SocketListener sl(9070, Socket::IP6);
	sl.start(server);
	while (!si.connect("localhost", 9070))
	{
		std::cout << "\n  client waiting to connect";
		::Sleep(100);
	}
	std::cout << "\n  Connection Built";
	Comm commC_(std::move(si));
	commC_.start("../CodePubC/RepoC/");

	// send a set of messages
	HttpMessage msg;
	for (size_t i = 0; i < 2; ++i)
	{
		std::vector<std::string> body_;
		std::string msgBody =
			"<msg> Message #" + Converter<size_t>::toString(i + 1) +
			" from client1 </msg>";
		body_.push_back(msgBody);
		msg = commC_.makeMessage(1, body_, "localhost:9070", "localhost:9070");

		commC_.postMessage(msg);
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": client1 sent\n" + msg.toIndentedString();
		::Sleep(100);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		msg = commC_.getMessage();
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": receive msg from server\n" + msg.toIndentedString();
}
}
#endif


