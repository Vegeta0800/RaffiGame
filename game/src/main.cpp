//EXTERNAL INCLUDES
#include <iostream>
//INTERNAL INCLUDES
#include "ra_application.h"

//Start point of game.exe
int main()
{
    //Create Mutex
    //const char mutexName[] = "RaffiGame";
    //HANDLE hHandle = CreateMutex(NULL, TRUE, mutexName);

    ////Check if program is already running
    //if (ERROR_ALREADY_EXISTS == GetLastError())
    //{
    //    // Program already running somewhere
    //    return 0; // Exit program
    //}

	//Initialize application
	Application::GetInstancePtr()->Initialize();
	//Update application while not closed
	Application::GetInstancePtr()->Update();
	//Close application
	Application::GetInstancePtr()->Close();

    //ReleaseMutex(hHandle); // Explicitly release mutex
    //CloseHandle(hHandle);

	//End execution
	return 0;
}