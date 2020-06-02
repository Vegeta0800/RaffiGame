//EXTERNAL INCLUDES
#include <iostream>
//INTERNAL INCLUDES
#include "launcher\ra_launcher_window.h"
#include "ra_launcher.h"

//This pointer to use in WndProc
Launcher_Window* thisPtr;

///THREAD FUNCTIONS
//Message handling thread function
LRESULT CALLBACK LaunchWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND: //If window registers a command
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
		}
		break;
	}
		//If the message is destroy, destroy the window.
	case WM_DESTROY:
	{
		DestroyWindow(hwnd); //Destroy the window.
		thisPtr->Close();
		break;
	}
	//If the message is close,  destroy the window.
	case WM_CLOSE:
	{
		DestroyWindow(hwnd); //Destroy the window.
		thisPtr->Close();
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

	if (this->updateWindow)
	{
		this->updateWindow = false;
		this->UpdateCurrentWindow();
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

}

//Create message based on what window is currently active
void Launcher_Window::CreateMessage(std::string text)
{
	//Check what window is currently active
	switch (this->launcher->GetLauncherState())
	{
	case LauncherState::IN_LOBBY: //If in lobby, room message
	{
		this->CreateLobbyMessage(text);
		break;
	}
	case LauncherState::IN_ROOM: //If in room, chat message
	{
		this->CreateRoomMessage(text);
		break;
	}
	}
}

//Create message for lobby
void Launcher_Window::CreateLobbyMessage(std::string text)
{
	LobbyWindow lb = *reinterpret_cast<LobbyWindow*>(this->currentWindow);

	MessageWindow newMessage;
	newMessage.id = lb.messages.size();
	newMessage.handle = HWND();

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
		//Create visible room text at last possible position
		newMessage.handle = CreateWindowEx(0, "static", text.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP, 1, 1, (5 * (static_cast<int>(text.size()) + 10)), 22, lb.lobby,
			NULL, (HINSTANCE)(LONG_PTR)GetWindowLong(lb.lobby, -6), NULL);
	}

	GetWindowRect(newMessage.handle, &newMessage.rect);
	lb.messages.push_back(newMessage);
}
//Create message for room
void Launcher_Window::CreateRoomMessage(std::string text)
{

}

//Trigger UpdateCurrentWindow to occur on next handle loop
void Launcher_Window::TriggerUpdate()
{
	this->updateWindow = true;
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


//Event triggered when Login button is clicked
void Launcher_Window::LoginButtonClick()
{
	//Recast current window to login window and get local copy
	LoginWindow loginWindow = *reinterpret_cast<LoginWindow*>(this->currentWindow);
	
	//Allocate 2 char arrays and set to 0
	char name[32];
	char password[32];
	memset(name, 0, 32);
	memset(password, 0, 32);

	//Get text in name and password textfields
	GetWindowText(loginWindow.name, name, 32);
	GetWindowText(loginWindow.password, password, 32);

	//Set the name and password textfields to nothing again
	SetWindowText(loginWindow.name, "");
	SetWindowText(loginWindow.password, "");

	//Create new query message and set type and attributes
	Query* query = new Query();
	query->type = static_cast<uint8_t>(MessageType::QUERY);
	query->name = name;
	query->password = password;

	//Set launchers active message to query
	this->launcher->SetActiveMessage(query);
	//Set clients name
	this->launcher->SetName(name);
}
//Event triggered when CreateRoom button is clicked
void Launcher_Window::CreateRoomButtonClick()
{
	std::string roomName = this->launcher->GetName();
	roomName.append("'s room 1/2");
	CreateLobbyMessage(roomName);
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
//Trigger close event for main window
void Launcher_Window::Close()
{
	this->close = true;
}


//Get handle to main window
HWND Launcher_Window::GetHandle()
{
	return this->handle;
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