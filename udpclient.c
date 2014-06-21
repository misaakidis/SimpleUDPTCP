/*

udpclient.c


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
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "constants.h"

int main(int argc, char *argv[])
{

/* Check parameters from command line */

    if(argc != 3)//check number of argc in command line
    {
        fprintf(stderr,"Usage: udpclient [IP_server] [server_port]\n");
        return USAGE_ERR;
    }

    int serverPort = atoi(argv[2]);//check number of TCP server port
    if(serverPort<=0 || serverPort>65535)
    {
        fprintf(stderr, "The server port number given is wrong.\n");
        return BAD_PORT_NUM_ERR;
    }

    int sockfd, bytesReceived;
    struct sockaddr_in serverAddr, clientAddr;
    struct hostent *hp;
    socklen_t len;

    /* Create UDP socket */

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)//check UDP socket is created correctly
    {
        perror("Cannot open socket ");
        return SOCK_OPEN_ERR;
    }

    hp = gethostbyname(argv[1]);
    if (hp == 0)//check assigment of UDP server host
    {
        perror("Unknown Host ");
        close(sockfd);
        return UNKNOWN_HOST_ERR;
    }
    
    /* Bind any port number */

    bzero((char *) &clientAddr, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(LOCAL_PORT);

    if (bind(sockfd, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0)//check UDP socket is bind correctly
    {
        perror("Cannot bind ");
        close(sockfd);
        return SOCK_BIND_ERR;
    }


    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(serverPort);

    /* Insert credentials */
    
    bool correctCredentials = false;
    char username[USER_PASS_LENGTH], password[USER_PASS_LENGTH], buffer[NETWORK_BUFFER];
    for(int i = 0; i < 3 ;i++)//check number of tries to login
    {
        printf("Please enter username:");
        gets(username);
        printf("Please enter password:");
        gets(password);
        strcpy(buffer, username);
        strcat(buffer, ":");
        strcat(buffer, password);
        //send credential request for confirming to UDP server
        sendto(sockfd, buffer, NETWORK_BUFFER, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        //read credential answer for confirming from UDP server
        bytesReceived = recvfrom(sockfd, buffer, NETWORK_BUFFER, 0, (struct sockaddr *)&clientAddr, &len);
        if(buffer[0] == '^' && buffer[1] == 'F')
        {
            correctCredentials = true;
            printf("You have connected successfully to the server.\n\n\n");
            char callFileServer[20] = "./tcpclient ";
            strcat(callFileServer, argv[1]);
            strcat(callFileServer, " 2424");
            system(callFileServer);
            close(sockfd);
            return 0;
        }
        else
            printf("The credentials you have provided are not valid.\n\n");
    }
    printf("You have provided three times wrong credentials.\n");

    close(sockfd);
    return 0;
}
