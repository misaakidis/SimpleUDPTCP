/*
tcpserver.c

Copyright 2012, Isaakidis Marios, Daniel Aguado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "constants.h"

//file found = 1
//file not found = 0

//write and overwrite =1
//apend =0

//tcpserver port
//argc1, argv0: tcpclient.exe
//argc2, argv1: IP address TCP Server

int main( int argc, char *argv[] )
{
    int sockfd, newsockfd, serverPort, clientLength, nBytes;;
    char buffer[BUFSIZ];
    char option=' ';
    int sent=0;
    struct sockaddr_in serverAddr, clientAddr;

/* Check parameters from command line */

    if(argc != 2){//check number of argc in command line
        fprintf(stderr,"Usage: tcpserver [server_port]\n");
        return USAGE_ERR;
    }

    printf("Starting TCP server...\n");

    serverPort = atoi(argv[1]);
    if(serverPort<=0 || serverPort>65535)//check number of TCP server port
    {
        fprintf(stderr, "The port number given is wrong.\n");
        return BAD_PORT_NUM_ERR;
    }

    /* Create a TCP socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)//check TCP socket is created correctly
    {
        perror("Error opening TCP socket");
        return SOCK_OPEN_ERR;
    }

    /* Initialize TCP socket structure */

    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    /* Bind the host address using bind() call */

    if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)//check TCP socket is bind correctly
    {
        perror("Error on binding TCP socket");
        return SOCK_BIND_ERR;
    }

    /* Start listening for the clients, TCP server wait for the incoming TCP client connection */

    listen(sockfd, BACKLOG);
    clientLength = sizeof(clientAddr);

    /* Accept actual connection from the TCP client */

    newsockfd = accept(sockfd, (struct sockaddr *) &clientAddr, (socklen_t *) &clientLength);
    if (newsockfd < 0)//check actual connection is created correctly
    {
        perror("Error on accepting actual TCP client");
        return ACC_CONN_ERR;
    }

    /* Start communicating between TCP client and server */

    bzero(buffer,BUFSIZ);
    nBytes = read( newsockfd,buffer,BUFSIZ );
    if (nBytes < 0)
    {
        perror("Error reading from TCP socket");
        return SOCK_READ_ERR;
    }
    printf("This is the file to transffer: %s\n",buffer);

    /* Searching the file or creating file */
    FILE * file;
    char sample[10];
    if (fopen(buffer, "r") != NULL)
    {
        nBytes = write(newsockfd,"1",1);
        if (nBytes < 0)
        {
            perror("Error writing to TCP socket");
            return SOCK_WRITE_ERR;
        }
    }
    else
    {
        nBytes = write(newsockfd,"0",1);
        if (nBytes < 0)
        {
            perror("Error writing to TCP socket");
            return SOCK_WRITE_ERR;
        }
    }


    nBytes = read( newsockfd, sample,1 );
    if (nBytes < 0)
    {
        perror("Error reading from TCP socket");
        return SOCK_READ_ERR;
    }

    option = sample[0];
    char readBuffer[NETWORK_BUFFER];
    switch (option)
    {
    case '1': //write or overwrite file
        if ((file = fopen(buffer, "w")) == NULL)
        {
            fprintf(stderr, "Cannot write on file.");
            return FILE_WRITE_ERR;
        }
        break;
    case '0': //append file
        if ((file = fopen(buffer, "a+")) == NULL)
        {
            fprintf(stderr, "Cannot append to file.");
            return FILE_APP_ERR;
        }
    }

    while((nBytes = read(newsockfd, readBuffer, NETWORK_BUFFER)) > 0)
    {
        if(nBytes < 0)
        {
            fprintf(stderr, "Error while receiving the file.\n");
            return FILE_RCV_ERR;
        }
        fprintf(file, readBuffer);
        fprintf(stderr, readBuffer);
        if(readBuffer == "")
            break;
    }
    fclose(file);
    sent=1;


    /* Write a response to the TCP client */
    if (sent)
    {
        nBytes = write(newsockfd,"Transfer completed and successful",33);
    }
    else
    {
        printf("Error writting the file in TCP server");
    }
    if (nBytes < 0)
    {
        perror("Error writing to TCP socket");
        return SOCK_WRITE_ERR;
    }

    return 0;
}
