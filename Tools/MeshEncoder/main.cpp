#include "MeshEncoder.h"

#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
	MeshEncoder encoder;

	return encoder.ProcessCommands();
}
