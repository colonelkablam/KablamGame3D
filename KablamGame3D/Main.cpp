#include <iostream>
#include <windows.h>  // For general Windows API functions

#include <mmsystem.h> // For multimedia functions

#pragma comment(lib, "winmm.lib") // Link with the Winmm library
#include "KablamGame3D.h"

int main()
{
	// will hopefully not be seen...
	std::cout << "Welcome to KablamGame3D!" << std::endl;

	KablamGame3D kablamGame3D(L"K-blam3D");
	
	// 4OD res			180 x 90
	// low res			240 x 135
	// med res			320 x 180
	// med res wide		320, 120, 4, 4, 0, 0

	// med-hi	res		480 x 270
	// med-hi res		480, 180, 3, 3, 0, 0

	// hiigh-res		640 x 360
	// mega res			1280 x 720
	// ultra res		1920 x 1080

	kablamGame3D.BuildConsole(320, 150, 4.5, 4.5, 0, 0);
	
	kablamGame3D.Start();

	std::cout << "End of main() reached\n";

	return 0;
}