#define NOMINMAX
#include <windows.h>
#include "pong.h"
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE /*prevInstance*/, LPWSTR /*command_line*/,
	int show_command)
{
	pong app{ instance };
	return app.run(show_command);
}