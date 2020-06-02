//EXTERNAL INCLUDES
#include <ws2tcpip.h>
#include <iostream>
#include <string>
//INTERNAL INCLUDES
#include "launcher/ra_launcher_netcode.h"
#include "ra_launcher.h"
#include "ra_utils.h"

//Get winsock libary
#pragma comment(lib, "Ws2_32.lib")


///PUBLIC FUNCTIONS:
//Create connection to launcher
void Launcher_Netcode::SetLauncher(Launcher* launcher)
{
	this->launcher = launcher;
	this->state = NetState::CLOSED;
}


//Start winsock and connect to server
bool Launcher_Netcode::SetupConnection()
{
	//Start winsock
	StartWinsock();
	//Connect to server
	ConnectToServer();

	//Return false if failed
	if (this->state == NetState::CLOSED) return false;

	//Create receive thread for receiving messages from server
	this->receiveThread = std::thread(&Launcher_Netcode::Receive, this);

	//Return true
	return true;
}
//Handle incoming messages and send replies
bool Launcher_Netcode::HandleConnection()
{
	//If there is an active message
	if (this->launcher->GetActiveMessage() != nullptr)
	{
		//Send the message
		this->Send();
		//Reset
		this->launcher->SetActiveMessage(nullptr);
	}

	return (this->state != NetState::CLOSING);
}
//Close connection down and end netcode
void Launcher_Netcode::CloseConnection()
{
	//Close connection to Server, cleanup WinSock and release the launcher window's instance.
	this->NetStop("Server shutdown! \n", 0);

	//Let receive thread join
	this->receiveThread.join();
	this->serverSocket = NULL;
	this->state = NetState::CLOSED;
}



///PRIVATE FUNCTIONS:
//Start winsock
void Launcher_Netcode::StartWinsock()
{
	//Startup WinSocket
	WSADATA wsaData;
	
	this->state = NetState::STARTING;

	//Start winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Error handling.
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		this->state = NetState::CLOSED;
		return;
	}


	//Create ServerSocket for client to connect to.
	this->serverSocket = INVALID_SOCKET;

	//Create Socket object. 
	this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Error handling.
	if (this->serverSocket == INVALID_SOCKET)
	{
		printf("Failed to execute socket(). Error Code: %ld\n", WSAGetLastError());
		WSACleanup();
		this->state = NetState::CLOSED;
		return;
	}
}
//Start connection to server
void Launcher_Netcode::ConnectToServer()
{
	int iResult = 0;

	//Setup socket address structure
	SOCKADDR_IN sockAddr;
	sockAddr.sin_port = htons(DEFAULT_PORT); //Set port
	sockAddr.sin_family = AF_INET; //IPv4 Address
	inet_pton(AF_INET, SERVER_IP, &sockAddr.sin_addr.s_addr); //Convert the server IP to the sockaddr_in format and set it.


	//Connect to server.
	iResult = connect(this->serverSocket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(SOCKADDR_IN));

	//Error handling.
	if (iResult == SOCKET_ERROR)
	{
		this->serverSocket = INVALID_SOCKET;
		this->NetStop("Unable to connect to server!Error Code : % ld\n", WSAGetLastError());
		return;
	}
	//Error handling.
	if (this->serverSocket == INVALID_SOCKET)
	{
		this->NetStop("Unable to connect to server!\n", 0);
		return;
	}

	printf("Connected to server!\n");
	this->state = NetState::CONNECTED;
}


//Send current message to be send to server
void Launcher_Netcode::Send()
{
	if (this->state == NetState::CLOSED) return;

	int iResult = 0;

	//Check message type of active message
	switch (static_cast<MessageType>(this->launcher->GetActiveMessage()->type))
	{
	case MessageType::QUERY: //If Query:
	{
		//Send active message as a reinterpreted query, that gets reinterpreted as an const char*, to the server
		iResult = send(this->serverSocket, reinterpret_cast<const char*>(reinterpret_cast<Query*>(this->launcher->GetActiveMessage())), sizeof(Query), 0);
		break;
	}
	}

	//Afterwards delete the active message
	delete this->launcher->GetActiveMessage();

	//Error handling
	if (iResult == SOCKET_ERROR)
	{
		this->NetStop("send failed: %d\n", WSAGetLastError());
		return;
	}
}
//Receive incoming messages from server
void Launcher_Netcode::Receive()
{
	int iResult = 0;
	char buffer[DEFAULT_BUFFLENGTH];
	
	//While Launcher is still running
	while (this->state != NetState::CLOSING)
	{
		//Reset currentbuffer to null
		memset(buffer, 0, DEFAULT_BUFFLENGTH);
		
		//Receive message from server
		iResult = recv(this->serverSocket, buffer, DEFAULT_BUFFLENGTH, 0);

		//If succesful
		if (iResult > 0)
		{
			//Check message type
			switch (static_cast<MessageType>(reinterpret_cast<Message*>(buffer)->type))
			{
			case MessageType::QUERYRESP: //If message is a query response
			{
				this->HandleLogin(reinterpret_cast<QueryResponse*>(buffer)); //Handle login with QueryResponse recast
				break;
			}
			}
		}
		else
		{
			//Error handling
			this->NetStop("Receive failed! %d\n", WSAGetLastError());
		}
	}
}
//Teardown connection and show errorMessage/logMessage
void Launcher_Netcode::NetStop(const char* errorMessage, int errorCode)
{
	//Shutdown TCP connection in both directions
	shutdown(this->serverSocket, SD_BOTH); 
	//Print error message and error code or print log message
	printf(errorMessage, errorCode);
	//Close the server socket
	closesocket(this->serverSocket);
	//Cleanup winsock
	WSACleanup();
	//Set state to closing
	this->state = NetState::CLOSING;
}


//Handle Login message
void Launcher_Netcode::HandleLogin(QueryResponse* resp)
{
	//If the query was succesful
	if (resp->success)
	{
		this->launcher->SetLauncherState(LauncherState::IN_LOBBY);
	}
	else
	{
		printf("Failure!\n");
	}
}
