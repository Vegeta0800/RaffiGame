
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_utils.h"

//Forward declarations
class Game;
class Launcher;

//States that the application itself can be in
enum class ApplicationState
{
	STARTING = 0,
	LAUNCHER = 1,
	GAME = 2,
	CLOSING = 3
};

//Singleton class Application
class Application
{
	//Forbid construction, static instance
	DEFINE_SINGLETON(Application)
public:
	//Initializing launcher and game.
	void Initialize();
	//Updating launcher or game depending on state.
	void Update();
	//Close and cleanup launcher and game.
	void Close();


	//Get reference to current state.
	ApplicationState& GetState();
private:
	Game* game;
	Launcher* launcher;

	ApplicationState state;
};	
