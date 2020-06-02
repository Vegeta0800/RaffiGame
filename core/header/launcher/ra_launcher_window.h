
#pragma once
//EXTERNAL INCLUDES
#include <windows.h>
#include <stdint.h>
#include <vector>
#include <string>
//INTERNAL INCLUDES

//Forward declaration
class Launcher;

//States the window itself can be in
enum class WindowState
{
	INSTANTIATED = 0,
	SHOW = 1,
	HIDE = 2,
	CLOSING = 3,
	CLOSED = 4
};
enum class WindowTypes
{
	LOGIN = 0,
	LOBBY = 1,
	ROOM = 2
};

struct MessageWindow
{
	HWND handle;
	RECT rect;
	uint8_t id;
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
#define LobbyButton_ID 3
struct LobbyWindow : public Window
{
	std::vector<MessageWindow> messages;
	HWND lobby;
	HWND button;
};
#define RoomReadyButton_ID 4
#define RoomLeaveButton_ID 5
#define RoomChatButton_ID 6
#define RoomChatBox_ID 7
struct RoomWindow : public Window
{
	std::vector<MessageWindow> messages;
	HWND room;
	HWND readyButton;
	HWND leaveButton;
	HWND chatButton;
	HWND chatBox;
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


	//Create Login Window screen with Name, Password textfields and Login button
	void CreateLoginWindow();
	//Create Lobby Window screen with active rooms and create button
	void CreateLobbyWindow();
	//Create Room Window screen with chat messages, ready button, chat box, chat button and leave button.
	void CreateRoomWindow();

	//Create message based on what window is currently active
	void CreateMessage(std::string text);
	//Create message for lobby
	void CreateLobbyMessage(std::string text);
	//Create message for room
	void CreateRoomMessage(std::string text);

	//Trigger UpdateCurrentWindow to occur on next handle loop
	void TriggerUpdate();
	//Delete current window and create next window based on current launcher state
	void UpdateCurrentWindow();


	//Delete current active window
	void DeleteCurrentWindow();


	//Event triggered when Login button is clicked
	void LoginButtonClick();
	//Event triggered when CreateRoom button is clicked
	void CreateRoomButtonClick();


	//Change window state 
	void ChangeState(WindowState state);
	//Trigger close event for main window
	void Close();


	//Get handle to main window
	HWND GetHandle();

	//Public state
	WindowState state;
private:
	//Create the window
	void Create();



	uint32_t width;
	uint32_t height;
	
	bool close = false;
	bool updateWindow = false;

	Window* currentWindow;

	Launcher* launcher;
	HWND handle;
};
