#include <iostream>

#include "KablamGame3D.h"

int main()
{
	std::cout << "Welcome to KablamGame3D!" << std::endl;

	KablamGame3D kablamGame3D(L"K-blam3D");
	
	// very low 180 x 90
	// low res 240 x 135
	// med res 320 x 180
	// very med 480 x 270
	// high res 640 x 360
	// ultra 1280 x 720

	kablamGame3D.BuildConsole(320, 180, 3, 3);
	
	kablamGame3D.Start();

	std::cout << "End of main() reached\n";

	return 0;
}