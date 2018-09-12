Requirement#1: User has GUI implemented by WPF.
Requirement#2: In this project, whenever using "new", I have used "delete" for deleting pointer and heap-based memory management.
Requirement#3: provide a Repository program that provides functionality to publish, as linked web pages, the contents of a set of C++ source code files.
Requirement#4: for the publishing process, satisfy the requirements of CodePublisher developed in Project #3.
  HTML Tab:
	"View HTML" button could view the html file name on server of the specific client.
	"Download" button could download the html file you choose.
	"Open" could open the Html file you choose, but you must download it before opening it.

Requirement#5: provide a Client program that can upload files3, and view Repository contents, as described in the Purpose section, above.
  File List Tab:
	  "Select Directory" button will display the the current directory and you could choose one to view the details;
	  when the details are shown in the textbox, you could choose one or more files to send by click "Upload" button;
	  "Upload Path" let you define the path of received file in server before you upload files and the default path 
	  will be "../CodePubS/RepoS/ClientName", the path you type will be added to the tail of the default path;
  Catalog:
	"View Catagory" could display the content of the directory you choose. It's default path is its root path in Server "../CodePubS/RepoS/ClientName".

  Dialog Tab:
	deisplay the message content sent from server

Requirement#6: this project provide a message-passing communication system, based on Sockets, used to access the Repository's functionality from another process or machine.
Requirement#7: The communication system provide support for passing HTTP style messages using either synchronous request/response or asynchronous one-way messaging. 
Requirement#8: and the  The communication system shall also support sending and receiving streams of bytes6. Streams will be established with an initial exchange of messages.
Requirement#9: the project include an automated unit test suite that demonstrates you meet all the requirements of this project4 including the transmission of files.