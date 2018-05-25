#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <thread>

using namespace std;



void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void recv_msg(int sock)
{
    char buffer[256];
    int n = read(sock,buffer,255);
    while (n >=0)
    {
        printf("%s\n",buffer);
        n = read(sock,buffer,255);
    }
    if (n < 0) 
         error("ERROR reading from socket");
}

void write_msg(int sockfd)
{
    char buffer[256];
    while(true)
    {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        int n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
        {
             error("ERROR writing to socket");
             break;
        }
    }
    
}

void new_connect(char* serv, int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
   
    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(serv);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    thread write_thd(write_msg, sockfd);
    thread read_thd(recv_msg, sockfd);

    read_thd.join();
    write_thd.join();
}


int main(int argc, char *argv[])
{
    if (argc < 3) {
         fprintf(stderr,"ERROR, no host name and port provided\n");
         exit(1);
     }
    int n = 1000;
    while(n >0)
    {
        new_connect(argv[1], atoi(argv[2]));
        n--;
    }
    return 0;
}
