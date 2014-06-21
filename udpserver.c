/*

udpserver.c

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
#include "stringUtils.h"
#include <sys/socket.h>
#include <netinet/in.h>

#include "constants.h"

int parseUsersFile(FILE *, char [MAX_USERS][2][USER_PASS_LENGTH]);
bool checkIfValidUser(char *, char [MAX_USERS][2][USER_PASS_LENGTH]);

int main(int argc, char *argv[])
{

    if(argc != 3)
    {//Check if program is correclty called
        fprintf(stderr, "Usage: udpserver [usersfile] [server_port]\n");
        return USAGE_ERR;
    }

    FILE * fp = fopen(argv[1], "r");
    if (fp == NULL)
    {//Check if the user file given exists
        fprintf(stderr, "Unable to open users file.\n");
        return USERS_REPO_ERR;
    }

    char users[MAX_USERS][2][USER_PASS_LENGTH];
    if ( parseUsersFile(fp, users) <= 0)
    {//Parsing the users from the usersfile
        fprintf(stderr, "Cannot parse the users file.\n");
        return USERS_REPO_PARSE_ERR;
    }

    int serverPort = atoi(argv[2]);
    if(serverPort<=0 || serverPort>65535)
    {//Checking if the port given is valid and withing the accepted values
        fprintf(stderr, "The port number given is wrong.\n");
        return BAD_PORT_NUM_ERR;
    }

    printf("Starting UDP server...\n");
    //Initializing the socket variables
    int sockfd, bytesReceived;
    struct sockaddr_in serverAddr,clientAddr;
    socklen_t len;
    char buffer[NETWORK_BUFFER];
    //Creating a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("Cannot open socket ");
        return SOCK_OPEN_ERR;
    }

    //Preparing serverAddr
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    //Binding on the socket
    if(bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Cannot bind on UDP port");
        close(sockfd);
        return SOCK_BIND_ERR;
    }
    fprintf(stderr, "Bind on UDP port %d.\n", serverPort);

    bool correctCredentials;

    while(true)
    {
        correctCredentials = false;
        //for(int i = 0; i < 3 && !correctCredentials; i++)
        //{
            len = sizeof(clientAddr);
	    //Listening from clients
            bytesReceived = recvfrom(sockfd, buffer, NETWORK_BUFFER, 0, (struct sockaddr *)&clientAddr, &len);
            correctCredentials = checkIfValidUser(buffer, users);
        //}
        //Check if the credentials provided are valid, send back an ACK/NACK message
        if(correctCredentials)
        {
            printf("User connected on the server.\n");
            sendto(sockfd, "^F", NETWORK_BUFFER, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        }
        else
        {
            printf("You have not provided valid credentials.\n");
            sendto(sockfd, "^U", NETWORK_BUFFER, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        }
    }

    //close the socket
    close(sockfd);
    return 0;
}


//Function to parse the users from a usersfile
int parseUsersFile(FILE *fp, char users[MAX_USERS][2][USER_PASS_LENGTH])
{
    char user[USER_PASS_LENGTH];
	int c;
	int charsRead;
	int usersFound = 0;

	do
	{
		charsRead = 0;
		c = fgetc(fp);
		user[0] = '\0';
		while(c != '\n' && c != EOF)
		{
			if (charsRead++ > (USER_PASS_LENGTH*2 + 1))
			{
			    fprintf(stderr, "%s : Username or password more than %d characters.", user, USER_PASS_LENGTH);
				break;
			}
			user[charsRead-1] = c;
			user[charsRead] = '\0';
			c = fgetc(fp);
		}
		if (user[0] != '\0')
		{
		    char username[USER_PASS_LENGTH],  password[USER_PASS_LENGTH];
			char * token = xstrtok(user, ":");
			if(token != NULL && token != "")
                strcpy(username, token);
            else
                continue;
            token = xstrtok(NULL, ":");
            if(token != NULL && token != "")
                strcpy(password, token);
            else
                continue;
            strcpy(users[usersFound][0], username);
            strcpy(users[usersFound][1], password);
			usersFound++;
		}
	}while(usersFound < MAX_USERS && c != EOF);

    return usersFound;
}

bool checkIfValidUser(char *message, char users[MAX_USERS][2][USER_PASS_LENGTH])
{
    char usernamepassword[USER_PASS_LENGTH*2];
    for(int i=0; i<MAX_USERS; i++)
    {
        strcpy(usernamepassword, users[i][0]);
        strcat(usernamepassword,":");
        strcat(usernamepassword, users[i][1]);
        if(xstrcmp(usernamepassword, message) == true)
            return true;
    }
    return false;
}
