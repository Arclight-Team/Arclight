/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 filedialog.hpp
 */

#pragma once

#include "filesystem/path.hpp"
#include "util/bitmaskenum.hpp"
#include "types.hpp"

#include <string>
#include <vector>



struct FileFilter {
	std::string text;
	std::string patterns;
};

class FileDialog {

public:

	enum class Type {
		Open,
		Save,
		SelectFolder
	};

	enum class Options {

		None = 0x0,

		MultiSelect = 0x2,
		ShowHidden = 0x4,
		DisableSymlinks = 0x8,
		Validate = 0x10,

		//Open only
		ShowPreview = 0x20,

		//Save only
		ConfirmCreate = 0x40,
		ConfirmOverwrite = 0x80

	};

	ARC_CREATE_MEMBER_BITMASK_ENUM(Options)

	explicit FileDialog(Type type = Type::Open);
	~FileDialog();

	Options getOptions() const;

	void setDirectory(const Path& path);

	void setTitle(const std::string& title);
	void setButtonText(const std::string& text);
	void setFilenameLabel(const std::string& text);
	void setInitialFilename(const std::string& filename);
	void setDefaultExtension(const std::string& ext);

	void setOptions(Options options);
	void setFileFilters(const std::vector<FileFilter>& filters, SizeT selected = 0);

	Path getSelectedItem() const;
	std::vector<Path> getSelection() const;

	bool show();

	static Path getOpenFilePath(const std::string& title = std::string(), const Path& dir = Path(), const std::vector<FileFilter>& filters = {}, SizeT selected = 0, Options options = Options::Validate);
	static Path getSelectedDirectory(const std::string& title = std::string(), const Path& dir = Path(), const std::vector<FileFilter>& filters = {}, SizeT selected = 0, Options options = Options::Validate);
	static Path getSaveFilePath(const std::string& title = std::string(), const Path& dir = Path(), const std::string& defaultExtension = "", const std::vector<FileFilter>& filters = {}, SizeT selected = 0, Options options = Options::Validate);

	static std::vector<Path> getOpenFilePaths(const std::string& title = std::string(), const Path& dir = Path(), const std::vector<FileFilter>& filters = {}, SizeT selected = 0, Options options = Options::Validate);
	static std::vector<Path> getSelectedDirectories(const std::string& title = std::string(), const Path& dir = Path(), const std::vector<FileFilter>& filters = {}, SizeT selected = 0, Options options = Options::Validate);

private:

	static Path showAndReturnItem(Type type, const std::string& title, const Path& dir, const std::string& defaultExt, const std::vector<FileFilter>& filters, SizeT selected, Options options);
	static std::vector<Path> showAndReturnSelection(Type type, const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options);

	void createHandle();

	std::unique_ptr<class FileDialogHandle> handle;
	Options options;
	Type type;

};