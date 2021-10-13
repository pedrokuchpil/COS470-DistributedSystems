//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros


#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <map>
#include <sstream>
#include <fstream>

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define BUFFER_SIZE 1024

using namespace std;

void writeLog(string message)
{
    fstream myfile;
    myfile.open ("console.log",ios_base::app);
    myfile << message  << endl;
    myfile.close();
}


class distributedMutex
{
	private:
		mutex remote;
		queue<tuple<int, int>> mutexQueue;
		bool Lock = false;
		map<int, int> granted; //

		void grant()
		{
			Lock = true;
			tuple<int, int> next = mutexQueue.front();
			mutexQueue.pop();
			string message = "2";
			send(get<1>(next), message.c_str(), BUFFER_SIZE, 0);
			cout << to_string(get<0>(next)) + " 2\n";
			writeLog(to_string(get<0>(next)) + " 2");
			if (granted.find(get<0>(next)) == granted.end())
			{
				granted[get<0>(next)] = 1;
			}
			else
			{
				granted[get<0>(next)] += 1;
			}
		}
	public:

		distributedMutex(){};

		void request(int pid, int pfd){
			remote.lock();
			cout << to_string(pid) + " 1\n";
			writeLog(to_string(pid) + " 1");
			mutexQueue.push(make_tuple(pid, pfd));
			if (granted.find(pid) == granted.end()){
				granted[pid] = 0;
			}
			if(!Lock){
				grant();
			}
			remote.unlock();
		}

		void release(string pid){
			remote.lock();
			cout << "3" << endl;
			writeLog(pid + " 3");
			if (mutexQueue.empty()){
				Lock = false;
			}
			else{
				grant();
			}
			remote.unlock();
		}
		
		void printQueue(){
			remote.lock();
			queue<tuple<int, int>> copy = mutexQueue;
			while (!copy.empty()){
				cout << get<0>(copy.front()) << " ";
				copy.pop();
			}
			std::cout << std::endl;
			remote.unlock();
		}
		void printCount(){
				remote.lock();
				for (auto it = granted.cbegin(); it != granted.cend(); ++it)
				{
					std::cout << it->first << " | " << it->second << "\n";
				}
				remote.unlock();
			}
};

distributedMutex dmutex;

int server(int n_clients)
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket ,
		max_clients = n_clients+1 , activity, i , valread , sd;
	int max_sd;
	int manager_socket;
	struct sockaddr_in address;

	vector<int> client_socket;
		
	char buffer[BUFFER_SIZE]; 
		
	//set of socket descriptors
	fd_set readfds;
			
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket.push_back(0);
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	
	int n_processes_finished = 0;
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, BUFFER_SIZE)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 1;
					n_processes_finished++;
				}
					
				//Processar mensagem recebida
				else
				{
					string received(buffer);
					//establish manager socket
					if (received.find("manager")!= string::npos)
					{
						manager_socket = sd;
					}
					else
					//send received message to manager
					{
						string str_socket = to_string(sd);
						string answer = received + "|" + str_socket;						
						send(manager_socket , answer.c_str() , BUFFER_SIZE , 0 );
					}
				}
			}
		}
		if (n_processes_finished >= 10)
		{
			break;
		}
	}
	return 0;
}

int manager(int n_processes, int repetitions)
{
	int server_sock = 0, n;
    struct sockaddr_in serv_addr;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (connect(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

	string str_manager = "manager";

	n = send(server_sock , str_manager.c_str(), BUFFER_SIZE, 0);
		if (n < 0)
		{
			printf("Erro escrevendo no socket");
			exit(1);
		} 
	char buffer[BUFFER_SIZE];
	int count = 0;
	while(TRUE)
	{
		n = read( server_sock , buffer, BUFFER_SIZE);
		if (n < 0)
		{
			printf("Erro lendo do socket");
			exit(1);
		}
		string server_response(buffer);
		cout << server_response << endl;
		stringstream test(server_response);
		string segment;
		vector<string> seglist;

		while(getline(test, segment, '|'))
		{
			seglist.push_back(segment);
		}
		if (seglist[0] == "1")
		{
			cout << seglist[2] << endl;
			dmutex.request(stoi(seglist[1]),stoi(seglist[2]));
		}
		else if (seglist[0] == "3")
		{
			cout << seglist[2] << endl;
			dmutex.release(seglist[1]);
		}
		count++;
	}
	close(server_sock);
	return 0;
}

void terminal(){
	while (true){
		int var;
		cin >> var;
		if (var == 1)
		{
			dmutex.printQueue();
		}
		else if(var == 2){
			dmutex.printCount();
		}
		else if(var == 3) {
			return;
		}
		else{
			cout<<"Entrada inválida ";
		}
	}
}

int main(int argc , char *argv[])
{
	int n = atoi(argv[1]);
	int r = atoi(argv[2]);
	thread server_t (server,n);
	thread manager_t (manager,n,r);
    server_t.join();
	manager_t.join();
	return 0;
}
