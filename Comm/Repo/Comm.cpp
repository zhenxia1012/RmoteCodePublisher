///////////////////////////////////////////////////////////////
// Comm.cpp - Communication tools                            //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////

#include "Comm.h"

using namespace Utilities;

/////////////////////////////////////////////////////////////////////////////
// Comm class - Communication tools for Client

//-----------------< start sending and receiving messages >------------------

void Comm::start()
{
	threadS_ = std::thread(&Comm::ThreadProc_S, this, std::ref(connectionClosed_));
	threadR_ = std::thread(&Comm::ThreadProc_R, this, std::ref(connectionClosed_));
	threadS_.detach();
	threadR_.detach();
}
//--------------------------< enque message >--------------------------------

void Comm::postMessage(const HttpMessage& msg)
{
	sendQ_.enQ(msg);
}
//--------------------------< enque message >--------------------------------

HttpMessage Comm::getMessage()
{
	return recvQ_.deQ();
}
//----------------------< send message to server >---------------------------

void Comm::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----------------------< send message to server >---------------------------

HttpMessage Comm::recvMessage(Socket& socket, bool& connectionClosed_)
{
	HttpMessage msg;

	// read message attributes
	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	// If client is done, connection breaks and recvString returns empty string

	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}

	// is this a file message?
	std::string filename = msg.findValue("file");
	if (filename != "")
	{
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		else
			return msg;

		readFile(filename, contentSize, socket);
	}

	if (filename != "")
	{
		// construct message body
		msg.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		std::string sizeString = Converter<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	else
	{
		// read message body
		size_t numBytes = 0;
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
	}

	return msg;
}
//------------< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool Comm::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../CodePubC/RepoC/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		std::cout << "\n\n  can't open file " << fqname;
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
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
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

//------------------< factory for creating messages >------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage Comm::makeMessage(size_t n, const std::string& body, const std::string& ep_from, const std::string& ep_to)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;

	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep_to));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + ep_from));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Utilities::Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}

/////////////////////////////////////////////////////////////////////////
// CodePubS class - CodePublisher Server

//----< receiver functionality is defined by this function >---------

void CodePubS::operator()(Socket socket)
{
	Comm commS_(std::move(socket));
	commS_.start();

	HttpMessage tem_msg;
	for (size_t i = 0; i < 3; ++i)
	{
		tem_msg = commS_.getMessage();
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": receive msg from client1\n" + tem_msg.toIndentedString();
	}

	for (size_t i = 0; i < 3; ++i)
	{
		std::string msgBody =
			"<msg>Message #" + Converter<size_t>::toString(i + 1) +
			" from server1 </msg>";
		tem_msg = commS_.makeMessage(1, msgBody, "localhost:8080", "localhost:8080");
		commS_.postMessage(tem_msg);
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": server1 sent\n" + tem_msg.toIndentedString();
		::Sleep(100);
	}
	
}

#ifndef Comm_H
#define Comm_H

#include "../Utilities/Utilities.h"

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Demo functionality of package Comm");

	CodePubS server;
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
	commC_.start();

	// send a set of messages
	HttpMessage msg;
	for (size_t i = 0; i < 3; ++i)
	{
		std::string msgBody =
			"<msg>Message #" + Converter<size_t>::toString(i + 1) +
			" from client1 </msg>";
		msg = commC_.makeMessage(1, msgBody, "localhost:8080", "localhost:8080");
		/*
		* Sender class will need to accept messages from an input queue
		* and examine the toAddr attribute to see if a new connection
		* is needed.  If so, it would either close the existing connection
		* or save it in a map[url] = socket, then open a new connection.
		*/
		commC_.postMessage(msg);
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": client1 sent\n" + msg.toIndentedString();
		::Sleep(100);
	}

	for (size_t i = 0; i < 3; ++i)
	{
		msg = commC_.getMessage();
		std::cout << "\n\n Thread#" << std::this_thread::get_id() << ": receive msg from client1\n" + msg.toIndentedString();
	}


}
#endif


