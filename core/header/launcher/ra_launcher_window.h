
#pragma once
//EXTERNAL INCLUDES
#include <windows.h>
#include <stdint.h>
//INTERNAL INCLUDES

//Forward declaration
class Launcher;

//States the window itself can be in
enum class WindowState
{
	INSTANTIATED = 0,
	SHOW = 1,
	HIDE = 2,
	CLOSED = 3
};


struct Window
{
	uint8_t type;
};

#define LoginButton_ID 0
#define LoginName_ID 1
#define LoginPassword_ID 2
struct LoginWindow : public Window
{
	HWND name;
	HWND password;
	HWND button;
};


//Launcher window class
class Launcher_Window
{
public:
	//Create connection to launcher
	void SetLauncher(Launcher* launcher);


	//Instantiate window
	bool Instantiate();
	//Handle window messages
	bool HandleWindow();
	//Close window
	void CloseWindow();


	void CreateLoginWindow();
	void CreateRoomWindow();
	void CreateRoom();


	//Change window state 
	void ChangeState(WindowState state);


	//Get handle to main window
	HWND GetHandle();


	//Public state
	WindowState state;
private:
	//Create the window
	void Create();


	uint32_t width;
	uint32_t height;
	
	Window currentWindow;

	Launcher* launcher;
	HWND handle;
};
