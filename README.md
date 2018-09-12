# RmoteCodePublisher
Remote Code Publisher are mainly used for clients to manage their source code files on the server side. 

Remote Code Publisher are mainly used for clients to manage their source code files on the server side. It consists of three major components: code publisher server, client interface, message-passing system.

A code publisher server
-	manage the source code files for clients
-	publish source code on the web page by HTML and show the dependency between files through link on the page.
-	analyze the dependency between source code files
-	the dependency relationship among files is maintained by NoSQL database based on unordered map.

Client interface
-	Provide a GUI interface for client to manage files on the server side
-	Upload or download files, view contents of their directory, ask server to do dependency analysis or show the source code on web pages.

The message-passing system is implemented by sockets and HTTP style messages. It asynchronously handles the request or response and synchronously sends files.

