#include <iostream>

#include "TexturePainter.h"


int main()
{
	std::cout << "Welcome to Texture Painter!" << std::endl;

	TexturePainter texturePainter(L"Texture Painter!");

	// very low 180 x 90
	// low res 240 x 135
	// med res 320 x 180
	// very med 480 x 270
	// high res 640 x 360
	// ultra 1280 x 720

	texturePainter.BuildConsole(320, 180, 6, 6);

	texturePainter.Start();

	std::cout << "End of main() reached\n";

	return 0;
}
