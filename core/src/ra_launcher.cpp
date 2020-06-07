//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_launcher.h"



//PUBLIC FUNCTIONS: 
//On construction, construct netcode and window
Launcher::Launcher()
{
	//Set connection between window and launcher
	this->window.SetLauncher(this);
	this->netcode.SetLauncher(this);

	//Initialize other variables
	this->activeLoad = nullptr;
	this->state = LauncherState::CLOSED;
}


//Initialize netcode and window
void Launcher::Initialize()
{
	//Start netcode
	this->StartNetcode();
	//Create and start window
	this->InstantiateWindow();

	//Start logging phase
	this->state = LauncherState::LOGGING;
}
//Update netcode and window. Return true if everything was succesful
bool Launcher::Update()
{
	//Close down launcher if window or netcode fails
	if (this->state == LauncherState::CLOSING)
	{
		printf("Launcher is closing! \n");
		this->Close();
		return false;
	}


	//If window isnt closed
	if (this->window.state != WindowState::CLOSED)
	{
		//Handle window messages
		if (!this->window.HandleWindow())
		{
			//If fails, shutdown launcher.
			printf("Launcher window couldnt be handled!\n");
			this->state = LauncherState::CLOSING;
			this->window.state = WindowState::CLOSING;
		}
	}
	//If netcode isnt closed
	if (this->netcode.state != NetState::CLOSED)
	{
		//Handle connection to server
		if (!this->netcode.HandleConnection())
		{
			//If fails, shutdown launcher.
			printf("Launcher netcode couldnt handle the connection!\n");
			this->state = LauncherState::CLOSING;
			this->netcode.state = NetState::CLOSING;
		}
	}

	return true;
}
//Cleanup netcode and window
void Launcher::Close()
{
	//Return if already closed
	if (this->state == LauncherState::CLOSED) return;

	//If netcode isnt closed yet, close netcode
	if (this->netcode.state != NetState::CLOSED)
		this->netcode.CloseConnection();
	//If window isnt closed yet, close window
	if (this->window.state != WindowState::CLOSED)
		this->window.CloseWindow();

	//Launcher is closed
	printf("Launcher closed!\n");
	this->state = LauncherState::CLOSED;
}


//Spawn new physical message
void Launcher::SpawnMessage(std::string text, uint8_t senderID)
{
	//Trigger event in window to create new message on screen
	this->window.TriggerUpdateMessages(text, senderID);
}


//Set active message to be send over network
void Launcher::SetActiveMessage(Message* mess)
{
	this->activeLoad = mess;
}
//Get active message to be send over network
Message* Launcher::GetActiveMessage()
{
	return this->activeLoad;
}

//Set current Launcher state and trigger update in window
void Launcher::SetLauncherState(LauncherState launcherState)
{
	this->state = launcherState;
	//Trigger event in window to change window
	this->window.TriggerUpdateWindow();
}
//Get current Launcher state
LauncherState Launcher::GetLauncherState()
{
	return this->state;
}

//Set the clients name
void Launcher::SetName(std::string name)
{
	this->name = name;
}
//Get the clients name
std::string Launcher::GetName()
{
	return this->name;
}

//Set the name of the room the client is inside. If hosting, its the clients name, if not its the hosting clients name
void Launcher::SetRoomName(std::string hostName)
{
	this->roomName = hostName.append("'s room");
}
//Get the name of the room the client is inside
std::string Launcher::GetRoomName()
{
	return this->roomName;
}

//Return if client is hosting a room right now
bool& Launcher::isHostingRoom()
{
	return this->hostingRoom;
}



///PRIVATE FUNCTIONS:
//Start winsock and build connection to server
void Launcher::StartNetcode()
{
	//Setup connection to server, if fails close launcher
	if (!this->netcode.SetupConnection())
		this->state = LauncherState::CLOSING;
}
//Instantiate window and show it
void Launcher::InstantiateWindow()
{
	//Instantiate window, if fails close launcher
	if (!this->window.Instantiate())
		this->state = LauncherState::CLOSING;
	//Else show window
	else
		this->window.ChangeState(WindowState::SHOW);
}