///////////////////////////////////////////////////////////////
// Typetable.cpp - Publish Html for files                    //
//                                                           //
// Language:    Visual C++                                   //
// Platform:    Alienware15, Windows 10                      //
// Author:      Zhen Xia, CIS687, Syracuse University        //
///////////////////////////////////////////////////////////////
#include "CodePublisher.h"

using namespace CodePublisher;

///////////////////////////////////////////////////////////////
// class: CodePublisher - make HTML content for source file
//
//----< Constructor >----------------------------------------------------

CodePub::CodePub(const NoSqlDb<std::string>& db_)
	:nosqldb(db_) {}
//----< initialize the HTML model >--------------------------------------

void CodePub::iniHtmlMd(std::string htmdPath, std::string ctmdPath)
{
	htmlMd = textFileToString(htmdPath);
	htmlCAT = textFileToString(ctmdPath);
	//std::cout << "\n\nhtml model: \n" << htmlModel;
}
//----< make HTML content for a source file >----------------------------

std::string CodePub::makeHtml(const std::string name, const std::string path)
{
	std::string prefix = "Cpp11-";
	std::string html_ = htmlMd;
	std::string file = textFileToString(path);
	std::vector<std::string> keys = nosqldb.keys();
	//std::cout << "\n " << name;
	//std::cout << "\n" << file;
	file = rpMKUPChar(file);
	file = mkDiv(file);
	file += "\n";

	size_t index = html_.find("id=\"filename\"") + 14;
	html_.insert(index, prefix + name);
	index = html_.find("Dependencies") + 19;
	for (auto item : keys)
	{
		Element<std::string> tem_elem = nosqldb.value(item);
		if (item == name)
			continue;
		for (auto pchild : tem_elem.children)
		{
			if (pchild == name)
			{
				std::string dp_ = "        <a href=\"" + prefix + item + ".htm\">" + prefix + item + "</a>\n";
				html_.insert(index, dp_);
				index += dp_.length();
				break;
			}
		}
	}
	index = html_.find("id=\"code\"") + 11;
	html_.insert(index, file);
	//std::cout << "\n\n" << file;
	return html_;
}
//----< make catalog for all Html files in a specific directory >-----------

std::string CodePub::mkCata(std::string path)
{
	std::cout << "\n  Make Html of catalog";
	path = FileSystem::Path::getFullFileSpec(path);
	std::string html_ = htmlCAT;
	std::vector<std::string> files = FileSystem::Directory::getFiles(path, "*.htm");
	//std::cout << "\n\n file size: " << files.size();
	size_t index = html_.find("<title>") + 7; //path
	html_.insert(index, path);
	index = html_.find("id=\"path\"") + 10; //path
	html_.insert(index, path);
	index = html_.find("id=\"catalog\"") + 14; // specify links for all Html files
	for (auto item : files)
	{
		if (item == "Catalog.htm")
			continue;
		std::string link = "        <a href=\"" + item + "\">" + item.substr(0, item.length() - 4) + "</a>\n";
		html_.insert(index, link);
		index += link.length();
	}

	//std::cout << "\n\n" << html_;
	return html_;
}
//----< make Html for all files in Non-SQL database >-----------------------

void CodePub::mkAllHtml(std::string svDir)
{
	std::vector<std::string> keys = nosqldb.keys();
	for (auto item : keys)
	{
		std::cout << "\n  Make Html for file \"" << item << "\"";
		std::string html_;
		Element<std::string> tem_elem = nosqldb.value(item);
		html_ = makeHtml(item, tem_elem.description);
		stringToTextFile(html_, svDir + "/Cpp11-" + item + ".htm");
	}

	std::string cata_ = mkCata(svDir);
	stringToTextFile(cata_, svDir + "/Catalog.htm");
}
//----< replace makeup characters >-----------------------------------------

