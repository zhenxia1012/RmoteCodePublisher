RmoteCodePublisher
==================
Description
-----------
Remote Code Publisher are mainly used for clients to manage their source code files on the server side.
It consists of three major components: code publisher server, client interface, message-passing system.
A code publisher server
* manage the source code files for clients
* publish source code on the web page by HTML and show the dependency between files through link on the page.
* analyze the dependency between source code files
* the dependency relationship among files is maintained by NoSQL database based on unordered map.
Client interface
* Provide a GUI interface for client to manage files on the server side
* Upload or download files, view contents of their directory, ask server to do dependency analysis or show the source code on web pages.
The message-passing system is implemented by sockets and HTTP style messages. It asynchronously handles the request or response and synchronously sends files.

Sub Projects
------------
Project1
* Designed a NoSQL Database implemented by template class providing key/value pairs
* Implemented functionality, such as addition, deletion, insertion of database
* Implemented retrieving and updating data from local memory to database
* Developed persistence from database contents to a XML file as well as the reversion
* Developed functionality of querying database

Project2
▪	Packages of my design: TypeTable, TypeAnal, DependencyAnal, StrongComp, NoSqlDb, CodeAnalyzer
-	TypeTable: Provide a container class that stores type information for a collection of related source files.
-	TypeAnal: Find namespaces, classes, functions and global functions defined in a collection of files based on Abstract Syntax Tree. 
−	It first gets all absolute path of each file. Then, it parses each file to get all namespaces, classes, functions and global functions and stores them into an Abstract Syntax Tree. Finally, it dfs this tree to store information of each node into Type Table.
-	DependencyAnal: Find dependency relationship between files based on type table. 
−	First, get all absolute paths of files in the collection. Then, iterate each record in Type Table and find dependent files for each type (record).
-	StrongComp: Find SCCs in a given graph based on NoSql database. 
-	NoSqlDb: maintain dependency information.
▪	Developed persistence from result of dependency analysis and strong components to a XML file
▪	Developed command line to run the project

Project3
▪	Implemented creation of web page which captured the content of a single C++ source code file
-	Built HTML model files.
-	Fill in each tag by search its id. 
-	Loaded content of the source file and insert the content into tag "pre".
▪	Implemented expand and collapse of web page
▪	Implemented embedded HTML5 links in opened web page to dependent files with a label, at the top of the web page

Project4
▪	Designed a NoSQL Database that supported queries, persistence to an XML file as well as the reversion, implemented by template class based on key/value pairs, to maintain dependencies between files.
▪	Provided a Publisher server 
-	Published, as linked web pages, the contents of a set of C++ source code files
-	Analyzed dependency between files.
-	Showed catalog to client.
-	Receiving files from client.
-	Deleting files required by client.
▪	Provided a client GUI program that can upload files3, and view Repository contents, implemented through WPF
-	HTML Tab:
−	"View HTML" button could view the HTML file name on server of the specific client
−	"Download" button could download the HTML file you choose
−	"Open" could open the HTML file you choose, but you must download it before opening it
-	File List Tab:
−	"Select Directory" button will display the current directory and you could choose one to view the details. When the details are shown in the textbox, you could choose one or more files to send by click "Upload" button
−	"Upload Path" let you define the path of received file in server before you upload files and the default path will be "../CodePubS/RepoS/ClientName", the path you type will be added to the tail of the default path
-	Catalog:
−	"View Catagory" could display the content of the directory you choose. It's default path is its root path in Server "../CodePubS/RepoS/ClientName".
-	Dialog Tab:
−	Display the message content sent from server
▪	Provided a message-passing communication system, based on sockets and HTTP style messages using either synchronous request/response or asynchronous one-way messaging, for sending and receiving streams of bytes or any other type of data
-	Client and server both have "Comm" respectively which contains a sender, a receiver and offer a function to create different types of HTTP style messages.
-	HTTP Message: header (attributes) + body (bytes).
-	Sender and receiver both have a BlockingQueue for synchronization between threads.
-	Sender puts every new message into its queue. There is a thread continuously sending message, serialized to bytes (serialized to string first, then byte), to the corresponding receiver if the queue is not empty.
-	Receiver handles request if its queue is not empty. There is thread continuously receiving message and putting them into queue.
-	When sending files, receiver would get the size of the file from the header of the message. Then, sender and receiver synchronized their transmission of files block by block.
