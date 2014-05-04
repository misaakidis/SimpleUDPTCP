SimpleuUDPTCP
=============

Simple UDP and TCP servers and clients. Support authentication and file exchange.


============ Linux Socket Programming ================================  
Contributors: Daniel Aguado 2012955406, Marios Isaakidis 2009437805  
License: GPLv2 or later  
License URI: http://www.gnu.org/licenses/gpl-2.0.html


=============Description of the program and files:==========================

We write a program for authenticating and transferring files from a client to a server 
by a socket connection. The first part is based on UDP sockets. We have a UDP client and
a UDP server. The UDP client must authenticate in the UDP server with a login and a 
password. UDP server must chek if the login and the pasword of the UDP client are right 
or wrong. The file named "login" have the logins and the passwords of the users. After 
authenticated, in the second part, we have a connection based in TCP socket for transferring
files from the TCP client to the TCP server. The client sends the filename to the server
and is waiting for a response, whether exists a file with such a filename and, in a case of
positive response, whether to overwrite or append to it or cancel the transfer. If there is
no such a file then the TCP client asks us if we should proceed with the transfer of the file
and the action is followed accordingly.
*  udpserver.c: Create a UDP socket, bind on a port and receive the login and password from the 
UDP client for authenticating it. Check in file "login" if the credential are right
*  udpclient.c: Create a UDP socket, bind on a port and send its login and password to the 
UDP client for authenticating. Controls the logic of how many times a user should try
to connect and what to do in case of success/failure at authentication.
*  tcpserver.c: Create, initalize bind and put to listen a TCP socket for transferring and managing
files form the TCP clients to the TCP server.
*  tcpclient.c: Create, and bind a TCP socket for transferring files form the TCP client to the TCP
server.
*  Makefile: Compile the files.c and creates the executables for the program. It has a method for 
deleting the executables created before and can start the UDP and TCP servers at the same time
in predefined ports.
*  stringUtils.c: File with the external string-manipulation functions used in the udpserver.c.
*  stringUtils.h: File with the headers of the external functions used in the udpserver.c.
*  README: File with documentation that explains the way to use the program and provide information about
the developers and files.
	

==================== Description about how to run the program ============================

First place run the Makefile in a terminal the command "make" in the directory with the all the source files 
of the program. Then start the sockets servers using  the command "make start" . Later, you must run the
UDP client for athenticating (you can see in login file the users and the passwords, the distribution 
is login:password). If you are well authenticated, the TCP client starts automaticly. Then you must 
decide the file to transfer to the TCP server.  
Calling arguments:  
udpserver [usersfile] [server_port]  
udpclient [IP_server] [server_port]  
tcpserver [server_port]  
tcpclient [IP_server] [server_port]



========================= Known Bugs =======================================

There is known bug we plan to fix, in the send of files from TCP client to the TCP server. We did not manage
to find a way to stop listening on the socket so the program stalls there after sending all the data of the
file. The file is sent correctly and by terminating either the client or the server after some time everything
should be working fine as an ungly workaround.