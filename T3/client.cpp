// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;


#define PORT 8888
#define BUFFER_SIZE 1024
   
int main(int argc, char const *argv[])
{
    int sock = 0, n;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int pid = getpid();
    string mypid = to_string(pid);    
    string request = "1|";

    string message = request + mypid;
    string response;

    for (int i = 0; i < 5; i++)
    {
        
        n = send(sock , message.c_str(), BUFFER_SIZE, 0);
        if (n < 0)
        {
            printf("Erro escrevendo no socket");
            exit(1);
        } 
        char buffer[BUFFER_SIZE];
        n = read( sock , buffer, BUFFER_SIZE);
        if (n < 0)
        {
            printf("Erro lendo do socket");
            exit(1);
        }
        cout << buffer << endl << i << endl;
        sleep(3);
    }
    close(sock);

    return 0;
}