//EXTERNAL INCLUDES
#include <iostream>
//INTERNAL INCLUDES
#include "ra_application.h"

//Start point of game.exe
int main()
{
	//Initialize application
	Application::GetInstancePtr()->Initialize();
	//Update application while not closed
	Application::GetInstancePtr()->Update();
	//Close application
	Application::GetInstancePtr()->Close();

	//End execution
	return 0;
}