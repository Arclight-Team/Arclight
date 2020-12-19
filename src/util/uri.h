#pragma once

#include <filesystem>


class Uri {

public:

	Uri();
	Uri(const char* path);
	Uri(const std::string& path);

	void setPath(const std::string& path);
	bool createDirectory();
	void move(const std::string& path);

	bool fileExists() const;
	bool directoryExists() const;
	std::string getPath() const;

	static bool fileExists(const std::string& path);
	static bool directoryExists(const std::string& path);

	static void setApplicationUriRoot(const std::string& root);

private:
	std::filesystem::path path;

};