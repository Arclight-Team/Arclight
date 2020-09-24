#include "util/log.h"
#include "util/vector.h"
#include <iostream>

#include "util/color.h"

void colorTest() {
	// color types: 4 (RGB15, RGBA16, RGB24, RGBA32)
	// possible conversions: 16	(4 ^ 2)
	// conversions to other types: 12 (16 - 4)
	// preferred types for conversions: RGBA32, RGBA16
	//
	// RGB24 -> RGBA32
	// RGB24 -> RGB15	(requires format conversion)
	// RGB24 -> RGBA16	(requires format conversion)
	//
	// RGBA32 -> RGB24
	// RGBA32 -> RGB15	(requires format conversion)
	// RGBA32 -> RGBA16	(requires format conversion) - preferred
	//
	// RGB15 -> RGB24	(requires format conversion)
	// RGB15 -> RGBA32	(requires format conversion)
	// RGB15 -> RGBA16	
	//
	// RGBA16 -> RGB24	(requires format conversion)
	// RGBA16 -> RGBA32	(requires format conversion) - preferred
	// RGBA16 -> RGB15
	//
	// remaining conversions: 2

	std::cout << std::hex;

	RGB15 col15 = 0x7FFF; // white, no alpha
	std::cout << std::endl << "Hex to RGB15\n";
	std::cout << "RGB: " << col15.rgb << "\n";
	std::cout << "R: " << (int)col15.r << "\n";
	std::cout << "G: " << (int)col15.g << "\n";
	std::cout << "B: " << (int)col15.b << "\n";
	std::cout << "_: " << (int)col15._ << "\n";

	// this conversion:
	// RGB15 -> RGBA32
	// will get expanded to:
	// RGB15 -> RGBA16 -> RGBA32
	RGBA32 col32 = col15;
	std::cout << std::endl << "RGB15 to RGBA32\n";
	std::cout << "RGBA: " << col32.rgba << "\n";
	std::cout << "R: " << (int)col32.r << "\n";
	std::cout << "G: " << (int)col32.g << "\n";
	std::cout << "B: " << (int)col32.b << "\n";
	std::cout << "A: " << (int)col32.a << "\n";

	// remove red
	RGBA16 col16 = col32;
	col16.r = 0;
	std::cout << std::endl << "RGBA32 to RGBA16 (no red)\n";
	std::cout << "RGBA: " << col16.rgba << "\n";
	std::cout << "R: " << (int)col16.r << "\n";
	std::cout << "G: " << (int)col16.g << "\n";
	std::cout << "B: " << (int)col16.b << "\n";
	std::cout << "A: " << (int)col16.a << "\n";

	// removes alpha
	RGB24 col24 = col16;
	std::cout << std::endl << "RGBA16 to RGB24\n";
	std::cout << "RGB: " << col24.rgb << "\n";
	std::cout << "R: " << (int)col24.r << "\n";
	std::cout << "G: " << (int)col24.g << "\n";
	std::cout << "B: " << (int)col24.b << "\n";
	std::cout << "_: " << (int)col24._ << "\n";

	col32 = col16;
	std::cout << std::endl << "RGBA16 to RGBA32\n";
	std::cout << "RGBA: " << col32.rgba << "\n";
	std::cout << "R: " << (int)col32.r << "\n";
	std::cout << "G: " << (int)col32.g << "\n";
	std::cout << "B: " << (int)col32.b << "\n";
	std::cout << "A: " << (int)col32.a << "\n";
}

int main(){

	Log::openLogFile();

	Log::info("Core", "Starting game engine");
	Log::info("Core", "Starting game engine");

	Vec2 a(1.2, 3.9);
	Vec2 b(0.3, 1.11);
	double x = Math::toDegrees(a.angle(b));
	std::cout << x << std::endl;

	colorTest();

	Log::closeLogFile();

	return 0;

}
