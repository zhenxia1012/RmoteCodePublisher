RmoteCodePublisher
==================
Description
-----------
Remote Code Publisher are mainly used for clients to manage their source code files on the server side.
It consists of three major components: code publisher server, client interface, message-passing system.<br>
A code publisher server
* manage the source code files for clients
* publish source code on the web page by HTML and show the dependency between files through link on the page.
* analyze the dependency between source code files
* the dependency relationship among files is maintained by NoSQL database based on unordered map.<br>
Client interface
* Provide a GUI interface for client to manage files on the server side
* Upload or download files, view contents of their directory, ask server to do dependency analysis or show the source code on web pages.<br>
The message-passing system is implemented by sockets and HTTP style messages. It asynchronously handles the request or response and synchronously sends files.

Main Job
--------
* Developed a remote code publisher server, local client, and communication channel that supported access from client to server
* Designed a NoSQL Database that supported queries, persistence to XML file as well as the reversion
* Designed a publisher that published C++ source file on web and built dependency between files by Abstract Syntax Tree
* Designed a client GUI program by WPF, which sent and downloaded files from server and viewed contents of repository
* Provided a synchronous request/response or asynchronous one-way message-passing system by sockets and HTTP style messages

