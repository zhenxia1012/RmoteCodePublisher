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
This package contains communication tools for any two conmunicating
endpoint and three classes: Sender, Receiver and Comm.

Sender is mainly for sending message, it has a threadproc that will 
check the sendQ_(BlockingQueue<HttpMessage>) and dequeue the mesage
and send it to target, once the queue is not empty and. Sender 
has interface to let user inputs its message into the queue.
And the threadproc will be bind with a thread and start by the function
start().

Receiver is mainly for receiving message, it has a threadproc that will
always receiving message and enqueue the mesage in receQ_. receiver
has interface to let user dequeue the message. And the threadproc will 
be bind with a thread and start by the function start().

Comm Class wraps Sender and Receiver and control the communication.
makemessage() is for user to difine the type and content of the
the message they want to send. The argument "body" define the attributes
and body content of the message

Maintanence Information:
========================
Required files:
---------------
Comm.h, Comm.cpp
Cpp11-BlockingQueue.h, FileSystem.h
HttpMessage.h, Sockets.h
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
// Sender class - tools for sending message

class Sender
{
public:
	void start(Socket& sock_, bool& connectionClosed); // bind start the threadS_ with ThreadProc_S
	void postMessage(const HttpMessage& msg); // enqueue message into sendQ_
	void sendMessage(HttpMessage& msg, Socket& socket); // call socket to send message by bytes
	bool sendFile(const std::string& path, Socket& socket); // send file bytes
private:
	std::thread threadS_; 
	BlockingQueue<HttpMessage> sendQ_;

	void ThreadProc_S(Socket& sock_, bool& connectionClosed)
	{
		while (true)
		{
			HttpMessage msg_ = sendQ_.deQ();
			sendMessage(msg_, sock_);
			if (msg_.bodyString() == "quit" || connectionClosed)
				break;
			::Sleep(100);
		}
		std::cout << "\n  Sender Quit";
	}
};

/////////////////////////////////////////////////////////////////////////
// Receiver class - tools for receiving message

class Receiver
{
public:
	bool creatDir(std::string dir); // creat the directories and subdirectories with path dir
	void start(Socket& sock_, bool& connectionClosed, const std::string& path); //bind start the threadR_ with ThreadProc_S
	HttpMessage getMessage(); // dequeue message from receQ_
	HttpMessage recvMessage(Socket& socket, bool& connectionClosed_); // use socket to receive string
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket); // use socket to receive file bytes
private:
	std::string path_ = "../Comm/Repo"; // default the path of received file
	std::thread threadR_;
	BlockingQueue<HttpMessage> recvQ_;

	void ThreadProc_R(Socket& sock_, bool& connectionClosed)
	{
		while (true)
		{
			HttpMessage msg_ = recvMessage(sock_, connectionClosed);
			if (msg_.bodyString() == "quit" || connectionClosed)
				break;
			recvQ_.enQ(msg_);
		}
		std::cout << "\n  Receiver Quit";
	}
};

/////////////////////////////////////////////////////////////////////////
// Comm class - Communication tools for Client

class Comm
{
public:
	Comm() {}
	Comm(Socket&& sock) :sock_(std::move(sock)) {} // copy constructor has been deleted so use move
	Comm& operator=(Socket&& s);
	void start(const std::string& path); // call start() of sndr_ and rcvr_
	void postMessage(const HttpMessage& msg); // call postMessage() of sndr
	HttpMessage getMessage(); // call getMessage() of rcvr_
	HttpMessage makeMessage(size_t n, const std::vector<std::string>& argv, const std::string& ep_from, const std::string& ep_to);
private:
	bool connectionClosed_ = false; // control the communication
	Socket sock_;
	Sender sndr_;
	Receiver rcvr_;
};

#endif
