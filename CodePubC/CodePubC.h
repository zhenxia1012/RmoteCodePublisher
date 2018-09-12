#ifndef CODEPUBC_H
#define CODEPUBC_H
///////////////////////////////////////////////////////////////
// CodePubC.h - Code Publisher Client                        //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This package contains tools for client to accept user command 
and deal with server responses.

IClient struct declare the behaviors of CodePubC which is the truly
client object.

CodePubC define the actual behavior of functions declared by IClient
and it builds a brige between GUI and Server.

Maintanence Information:
========================
Required files:
---------------
CodePubC.h, CodePubC.cpp
HttpMessage.h, Comm.h
*/

#include <string>
#include <vector>
#include <sstream>
#include <fstream> 
#include <iostream>
#include "../HttpMessage/HttpMessage.h"

/////////////////////////////////////////////////////////////////////
// IClient class - contracts of handle user request

struct IClient
{
	virtual std::string getName() = 0;
	virtual void iniConn(const std::string& ip = "localhost", const size_t& port = 9070) = 0; // send user name and address to server
	virtual HttpMessage getMessage() = 0;
	virtual void sendFiles(const std::vector<std::string>& pfrom, const std::string& pto) = 0; // recursively send all the files in pfrom
	virtual void delFiles(const std::string& file) = 0;
	virtual void askCata(const std::string& dir) = 0; // ask server to show all the directories and files under dir
	virtual std::vector<std::string> getCata(const std::string& cata) = 0; // parse the message body if body composes of paths
	virtual void viewHtml() = 0; // ask server to show all the html file name
	virtual void loadHtml(const std::vector<std::string>& files) = 0; // ask server to send corresponding html files
	virtual void openHtml(const std::string& name) = 0; // open the html file on web 
};

/////////////////////////////////////////////////////////////////////
// ObjectFactory class - create client object

struct ObjectFactory
{
	IClient* createCli(const std::string& name); // create CodePubC object
};

#endif
