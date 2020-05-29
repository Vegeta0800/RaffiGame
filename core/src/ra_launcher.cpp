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
	//Set connection between window and netcode


	//Initialize other variables
	this->activeLoad = nullptr;
	this->state = LauncherState::CLOSED;
}


//Initialize netcode and window
void Launcher::Initialize()
{
	//Start netcode
	//this->StartNetcode();
	//Create and start window
	this->InstantiateWindow();

	//Start logging phase
	this->state = LauncherState::LOGGING;

	//Query* q = new Query();
	//q->name = "Raffi";
	//q->password = "Password";
	//q->type = 0;

	//this->netcode.SetMessage(q);
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
			this->window.state = WindowState::CLOSED;
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



//Set active message
void Launcher::SetActiveMessage(Message* mess)
{
	this->activeLoad = mess;
}
//Get active message
Message* Launcher::GetActiveMessage()
{
	return this->activeLoad;
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