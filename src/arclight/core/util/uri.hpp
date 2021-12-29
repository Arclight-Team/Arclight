/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uri.hpp
 */

#pragma once

#include "types.hpp"

#include <filesystem>
#include <functional>
#include <unordered_map>


class Uri {

public:

	using SpecialUriHandler = std::function<std::string()>;

	struct CopyOptions {

		enum : u32 {
			SkipExisting = 0x1,
			OverwriteExisting = 0x2,
			UpdateExisting = 0x4,
			Recursive = 0x8,
			CopySymlinks = 0x10,
			SkipSymlinks = 0x20,
			DirectoriesOnly = 0x40,
			CreateSymlinks = 0x80,
			CreateHardlinks = 0x100
		};

	};

	Uri();
	Uri(const char* path, bool skipParsing = false);
	Uri(const std::string& path, bool skipParsing = false);

	void move(const std::string& path);
	void setPath(const std::string& path, bool skipParsing = false);

	bool createDirectory() const;
	bool copy(const Uri& target, u32 options = 0) const;
	bool remove() const;
	u64 removeRecursively() const;

	Uri rootName() const;
	Uri rootDirectory() const;
	Uri rootPath() const;
	Uri relativePath() const;
	Uri parentPath() const;
	Uri filename() const;
	Uri stem() const;
	Uri extension() const;

	bool fileExists() const;
	bool directoryExists() const;
	std::string getPath() const;
	u64 getFileSize() const;
	bool empty() const;

	bool hasRootName() const;
	bool hasRootDirectory() const;
	bool hasRootPath() const;
	bool hasRelativePath() const;
	bool hasParentPath() const;
	bool hasFilename() const;
	bool hasStem() const;
	bool hasExtension() const;

	static bool fileExists(const std::string& path);
	static bool directoryExists(const std::string& path);

	static void setSpecialUriHandler(char symbol, SpecialUriHandler handler);
	static std::string getSpecialUriRootPath(char symbol);
	static std::string getSystemTempPath();

private:

	static inline std::unordered_map<char, SpecialUriHandler> specialHandlers;
	std::filesystem::path path;

};