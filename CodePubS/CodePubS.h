#ifndef CODEPUBS_H
#define CODEPUBS_H
///////////////////////////////////////////////////////////////
// CodePubS.h - Code Publisher Server                        //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This package contains tools for server to handle user command.

Maintanence Information:
========================
Required files:
---------------
CodePublisher.h, CodePublisher.cpp
Comm.h, DependencyAnal.h
*/

#include <string>
#include <vector>
#include <sstream>
#include <fstream> 
#include <thread>
#include <iostream>

#include "../Comm/Comm.h"
#include "../DependencyAnal/DependencyAnal.h"
#include "../TypeAnal/TypeAnal.h"
#include "../CodePublisher/CodePublisher.h"

/////////////////////////////////////////////////////////////////////
// CodePubS class - CodePublisher Server

class CodePubS
{
public:
	void iniCliInfo(Socket& socket);
	void iniCliResource(Comm& comm);
	bool copyFile(const std::string& frompath, const std::string& topath);
    std::vector<std::string> getCata(const std::string& cata);
	bool delFile(const std::string& file);
	std::vector<std::string> findfiles(const std::string& dir, bool onlyName = false);
	std::vector<std::string> findfiles(const std::string& dir, const std::vector<std::string> patterns, bool onlyName = false);
	void depAnal(const std::string& dir);
	void htmlGen(const std::string& path);
	void sendFiles(Comm& comm, const std::vector<std::string>& files);
	void operator()(Socket socket);
private:
	std::string client;
	std::string cli_addr;
	std::string repoPath_ = "../CodePubS/RepoS";
};
#endif