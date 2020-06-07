
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
	IN_LOBBY = 1,
	IN_ROOM = 2,
	CLOSING = 3,
	CLOSED = 4
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
	

	//Spawn new physical message
	void SpawnMessage(std::string text, uint8_t senderID = 255);


	//Set active message to be send over network
	void SetActiveMessage(Message* message);
	//Get active message to be send over network
	Message* GetActiveMessage();

	//Set current Launcher state and trigger update in window
	void SetLauncherState(LauncherState launcherState);
	//Get current Launcher state
	LauncherState GetLauncherState();

	//Set the clients name
	void SetName(std::string name);
	//Get the clients name
	std::string GetName();

	//Set the name of the room the client is inside. If hosting its the clients name, if not its the hosting clients name
	void SetRoomName(std::string hostName);
	//Get the name of the room the client is inside
	std::string GetRoomName();

	//Return if client is hosting a room right now
	bool& isHostingRoom();
private:
	//Start winsock and build connection to server
	void StartNetcode();
	//Instantiate window and show it
	void InstantiateWindow();

	bool hostingRoom = false;
	
	Message* activeLoad = nullptr;

	std::string name = "";
	std::string roomName = "";
	
	Launcher_Netcode netcode;
	Launcher_Window window;
	LauncherState state;
};
