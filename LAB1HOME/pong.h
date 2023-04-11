#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "resource.h"
#define WIDTH(rc) ((rc).right - (rc).left)
#define HEIGHT(rc) ((rc).bottom - (rc).top)
 
class pong
{
private:
	bool register_class();//registers class

	static std::wstring const s_class_name;  // static member function that serves as the window procedure for the application.
	static LRESULT CALLBACK window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); //recieves messages and stores them
	HWND create_window(DWORD style, HWND parent = nullptr, DWORD ex_style = 0); // extension for transparancy

	HINSTANCE m_instance;
	HWND m_main;
	HWND m_popup;
	HWND window;

	////trail
	void Draw(HDC hdc);
	
	
	int Bx;
	int By;
	int Brad;

	//paddle height and width
	int pw ;
	int ph ;

	//counters
	int lscore=0;
	int rscore=0;

	int speedx;
	int speedy;
	
	void ball_paddle_collision();
	void move_paddle(int y);
	void update_transparency();

	COLORREF background = RGB(144, 238, 144);
	TCHAR szFile[260];
	void bitmap(HWND hwnd, LPCWSTR filename);
	

	HBITMAP  hBitmap;
	bool isBitmap = false;


public:

	pong(HINSTANCE instance);
	int run(int show_command);
};


