//EXTERNAL INCLUDES
#include <iostream>
//INTERNAL INCLUDES
#include "launcher\ra_launcher_window.h"
#include "ra_launcher.h"
#include "ra_utils.h"

//This pointer to use in WndProc
Launcher_Window* thisPtr;

///THREAD FUNCTIONS
//Message handling thread function
LRESULT CALLBACK LaunchWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	//If window registers a command
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) //Convert command
		{
		case LoginButton_ID: //If its the login button being pressed.
		{
			thisPtr->LoginButtonClick(); //Trigger event in launcher window
			break;
		}
		case LobbyButton_ID: //If its the create room button being pressed.
		{
			thisPtr->CreateRoomButtonClick(); //Trigger event in launcher window
			break;
		}
		case RoomChatButton_ID: //If send button is being pressed.
		{
			thisPtr->SendButtonClick(); //Trigger event in launcher window
		}
		}
		break;
	}
	//If left mouse double clicks
	case WM_LBUTTONDBLCLK:
	{
		//If the current launcher window is the lobby
		if (thisPtr->GetCurrentWindow()->type == static_cast<uint8_t>(WindowTypes::LOBBY))
		{
			//Get the lobby window by reinterpreting the current window
			LobbyWindow* lobby = reinterpret_cast<LobbyWindow*>(thisPtr->GetCurrentWindow());

			if (lobby->messages.size() == 0) break;

			SetFocus(hwnd);
			POINT pt;

			//Get current cursor position
			GetCursorPos(&pt);

			//Check if the cursor position is inside a lobby message
			for (int i = 0; i < lobby->messages.size(); i++)
			{
				if (PtInRect(&lobby->messages[i].rect, pt))
				{
					//Trigger event in launcher window and try to join the room
					thisPtr->JoinRoomClick(lobby->messages[i].clientID);
				}
			}
		}
		break;
	}
	//If the message is destroy, destroy the window.
	case WM_DESTROY:
	{
		DestroyWindow(hwnd); //Destroy the window.
		thisPtr->Close(); //Close
		break;
	}
	//If the message is close,  destroy the window.
	case WM_CLOSE:
	{
		DestroyWindow(hwnd); //Destroy the window.
		thisPtr->Close(); //Close
		break;
	}
	default:
		//Go on.
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}



///PUBLIC FUNCTIONS:
//Create connection to launcher
void Launcher_Window::SetLauncher(Launcher* launcher)
{
	this->launcher = launcher;
	this->state = WindowState::CLOSED;
}


//Instantiate window
bool Launcher_Window::Instantiate()
{
	//If not instantiated yet
	if (this->state == WindowState::INSTANTIATED) return false;

	thisPtr = this;

	//Create window
	this->Create();

	this->CreateLoginWindow();

	//See if create was succesful by checking if window is now instantiated
	return (this->state == WindowState::INSTANTIATED);
}
//Handle window messages
bool Launcher_Window::HandleWindow()
{
	//Handle window messages if window is visible
	if (this->state == WindowState::HIDE) return true;

	//Event handling for updating the window
	if (this->updateWindow)
	{
		this->updateWindow = false;
		this->UpdateCurrentWindow();
	}

	//Event handling for adding a message
	if (this->currentMessage != "")
	{
		this->CreateMessage();
		this->currentMessage = "";
	}

	//If window events occur
	MSG msg = { };
	if (PeekMessageA(&msg, this->handle, 0, 0, PM_REMOVE))
	{
		//Translate and handle them
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//Window is closed cleanup launcher
		if (this->close) return false;
	}

	//Update the main launcher window
	UpdateWindow(this->handle);
	return true;
}
//Close window
void Launcher_Window::CloseWindow()
{
	//Get rid of launcher connection
	this->launcher = nullptr;

	//Destroy the main window
	DestroyWindow(this->handle);
	this->handle = NULL;
}
//Trigger close event for main window
void Launcher_Window::Close()
{
	this->close = true;
}


