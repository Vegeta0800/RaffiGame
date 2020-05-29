//EXTERNAL INCLUDES
#include <iostream>
//INTERNAL INCLUDES
#include "launcher\ra_launcher_window.h"

bool close = false;

///THREAD FUNCTIONS
//Message handling thread function
LRESULT CALLBACK LaunchWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//If the message is destroy, destroy the window.
	case WM_DESTROY:
	{
		DestroyWindow(hwnd); //Destroy the window.
		close = true;
		break;
	}
	//If the message is close,  destroy the window.
	case WM_CLOSE:
	{
		DestroyWindow(hwnd); //Destroy the window.
		close = true;
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

	//Create window
	this->Create();

	//See if create was succesful by checking if window is now instantiated
	return (this->state == WindowState::INSTANTIATED);
}
//Handle window messages
bool Launcher_Window::HandleWindow()
{
	//Handle window messages if window is visible
	if (this->state == WindowState::HIDE) return true;





	

	//If window events occur
	MSG msg = { };
	if (PeekMessageA(&msg, this->handle, 0, 0, PM_REMOVE))
	{
		msg.lParam = reinterpret_cast<LPARAM>(this);
		//Translate and handle them
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//Window is closed cleanup launcher
		if (close) return false;
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


void Launcher_Window::CreateLoginWindow()
{
	//Create the LoginButton
	LoginWindow loginWindow; 
	loginWindow.button = HWND();
	loginWindow.button = CreateWindowEx(0, "BUTTON", "Login",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 315, 480, 120, 25, this->handle,
		(HMENU)LoginButton_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), NULL);

	//Create the name box.
	loginWindow.name = HWND();
	loginWindow.name = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN,
		10, 420, 300, 25, this->handle, (HMENU)LoginName_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	//Create the password box.
	loginWindow.password = HWND();
	loginWindow.password = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_PASSWORD,
		10, 480, 300, 25, this->handle, (HMENU)LoginPassword_ID, (HINSTANCE)(LONG_PTR)GetWindowLong(this->handle, -6), 0);

	UpdateWindow(this->handle);

	this->currentWindow = loginWindow;
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



///PRIVATE FUNCTIONS:
//Create the window
void Launcher_Window::Create()
{
	//Initialize and set variables
	DWORD style = 0;
	LPCSTR title = "Raffi Launcher";
	this->width = 800;
	this->height = 600;


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