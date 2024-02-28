#include <iostream>

#include "KablamGame3D.h"

int main()
{
	std::cout << "Welcome to KablamGame3D!" << std::endl;

	KablamGame3D kablamGame3D(L"K-blam3D");
	
	kablamGame3D.BuildConsole(320, 200, 3, 3);

	kablamGame3D.Start();

	std::cout << "End of main() reached\n";

	return 0;
}