//Create message based on what window is currently active
void Launcher_Window::CreateMessage()
{
	//Check what window is currently active
	switch (this->launcher->GetLauncherState())
	{
	case LauncherState::IN_LOBBY: //If in lobby, room message
	{
		this->CreateLobbyMessage(this->currentMessage, this->currentMessageID);
		break;
	}
	case LauncherState::IN_ROOM: //If in room, chat message
	{
		this->CreateRoomMessage(this->currentMessage);
		break;
	}
	}
}
//Trigger UpdateCurrentWindow to occur on next handle loop
void Launcher_Window::TriggerUpdateWindow()
{
	this->updateWindow = true;
}
//Trigger CreateMessage to occur on next handle loop
void Launcher_Window::TriggerUpdateMessages(std::string message, uint8_t id)
{
	this->currentMessage = message;
	this->currentMessageID = id;
}


//Event triggered when Login button is clicked
void Launcher_Window::LoginButtonClick()
{
	//Recast current window to login window and get local copy
	LoginWindow loginWindow = *reinterpret_cast<LoginWindow*>(this->currentWindow);
	
	//Allocate 2 char arrays and set to 0
	char name[16];
	char password[16];
	memset(name, 0, 16);
	memset(password, 0, 16);

	//Get text in name and password textfields
	GetWindowText(loginWindow.name, name, 16);
	GetWindowText(loginWindow.password, password, 16);

	//Set the name and password textfields to nothing again
	SetWindowText(loginWindow.name, "");
	SetWindowText(loginWindow.password, "");

	//Create new query message and set type and attributes
	Query* query = new Query();
	query->type = static_cast<uint8_t>(MessageType::QUERY);

	//Set the name and password inside query
	SET_STRING(query->name, name, 16);
	SET_STRING(query->password, password, 16);

	//Set launchers active message to query
	this->launcher->SetActiveMessage(query);
	//Set clients name
	this->launcher->SetName(name);
}
//Event triggered when CreateRoom button is clicked
void Launcher_Window::CreateRoomButtonClick()
{
	//Create normal message and set type, because nothing else is needed for creating a room
	Message* createRoomMessage = new Message();
	createRoomMessage->type = static_cast<uint8_t>(MessageType::CREATEROOM);

	//Set message, set the room name and set hosting bool
	this->launcher->SetActiveMessage(createRoomMessage);
	this->launcher->SetRoomName(this->launcher->GetName());
	this->launcher->isHostingRoom() = true;

	//Change state and window to inside the room
	this->launcher->SetLauncherState(LauncherState::IN_ROOM);
}
//Event triggered when a client double clicks a room
void Launcher_Window::JoinRoomClick(uint8_t roomHostID)
{
	//Create join room message with host id and send it
	JoinRoomMessage* joinRoomMessage = new JoinRoomMessage();
	joinRoomMessage->hostID = roomHostID;
	joinRoomMessage->type = static_cast<uint8_t>(MessageType::JOINROOM);

	this->launcher->SetActiveMessage(joinRoomMessage);
}
//Event triggered when Send button is clicked
void Launcher_Window::SendButtonClick()
{
	//Get current room window by reinterpreting current window
	RoomWindow roomWindow = *reinterpret_cast<RoomWindow*>(this->currentWindow);

	//Start message with "name: "
	std::string mess = this->launcher->GetName();
	mess.append(": ");

	//Get message from the chatbox. 46 because max is 64 (16 max name plus 2 of ": ")
	char message[46];
	memset(message, 0, 46);
	GetWindowText(roomWindow.chatBox, message, 46);

	//Append message
	mess.append(message);

	//Create new chatmessage and send it with the message 
	ChatMessage* chatMessage = new ChatMessage();
	chatMessage->type = static_cast<uint8_t>(MessageType::CHATMESS);

	SET_STRING(chatMessage->chatMessage, mess, mess.length());

	this->launcher->SetActiveMessage(chatMessage);
	
	//Create message on own screen
	this->CreateRoomMessage(mess);
}


//Change window state 
void Launcher_Window::ChangeState(WindowState state)
{
	//Show window
	if (state == WindowState::SHOW)
	{
		ShowWindow(this->handle, SW_SHOW);
		UpdateWindow(this->handle);
		this->state = state;
	}
	//Hide window
	else if (state == WindowState::HIDE)
	{
		ShowWindow(this->handle, SW_HIDE);
		UpdateWindow(this->handle);
		this->state = state;
	}
}


