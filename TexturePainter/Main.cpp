#include <iostream>
#include <Windows.h>  // For Windows console manip

#include "TexturePainter.h"
#include "Utility.h"

int main()
{
    // Set console window size and center it
    SetConsoleWindowSize(100, 30, 30);
    SetConsoleFont(16, L"Lucida Console");
    CenterConsoleWindow();


	std::cout << "Welcome to...\n\n";
	printChunkyString("Texture Painter"); // in utility
	std::cout << "\n";
	system("pause");

	TexturePainter texturePainter(L"Texture Painter!");

	// 4od res		180 x 90
	// low res		240 x 135
	// med res		320 x 180
	// med-high	res	480 x 270
	// high res		640 x 360
	// mega res		1280 x 720
	// ultra res	1920 x 1080

	if (texturePainter.GetUserStartInput())
	{
		texturePainter.BuildConsole(TexturePainter::CANVAS_DISPLAY_WIDTH + 20, TexturePainter::CANVAS_DISPLAY_HEIGHT + 26, 8.5, 8.5, 0, 0);
		texturePainter.Start();
	}

	// manually clean up any static pointers in canvas
	Canvas::CleanUpStaticPointers();

	// print that the end of main reached to OG console
	std::cout << "End of main() Texture Painter reached.\n";

	return 0;
}
