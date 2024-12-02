#pragma once
#include <Windows.h>
#include <string>

class Window {

public:
	HWND hwnd;
	HINSTANCE hinstance;

	std::string name;
	int window_width;
	int window_height;
	int window_x;
	int window_y;
	int window_dx;
	int window_dy;
	int width;
	int height;

	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];


	void updateMouse(int x, int y);
	void init(std::string, int, int,int,int);
	void processMessages();
	


};