//Get handle to main window
HWND Launcher_Window::GetHandle()
{
	return this->handle;
}
//Get pointer to current window
Window* Launcher_Window::GetCurrentWindow()
{
	return this->currentWindow;
}



///PRIVATE FUNCTIONS:
//Create the window
void Launcher_Window::Create()
{
	//Initialize and set variables
	DWORD style = 0;
	LPCSTR title = "Raffi Launcher";
	this->width = 455;
	this->height = 555;


	//Create a struct to hold window data and fill that struct with data
	WNDCLASSEXA wndex = { 0 };
	wndex.cbSize = sizeof(WNDCLASSEX);
	wndex.lpfnWndProc = LaunchWndProc; //Set this function as the function to handle the messages on another thread
	wndex.hInstance = GetModuleHandle(NULL);
	wndex.hIcon = NULL;
	wndex.hCursor = LoadCursor(NULL, IDC_ARROW); //Cursor
	wndex.hbrBackground = CreateSolidBrush(RGB(255, 210, 0)); //Yellow
	wndex.lpszClassName = title;
	wndex.style = CS_DBLCLKS; //Activate double clicks
	
	//Register a window and if that fails crash the program
	if (!RegisterClassExA(&wndex))
		throw;


	//Set extra window styles
	style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;


	//Fill the RECT struct with data
	RECT adjustedRect = RECT
	{
		static_cast<LONG>(0),
		static_cast<LONG>(0),
		static_cast<LONG>(this->width),
		static_cast<LONG>(this->height)
	};

	//Adjust the Window. If that fails crash the program
	if (!AdjustWindowRect(&adjustedRect, style, FALSE))
		throw;


	//Get the selected display
	int displayWidth = GetSystemMetrics(SM_CXSCREEN);
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);

	//Get the center position of the display
	int posX = ((displayWidth - this->width) / 2);
	int posY = ((displayHeight - this->height) / 2);


	//Create a handle onto a newly created window
	this->handle = CreateWindowExA
	(
		NULL,
		title,
		title,
		style,
		posX,
		posY,
		this->width,
		this->height,
		NULL,
		NULL,
		GetModuleHandle(NULL), NULL
	);

	//If that fails crash the program
	if (this->handle == NULL)
		throw;

	//Set state to instantiated
	this->state = WindowState::INSTANTIATED;
}

//Delete current window and create next window based on current launcher state
void Launcher_Window::UpdateCurrentWindow()
{
	DeleteCurrentWindow();

	//Check current state of Launcher
	switch (this->launcher->GetLauncherState())
	{
	case LauncherState::IN_LOBBY: //If lobby
	{
		//Create lobby window
		this->CreateLobbyWindow();
		break;
	}
	case LauncherState::IN_ROOM: //If room
	{
		//Create room window
		this->CreateRoomWindow();
		break;
	}
	}
}
//Delete current window based on type
void Launcher_Window::DeleteCurrentWindow()
{
	//Check type of window
	switch (static_cast<WindowTypes>(this->currentWindow->type))
	{
	case WindowTypes::LOGIN: //If its the login screen
	{
		//Get local copy
		LoginWindow* lw = reinterpret_cast<LoginWindow*>(this->currentWindow);

		//Destroy windows.
		DestroyWindow(lw->name);
		DestroyWindow(lw->password);
		DestroyWindow(lw->button);
		
		UpdateWindow(this->handle);

		//Delete current window
		delete this->currentWindow;
		this->currentWindow = nullptr;
		break;
	}
	case WindowTypes::LOBBY:
	{
		//Get local copy
		LobbyWindow* lw = reinterpret_cast<LobbyWindow*>(this->currentWindow);

		//Destroy windows.
		DestroyWindow(lw->button);
		DestroyWindow(lw->lobby);

		//Destroy all messages
		for (MessageWindow window : lw->messages)
		{
			DestroyWindow(window.handle);
		}

		UpdateWindow(this->handle);

		//Delete current window
		delete this->currentWindow;
		this->currentWindow = nullptr;
		break;
	}
	case WindowTypes::ROOM:
	{
		//Get local copy
		RoomWindow* rw = reinterpret_cast<RoomWindow*>(this->currentWindow);

		//Destroy windows.
		DestroyWindow(rw->chatBox);
		DestroyWindow(rw->room);
		DestroyWindow(rw->readyButton);
		DestroyWindow(rw->leaveButton);
		DestroyWindow(rw->chatButton);

		//Destroy all messages
		for (MessageWindow window : rw->messages)
		{
			DestroyWindow(window.handle);
		}

		UpdateWindow(this->handle);

		//Delete current window
		delete this->currentWindow;
		this->currentWindow = nullptr;
		break;
	}
	}
}

