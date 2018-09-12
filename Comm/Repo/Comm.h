#ifndef COMM_H
#define COMM_H
///////////////////////////////////////////////////////////////
// Comm.h - Communication tools                              //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This package contains only one class-CodePub used for publishing
Html for source code files.

Maintanence Information:
========================
Required files:
---------------
CodePublisher.h, CodePublisher.cpp
NoSqlDb.h, FileSystem.h
*/

#include <string>
#include <vector>
#include <sstream>
#include <fstream> 
#include <thread>
#include <iostream>
#include "../Sockets/Sockets.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include "../FileSystem/FileSystem.h"

using namespace Async;

/////////////////////////////////////////////////////////////////////////
// Comm class - Communication tools for Client

class Comm
{
public:
	Comm(Socket&& sock) :sock_(std::move(sock)) {};
	void start();
	void postMessage(const HttpMessage& msg);
	HttpMessage getMessage();
	void sendMessage(HttpMessage& msg, Socket& socket);
	HttpMessage recvMessage(Socket& socket, bool& connectionClosed_);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	HttpMessage makeMessage(size_t n, const std::string& body, const std::string& ep_from, const std::string& ep_to);
private:
	bool connectionClosed_ = false;
	Socket sock_;
	std::thread threadS_;
	std::thread threadR_;
	BlockingQueue<HttpMessage> sendQ_;
	BlockingQueue<HttpMessage> recvQ_;

	void ThreadProc_S(bool& connectionClosed_)
	{
		while (true)
		{
			HttpMessage msg_ = sendQ_.deQ();
			sendMessage(msg_, sock_);
			if (msg_.bodyString() == "quit" || connectionClosed_)
				break;
			::Sleep(100);
		}
		std::cout << "\n  Sender Quit";
	}

	void ThreadProc_R(bool& connectionClosed_)
	{
		while (true)
		{
			HttpMessage msg_ = recvMessage(sock_, connectionClosed_);
			if (msg_.bodyString() == "quit" || connectionClosed_)
				break;
			recvQ_.enQ(msg_);
		}
		std::cout << "\n  Receiver Quit";
	}
};

/////////////////////////////////////////////////////////////////////////
// CodePubS class - CodePublisher Server

class CodePubS
{
public:
	void operator()(Socket socket);
private:
};
#endif
