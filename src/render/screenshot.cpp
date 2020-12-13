#include "screenshot.h"

#include "util/file.h"
#include "config.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



bool Screenshot::save(u32 w, u32 h, u8* data) {

	Uri fileUri(Config::getURIRootPath());
	fileUri.move("..");
	fileUri.move("screenshots");
	fileUri.createDirectory();
	fileUri.move("arclight_scshot.png");

	stbi_flip_vertically_on_write(true);
	return stbi_write_png(fileUri.getPath().c_str(), w, h, 3, data, w * 3);

}