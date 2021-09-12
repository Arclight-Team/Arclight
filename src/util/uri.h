#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>


class Uri {

public:

	using SpecialUriHandler = std::function<std::string()>;

	Uri();
	Uri(const std::string& path);

	void move(const std::string& path);
	void setPath(const std::string& path);

	bool createDirectory() const;
	bool copy() const;
	bool remove() const;

	bool fileExists() const;
	bool directoryExists() const;
	std::string getPath() const;

	static bool fileExists(const std::string& path);
	static bool directoryExists(const std::string& path);

	static void setSpecialUriHandler(char symbol, SpecialUriHandler handler);
	static std::string getSpecialUriRootPath(char symbol);
	static std::string getSystemTempPath();

private:

	static std::unordered_map<char, SpecialUriHandler> specialHandlers;
	std::filesystem::path path;

};