//Create Login Window Screen with Name, Password textfields and Login button
void Launcher_Window::CreateLoginWindow()
{
	//Create the LoginButton
	LoginWindow* loginWindow = new LoginWindow(); 
	loginWindow->button = HWND();
	loginWindow->button = CreateWindowEx(0, "BUTTON", "Login",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
		(HMENU)LoginButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create the name textfield
	loginWindow->name = HWND();
	loginWindow->name = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN,
		10, 420, 300, 25, this->handle, (HMENU)LoginName_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//Create the password textfield
	loginWindow->password = HWND();
	loginWindow->password = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_PASSWORD,
		10, 480, 300, 25, this->handle, (HMENU)LoginPassword_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	UpdateWindow(this->handle);

	loginWindow->type = static_cast<uint8_t>(WindowTypes::LOGIN);
	this->currentWindow = loginWindow;
}
//Create Lobby Window screen with active rooms and create button
void Launcher_Window::CreateLobbyWindow()
{
	LobbyWindow* lobbyWindow = new LobbyWindow();

	//Create Room button
	lobbyWindow->button = HWND();
	lobbyWindow->button = CreateWindowEx(0, "BUTTON", "Create Room",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
		(HMENU)LobbyButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create the lobby background.
	lobbyWindow->lobby = HWND();
	lobbyWindow->lobby = CreateWindowEx(0, "static", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, 50, 50, 340, 400, this->handle,
		NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	UpdateWindow(this->handle);

	lobbyWindow->type = static_cast<uint8_t>(WindowTypes::LOBBY);
	this->currentWindow = lobbyWindow;
}
//Create Room Window screen with chat messages, ready button, chat box, chat button and leave button.
void Launcher_Window::CreateRoomWindow()
{
	RoomWindow* roomWindow = new RoomWindow();

	//Create Send button
	roomWindow->chatButton = HWND();
	roomWindow->chatButton = CreateWindowEx(0, "BUTTON", "Send",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
		(HMENU)RoomChatButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create Leave button
	roomWindow->leaveButton = HWND();
	roomWindow->leaveButton = CreateWindowEx(0, "BUTTON", "Leave",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 15, 120, 25, this->handle,
		(HMENU)RoomLeaveButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create Ready button
	roomWindow->readyButton = HWND();
	roomWindow->readyButton = CreateWindowEx(0, "BUTTON", "Ready",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 450, 120, 25, this->handle,
		(HMENU)RoomReadyButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create chat box
	roomWindow->chatBox = HWND();
	roomWindow->chatBox = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN,
		10, 480, 300, 25, this->handle, (HMENU)RoomChatBox_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//Create the room background.
	roomWindow->room = HWND();
	roomWindow->room = CreateWindowEx(0, "static", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, 45, 45, 340, 400, this->handle,
		NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Set windows text to the name plus "'s room"
	SetWindowText(this->handle, this->launcher->GetRoomName().c_str());

	//Create a join message saying "name: joined the room!"
	std::string joinMessage = this->launcher->GetName();
	joinMessage.append(": joined the room!");

	//If client is joining
	if (!this->launcher->isHostingRoom())
	{
		//Send join message as chatmessage to host
		ChatMessage* chatMessage = new ChatMessage();
		chatMessage->type = static_cast<uint8_t>(MessageType::CHATMESS);
		
		SET_STRING(chatMessage->chatMessage, joinMessage, joinMessage.length());
		
		this->launcher->SetActiveMessage(chatMessage);
	}

	//Change current window
	roomWindow->type = static_cast<uint8_t>(WindowTypes::ROOM);
	this->currentWindow = roomWindow;

	//Create join message on own screen
	this->CreateRoomMessage(joinMessage);

	UpdateWindow(this->handle);
}

//Create message for lobby
void Launcher_Window::CreateLobbyMessage(std::string text, uint8_t id)
{
	LobbyWindow lb = *reinterpret_cast<LobbyWindow*>(this->currentWindow);

	//Create new message
	MessageWindow newMessage;
	newMessage.id = static_cast<uint8_t>(lb.messages.size());
	newMessage.handle = HWND();
	newMessage.clientID = id;

	//If there are already existing messages
	if (lb.messages.size() > 0)
	{
		RECT rect;

		//Get position of the lowest visible room.
		GetWindowRect(lb.messages[lb.messages.size() - 1].handle, &rect);
		MapWindowPoints(HWND_DESKTOP, lb.lobby, (LPPOINT)&rect, 2); //Map to lobby frame

		//If spawned visible room would be under the screen
		if (rect.top + 22 > 380)
		{
			for (MessageWindow room : lb.messages)
			{
				//Adjust positon to 22 units up
				SetWindowPos(room.handle, 0, rect.left, rect.top - 22, rect.right, rect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
				//Update rect of room
				GetWindowRect(room.handle, &room.rect);
			}

			//Create visible room text at last possible position
			newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, rect.top, (5 * (static_cast<int>(text.size()) + 10)), 22, lb.lobby,
				NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(lb.lobby, -6), NULL);

			return;
		}
		else //If not
		{
			//Create visible room text 22 units under the visible room message
			newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, rect.top + 22, (5 * (static_cast<int>(text.size()) + 10)), 22, lb.lobby,
				NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(lb.lobby, -6), NULL);
		}
	}
	else
	{
		//Create visible room text at first possible position
		newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, 1, (5 * (static_cast<int>(text.size()) + 10)), 22, lb.lobby,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(lb.lobby, -6), NULL);
	}

	GetWindowRect(newMessage.handle, &newMessage.rect);
	reinterpret_cast<LobbyWindow*>(this->currentWindow)->messages.push_back(newMessage);
}
//Create message for room
void Launcher_Window::CreateRoomMessage(std::string text)
{
	RoomWindow roomWindow = *reinterpret_cast<RoomWindow*>(this->currentWindow);

	//Create new message
	MessageWindow newMessage;
	newMessage.id = static_cast<uint8_t>(roomWindow.messages.size());
	newMessage.handle = HWND();

	//If there are already existing messages
	if (roomWindow.messages.size() > 0)
	{
		RECT rect;

		//Get position of the lowest visible message.
		GetWindowRect(roomWindow.messages[roomWindow.messages.size() - 1].handle, &rect);
		MapWindowPoints(HWND_DESKTOP, roomWindow.room, (LPPOINT)&rect, 2); //Map to room frame

		//If spawned visible message would be under the screen
		if (rect.top + 22 > 380)
		{
			for (MessageWindow mess : roomWindow.messages)
			{
				//Adjust positon to 22 units up
				SetWindowPos(mess.handle, 0, rect.left, rect.top - 22, rect.right, rect.bottom, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
				//Update rect of message
				GetWindowRect(mess.handle, &mess.rect);
			}

			//Create visible message text at last possible position
			newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, rect.top, (5 * (static_cast<int>(text.size()) + 10)), 22, roomWindow.room,
				NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(roomWindow.room, -6), NULL);

			return;
		}
		else //If not
		{
			//Create visible message text 22 units under the visible chat message
			newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, rect.top + 22, (5 * (static_cast<int>(text.size()) + 10)), 22, roomWindow.room,
				NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(roomWindow.room, -6), NULL);
		}
	}
	else
	{
		//Create visible message text at first possible position
		newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, 1, (5 * (static_cast<int>(text.size()) + 10)), 22, roomWindow.room,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(roomWindow.room, -6), NULL);
	}

	GetWindowRect(newMessage.handle, &newMessage.rect);
	reinterpret_cast<RoomWindow*>(this->currentWindow)->messages.push_back(newMessage);
}