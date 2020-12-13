#include "screenshot.h"
#include "util/file.h"
#include "util/time.h"
#include "util/log.h"
#include "config.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



bool Screenshot::save(u32 w, u32 h, u8* data) {

	Uri fileUri(Config::getUriScreenshotPath());
	bool ssDirExists = fileUri.createDirectory();

	if (!ssDirExists) {
		Log::error("Screenshotter", "Failed to create directory 'screenshots/'");
		return false;
	}

	std::string timestamp = Time::getTimestamp();

	for (u32 i = 0; i < maxScreenshotsPerTimestamp; i++) {

		if (!i) {
			fileUri.setPath(Config::getUriScreenshotPath() + "arc_" + timestamp + ".png");
		} else {
			fileUri.setPath(Config::getUriScreenshotPath() + "arc_" + timestamp + "_" + std::to_string(i) + ".png");
		}

		if (!fileUri.fileExists()) {

			stbi_flip_vertically_on_write(true);
			return stbi_write_png(fileUri.getPath().c_str(), w, h, 3, data, w * 3);

		} else {
			continue;
		}

	}

	Log::error("Screenshotter", "Failed to create more than %d screenshots per second (timestamp = %s)", maxScreenshotsPerTimestamp, timestamp.c_str());
	return false;

}