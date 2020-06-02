
#pragma once
//EXTERNAL INCLUDES
#include <winsock.h>
#include <thread>
#include <string>
//INTERNAL INCLUDES
#include "launcher/ra_messages.h"

//Defines needed for netcode
#define DEFAULT_PORT 12307
#define DEFAULT_BUFFLENGTH 65
#define SERVER_IP "127.0.0.1"

class Launcher;


//States the netcode itself can be in
enum class NetState
{
	STARTING = 0,
	CONNECTED = 1,
	RECEIVED = 2,
	SEND = 3,
	CLOSING = 4,
	CLOSED = 5
};


//Launcher netcode class
class Launcher_Netcode
{
public:
	//Create connection to launcher
	void SetLauncher(Launcher* launcher);


	//Start winsock and connect to server
	bool SetupConnection();
	//Handle incoming messages and send replies
	bool HandleConnection();
	//Close connection down and end netcode
	void CloseConnection();


	//Public state
	NetState state;
private:
	//Start winsock
	void StartWinsock();
	//Start connection to server
	void ConnectToServer();


	//Send current message to be send to server
	void Send();
	//Receive incoming messages from server
	void Receive();
	//Teardown connection and show errorMessage/logMessage
	void NetStop(const char* errorMessage, int errorCode);


	//Handle Login
	void HandleLogin(QueryResponse* resp);
	

	std::thread receiveThread;
	
	SOCKET serverSocket;
	Launcher* launcher;
};
