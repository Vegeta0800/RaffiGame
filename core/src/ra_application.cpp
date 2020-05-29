//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_application.h"
#include "ra_launcher.h"
#include "ra_game.h"

//Declare singleton
DECLARE_SINGLETON(Application)



///PUBLIC FUNCTIONS:
//Initializing launcher and game.
void Application::Initialize()
{
	//Create new launcher and set it
	this->launcher = new Launcher;
	//Create new game and set it
	this->game = nullptr;

	//Initialize launcher
	this->launcher->Initialize();

	//Start application in launcher
	this->state = ApplicationState::LAUNCHER;
}
//Updating launcher or game depending on state.
void Application::Update()
{
	//While application is still open
	while (this->state != ApplicationState::CLOSING)
	{
		//If launcher is active
		if (this->state == ApplicationState::LAUNCHER)
		{
			//Update launcher
			if (!this->launcher->Update())
			{
				//If launcher fails, fail application
				this->state = ApplicationState::CLOSING;
			}
		}
		//If game is active
		else if (this->state == ApplicationState::GAME)
		{
			//Update game
		}
	}
}
//Close and cleanup launcher and game.
void Application::Close()
{
	//Cleanup launcher
	this->launcher->Close();
	//Delete launcher
	delete launcher;
}


//Get reference to current state.
ApplicationState& Application::GetState()
{
	return this->state;
}