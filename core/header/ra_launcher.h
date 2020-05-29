
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "launcher/ra_launcher_netcode.h"
#include "launcher/ra_launcher_window.h"
#include "launcher/ra_messages.h"

//States launcher can be in
enum class LauncherState
{
	LOGGING = 0,
	PENDING = 1,
	IN_ROOM = 2,
	HOST_ROOM = 3,
	CLOSING = 4,
	CLOSED = 5
};

//Launcher class
class Launcher
{

public:
	//On construction, construct netcode and window
	Launcher();


	//Initialize netcode and window
	void Initialize();
	//Update netcode and window. Return true if everything was succesful
	bool Update();
	//Cleanup netcode and window
	void Close();
	

	//Set active message
	void SetActiveMessage(Message* message);
	//Get active message
	Message* GetActiveMessage();
private:
	//Start winsock and build connection to server
	void StartNetcode();
	//Instantiate window and show it
	void InstantiateWindow();


	bool newMessage = false;

	Message* activeLoad;

	Launcher_Netcode netcode;
	Launcher_Window window;
	LauncherState state;
};
