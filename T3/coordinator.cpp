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


class centralizedMutex
{
	mutex remote;
	queue<pair<string, int>> q;
	bool lock = false;
	map<string, int> granted; //

	void grant()
	{
		lock = true;
		pair<string, int> next = q.front();
		q.pop();
		string message = "2";
		//envia GRANT ao processo
		send(get<1>(next), message.c_str(), BUFFER_SIZE, 0);
		//cout << to_string(get<0>(next)) + " 2\n";
		writeLog(get<0>(next) + " 2");
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
		centralizedMutex(){};
		void request(string pid, int n_socket)
		{
			remote.lock();
			//cout << to_string(pid) + " 1\n";
			writeLog(pid + " 1");
			q.push(make_pair(pid, n_socket));
			if (granted.find(pid) == granted.end())
			{
				granted[pid] = 0;
			}
			if(!lock)
			{
				grant();
			}
			remote.unlock();
		}

		void release(string pid)
		{
			remote.lock();
			//cout << "3" << endl;
			writeLog(pid + " 3");
			if (q.empty())
			{
				lock = false;
			}
			else{
				grant();
			}
			remote.unlock();
		}
};

centralizedMutex cmutex;

int server()
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket ,
		max_clients = 129 , activity, i , valread , sd;
	int max_sd;
	int manager_socket;
	struct sockaddr_in address;

	vector<int> client_socket;
		
	char buffer[BUFFER_SIZE]; 
		
	//set of socket descriptors
	fd_set readfds;
			
	//inicia client_socket[] em 0
	for (i = 0; i < max_clients; i++)
	{
		client_socket.push_back(0);
	}
		
	//cria master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//master socket para permitir multiplas conexões
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//tipo de socket criado
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//binda o socket em localhost na porta 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	//printf("Listener on port %d \n", PORT);
		
	//maximo de 3 conexões pendentes para o master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//aceita a conexão
	addrlen = sizeof(address);
	//puts("Waiting for connections ...");
	
	while(TRUE)
	{
		//limpa o set de sockets
		FD_ZERO(&readfds);
	
		//adiciona master socket a set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//adiciona child sockets ao set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//se socket descriptor é valido, adicionar na read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//necessario para o select
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//espera por uma atividade em um dos sockets
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//Se é no master socket, então é uma nova conexão
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
				
			//adciona novo socket na array de sockets
			for (i = 0; i < max_clients; i++)
			{
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					//printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//se não, é algo acontecendo em um dos outros sockets
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//lê a mensagem e verifica se esta fechando conexão
				if ((valread = read( sd , buffer, BUFFER_SIZE)) == 0)
				{
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					//printf("Host disconnected , ip %s , port %d \n" ,
						//inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Fecha o socket e marca como 0 para reuso
					close( sd );
					client_socket[i] = 0;
				}
					
				//Processar mensagem recebida
				else
				{
					string received(buffer);
					//mensagem que estabelece o socket do manager
					if (received.find("manager")!= string::npos)
					{
						manager_socket = sd;
					}
					else
					//envia mensagem recebida ao manager
					{
						string str_socket = to_string(sd);
						string answer = received + "|" + str_socket;						
						send(manager_socket , answer.c_str() , BUFFER_SIZE , 0 );
					}
				}
			}
		}
	}
	return 0;
}

int manager()
{
	//identico ao cliente
	int server_sock = 0, n;
    struct sockaddr_in serv_addr;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
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
	//envia mensagem se identificando como manager
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
		//lê mensagem do servidor
		n = read( server_sock , buffer, BUFFER_SIZE);
		if (n < 0)
		{
			printf("Erro lendo do socket");
			exit(1);
		}
		string server_response(buffer);
		//cout << server_response << endl;
		stringstream test(server_response);
		string segment;
		vector<string> seglist;

		while(getline(test, segment, '|'))
		{
			seglist.push_back(segment);
		}
		if (seglist[0] == "1") //se REQUEST
		{
			//cout << seglist[2] << endl;
			cmutex.request(seglist[1],stoi(seglist[2]));
		}
		else if (seglist[0] == "3") //se RELEASE
		{
			//cout << seglist[2] << endl;
			cmutex.release(seglist[1]);
		}
	}
	close(server_sock);
	return 0;
}


int main(int argc , char *argv[])
{
	thread server_t (server);
	thread manager_t (manager);
    server_t.join();
	manager_t.join();
	return 0;
}
