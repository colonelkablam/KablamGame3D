#include <iostream>

#include "TexturePainter.h"
#include "Utility.h"

int main()
{
	std::cout << "Welcome to...\n\n";
	printChunkyString("Texture Painter"); // in utility
	std::cout << "\n";
	system("pause");

	TexturePainter texturePainter(L"Texture Painter!");

	// 4od res   180 x 90
	// low res		240 x 135
	// med res		320 x 180
	// med-high	res	480 x 270
	// high res		640 x 360
	// mega res		1280 x 720
	// ultra res	1920 x 1080

	if (texturePainter.GetUserStartInput())
	{
		texturePainter.BuildConsole(TexturePainter::CANVAS_DISPLAY_WIDTH + 20, TexturePainter::CANVAS_DISPLAY_HEIGHT + 26, 8.5, 8.5);
		texturePainter.Start();
	}

	std::cout << "End of main() Texture Painter reached.\n";

	Canvas::CleanUpStaticPointers();

	//Sleep(3000);

	return 0;
}