std::string CodePub::rpMKUPChar(std::string file)
{
	for (size_t i = 0; i <= file.length(); i++)
	{
		if (file[i] == '<')
			file.replace(i, 1, "&lt;");
		if (file[i] == '>')
			file.replace(i, 1, "&gt;");
	}
	return file;
}
//----< make Div label for class >------------------------------------------

std::string CodePub::mkDiv(std::string file)
{
	size_t filelen = file.length();
	//std::cout << "\n  file: " << file.substr(filelen - 50, 49);
	size_t index = 0; int id = 0;
	while (index < filelen)
	{
		index = file.find_first_of("{}", index);
		//std::cout << "\n  find at index: " << index;
		if (index == std::string::npos)
			break;
		index++;
		//std::cout << "\n  find " << file[index - 1] << " : " << file.substr(index - 20, 20);
		if (file[index - 1] == '{')
		{
			//std::cout << "\n  button: " << id1 << " div: " << id;
			int id1 = id; id++;
			std::string button = "<button id=\"" + std::to_string(id1) + "\" type=\"button\" onclick=\"expand("
				+ std::to_string(id1) + "," + std::to_string(id) + ")\">-</button>";
			size_t tem_index = file.rfind("\n", index - 1) + 1;
			if (tem_index == std::string::npos)
				tem_index = 0;

			file.insert(tem_index, button);
			index += button.length();
			filelen += button.length();
			std::string  span_ = "<span id = \"" + std::to_string(id) + "\">";
			file.insert(index - 1, span_);
			index += span_.length();
			filelen += span_.length();
			//std::cout << "\n  added <span> now index: " << file[index];
			id++;
		}
		else
		{
			file.insert(index, "</span>");
			index += 7;
			filelen += 7;
			//std::cout << "\n  added </span>";
		}
		//std::cout << "\n  index: " << index << " filelen: " << filelen;
	}
	return file;
}
//----< read text file contents into string >-------------------------------

std::string CodePub::textFileToString(const std::string& path)
{
	std::ostringstream out;
	try
	{
		std::ifstream in(path);
		if (!in.good())
			throw(std::exception(("can't open source file \"" + path + "\"").c_str()));
		out << in.rdbuf();
		in.close();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  " << ex.what() << std::endl;
	}

	return std::move(out.str());
}
//----< read string contents into text file >-------------------------------

bool CodePub::stringToTextFile(const std::string& content, const std::string path)
{
	std::ofstream out(path);
	try
	{
		if (out.is_open())
		{
			out << content;
			out.close();
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Write file into \"" + path + "\" failed \n";
		std::cout << "\n  " << ex.what();
		return false;
	}

	return true;
}


#ifdef TEST_CODEPUBLISHER

#include "../Utilities/Utilities.h"

using Utils = Utilities::StringHelper;

void main()
{
	Utils::Title("Demo functionality of package CodePublisher");
	Utilities::putline();

	Utils::title("Construct NonSqlDb");
	NoSqlDb<std::string> db("demo_db");
	Element<std::string> file1;
	file1.key = "Convert.h";
	file1.name = "Convert.h";
	file1.category = "test1";
	file1.description = "../Convert/Convert.h";
	file1.timeDate = "none";
	file1.children.push_back("Convert.cpp");
	file1.children.push_back("Test.cpp");
	file1.data = "Convert.h's StrData from test1";
	db.save(file1.name, file1);
	Element<std::string> file2;
	file2.key = "Convert.cpp";
	file2.name = "Convert.cpp";
	file2.category = "test1";
	file2.description = "../Convert/Convert.cpp";
	file2.timeDate = "none";
	file2.children.push_back("Convert.h");
	file2.data = "Convert.cpp's StrData from test1";
	db.save(file2.name, file2);
	db.showData();
	Utilities::putline();

	Utils::title("Demo functionality of CodePub");
	CodePub cdpb_(db);
	cdpb_.iniHtmlMd();
	cdpb_.mkAllHtml();
}
#endif

