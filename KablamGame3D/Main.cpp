#include <iostream>

#include "KablamGame3D.h"

int main()
{
	std::cout << "Welcome to KablamGame3D!" << std::endl;

	KablamGame3D kablamGame3D(L"K-blam3D");
	
	// dismal res   180 x 90
	// low res		240 x 135
	// med res		320 x 180
	// med-high	res	480 x 270
	// high res		640 x 360
	// mega res		1280 x 720
	// ultra res	1920 x 1080

	kablamGame3D.BuildConsole(640, 360, 2, 2);
	
	kablamGame3D.Start();

	std::cout << "End of main() reached\n";

	return 0;
}