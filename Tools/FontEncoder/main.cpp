#include "FontEncoder.h"

#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
	FontEncoder encoder;
	
	return encoder.ProcessCommands();
}
