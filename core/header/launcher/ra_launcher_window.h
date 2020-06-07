
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
	HWND handle = NULL;
	RECT rect = RECT();
	uint8_t id = 255;
	uint8_t clientID = 255;
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
	HWND name = NULL;
	HWND password = NULL;
	HWND button = NULL;
};
#define LobbyButton_ID 3
struct LobbyWindow : public Window
{
	std::vector<MessageWindow> messages;
	HWND lobby = NULL;
	HWND button = NULL;
};
#define RoomReadyButton_ID 4
#define RoomLeaveButton_ID 5
#define RoomChatButton_ID 6
#define RoomChatBox_ID 7
struct RoomWindow : public Window
{
	std::vector<MessageWindow> messages;
	HWND room = NULL;
	HWND readyButton = NULL;
	HWND leaveButton = NULL;
	HWND chatButton = NULL;
	HWND chatBox = NULL;
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
	//Trigger close event for main window
	void Close();


	//Create message based on what window is currently active
	void CreateMessage();
	//Trigger UpdateCurrentWindow to occur on next handle loop
	void TriggerUpdateWindow();
	//Trigger CreateMessage to occur on next handle loop
	void TriggerUpdateMessages(std::string message, uint8_t id);


	//Event triggered when Login button is clicked
	void LoginButtonClick();
	//Event triggered when CreateRoom button is clicked
	void CreateRoomButtonClick();
	//Event triggered when a client double clicks a room
	void JoinRoomClick(uint8_t roomHostID);
	//Event triggered when Send button is clicked
	void SendButtonClick();

	//Change window state 
	void ChangeState(WindowState state);


	//Get handle to main window
	HWND GetHandle();
	//Get pointer to current window
	Window* GetCurrentWindow();


	//Public state
	WindowState state = WindowState::CLOSED;
private:
	//Create the window
	void Create();


	//Delete current window and create next window based on current launcher state
	void UpdateCurrentWindow();
	//Delete current active window
	void DeleteCurrentWindow();


	//Create Login Window screen with Name, Password textfields and Login button
	void CreateLoginWindow();
	//Create Lobby Window screen with active rooms and create button
	void CreateLobbyWindow();
	//Create Room Window screen with chat messages, ready button, chat box, chat button and leave button.
	void CreateRoomWindow();


	//Create message for lobby
	void CreateLobbyMessage(std::string text, uint8_t id);
	//Create message for room
	void CreateRoomMessage(std::string text);


	std::string currentMessage = "";

	uint32_t width = 0;
	uint32_t height = 0;

	uint8_t currentMessageID = 255;
	
	bool close = false;
	bool updateWindow = false;

	Window* currentWindow = nullptr;

	Launcher* launcher = nullptr;
	HWND handle = NULL;
};
