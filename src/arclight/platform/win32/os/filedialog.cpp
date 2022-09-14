/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 filedialog.cpp
 */

#include "filedialog.hpp"
#include "util/log.hpp"
#include "filesystem/path.hpp"
#include "util/destructionguard.hpp"

#include <shobjidl.h>



class FileDialogHandle {

public:

	IFileDialog* dialog;

	FileDialogHandle() : dialog(nullptr) {}

	~FileDialogHandle() noexcept {

		if (dialog) {
			dialog->Release();
		}

	}

};



FileDialog::FileDialog(Type type) : type(type), options(Options::Validate | Options::ConfirmOverwrite) {
	createHandle();
}

FileDialog::~FileDialog() = default;



FileDialog::Options FileDialog::getOptions() const {
	return options;
}



void FileDialog::setDirectory(const Path& path) {

	if (!handle) {
		return;
	}

	const std::string& str = Path(path).toAbsolute().toNativeString();

	void* pItem = nullptr;

	if (!SUCCEEDED(SHCreateItemFromParsingName(std::wstring(str.begin(), str.end()).c_str(), nullptr, IID_IShellItem, &pItem))) {
		Log::error("File Dialog", "Failed to create directory shell item");
		return;
	}

	IShellItem* item = reinterpret_cast<IShellItem*>(pItem);

	if (!SUCCEEDED(handle->dialog->SetFolder(item))) {
		Log::error("File Dialog", "Failed to set default directory");
	}

	item->Release();

}



void FileDialog::setTitle(const std::string& title) {

	if (!handle) {
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetTitle(std::wstring(title.begin(), title.end()).c_str()))) {
		Log::error("File Dialog", "Failed to set dialog title");
	}

}



void FileDialog::setButtonText(const std::string& text) {

	if (!handle) {
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetOkButtonLabel(std::wstring(text.begin(), text.end()).c_str()))) {
		Log::error("File Dialog", "Failed to set button text");
	}

}



void FileDialog::setFilenameLabel(const std::string& text) {

	if (!handle) {
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetFileNameLabel(std::wstring(text.begin(), text.end()).c_str()))) {
		Log::error("File Dialog", "Failed to set filename label");
	}

}



void FileDialog::setInitialFilename(const std::string& filename) {

	if (!handle) {
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetFileName(std::wstring(filename.begin(), filename.end()).c_str()))) {
		Log::error("File Dialog", "Failed to set initial filename");
	}

}



void FileDialog::setDefaultExtension(const std::string& ext) {

	if (!handle) {
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetDefaultExtension(std::wstring(ext.begin(), ext.end()).c_str()))) {
		Log::error("File Dialog", "Failed to set default extension");
	}

}



void FileDialog::setOptions(Options options) {
	this->options = options;
}



void FileDialog::setFileFilters(const std::vector<FileFilter>& filters, SizeT selected) {

	if (!handle || filters.empty()) {
		return;
	}

	if (selected >= filters.size()) {
		selected = 0;
	}

	std::vector<std::wstring> wideStrings;
	std::vector<COMDLG_FILTERSPEC> convertedFilter;

	wideStrings.reserve(filters.size() * 2);
	convertedFilter.reserve(filters.size());

	for(const FileFilter& filter : filters) {

		std::string fullPattern = filter.patterns;

		for(auto it = fullPattern.rbegin(); it != fullPattern.rend(); it++) {

			if (*it != ' ') {

				fullPattern.resize(std::distance(fullPattern.begin(), it.base()));
				break;

			}

		}

		std::replace(fullPattern.begin(), fullPattern.end(), ' ', ';');

		SizeT offset = wideStrings.size();
		wideStrings.emplace_back(std::wstring(filter.text.begin(), filter.text.end()));
		wideStrings.emplace_back(std::wstring(fullPattern.begin(), fullPattern.end()));
		convertedFilter.emplace_back(COMDLG_FILTERSPEC {wideStrings[offset].c_str(), wideStrings[offset + 1].c_str()});

	}

	if (!SUCCEEDED(handle->dialog->SetFileTypes(convertedFilter.size(), convertedFilter.data()))) {
		Log::error("File Dialog", "Failed to set file types");
		return;
	}

	if (!SUCCEEDED(handle->dialog->SetFileTypeIndex(selected + 1))) {
		Log::error("File Dialog", "Failed to set file type index");
	}

}



Path FileDialog::getSelectedItem() const {

	IShellItem* item = nullptr;
	Path p;

	if (SUCCEEDED(handle->dialog->GetResult(&item))) {

		IShellItem* parentItem = nullptr;

		if (SUCCEEDED(item->GetParent(&parentItem))) {

			PWSTR parentPath = nullptr;

			if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &parentPath))) {

				p = Path(parentPath);
				CoTaskMemFree(parentPath);

			}

			parentItem->Release();

		}

		item->Release();

	}

	return p;

}



std::vector<Path> FileDialog::getSelection() const {

	if (!handle) {
		return { Path() };
	} else if (type == Type::Save) {
		return { getSelectedItem() };
	}

	IShellItemArray* itemArray = nullptr;
	IFileOpenDialog* dialog = static_cast<IFileOpenDialog*>(handle->dialog);

	DestructionGuard guard([&]() {

		if (itemArray) {
			itemArray->Release();
		}

	});

	if (SUCCEEDED(dialog->GetResults(&itemArray))) {

		DWORD itemCount = 0;

		if (SUCCEEDED(itemArray->GetCount(&itemCount)) && itemCount != 0) {

			std::vector<Path> paths;
			paths.reserve(itemCount);

			for (SizeT i = 0; i < itemCount; i++) {

				IShellItem* item = nullptr;

				if (SUCCEEDED(itemArray->GetItemAt(i, &item))) {

					PWSTR itemPath = nullptr;

					if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &itemPath))) {

						paths.emplace_back(itemPath);
						CoTaskMemFree(itemPath);

					}

					item->Release();

				}

			}

			return paths;

		}

	}

	return { Path() };

}



bool FileDialog::show() {

	if (!handle) {
		return false;
	}

	DWORD dialogOptions = FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR;

	if (type == Type::Save) {
		dialogOptions |= FOS_NOREADONLYRETURN;
	} else if (type == Type::SelectFolder) {
		dialogOptions |= FOS_PICKFOLDERS;
	}

	if (type == Type::Save && (options & Options::ConfirmOverwrite) == Options::ConfirmOverwrite) {
		dialogOptions |= FOS_OVERWRITEPROMPT;
	}

	if ((options & Options::Validate) != Options::Validate) {
		dialogOptions |= FOS_NOVALIDATE;
	}

	if (type != Type::Save && (options & Options::MultiSelect) == Options::MultiSelect) {
		dialogOptions |= FOS_ALLOWMULTISELECT;
	}

	if (type == Type::Save && (options & Options::ConfirmCreate) == Options::ConfirmCreate) {
		dialogOptions |= FOS_CREATEPROMPT;
	}

	if ((options & Options::DisableSymlinks) == Options::DisableSymlinks) {
		dialogOptions |= FOS_NODEREFERENCELINKS;
	}

	if ((options & Options::ShowHidden) == Options::ShowHidden) {
		dialogOptions |= FOS_FORCESHOWHIDDEN;
	}

	if (type == Type::Open && (options & Options::ShowPreview) == Options::ShowPreview) {
		dialogOptions |= FOS_FORCEPREVIEWPANEON;
	}

	if(!SUCCEEDED(handle->dialog->SetOptions(dialogOptions))) {
		Log::error("File Dialog", "Failed to set options");
		return false;
	}


	if (!SUCCEEDED(handle->dialog->Show(nullptr))) {
		return false;
	}

	return true;

}



Path FileDialog::getOpenFilePath(const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options) {
	return showAndReturnItem(Type::Open, title, dir, "", filters, selected, options);
}



Path FileDialog::getSelectedDirectory(const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options) {
	return showAndReturnItem(Type::SelectFolder, title, dir, "", filters, selected, options);
}



Path FileDialog::getSaveFilePath(const std::string& title, const Path& dir, const std::string& defaultExtension, const std::vector<FileFilter>& filters, SizeT selected, Options options) {
	return showAndReturnItem(Type::Save, title, dir, defaultExtension, filters, selected, options);
}



std::vector<Path> FileDialog::getOpenFilePaths(const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options) {
	return showAndReturnSelection(Type::Open, title, dir, filters, selected, options);
}



std::vector<Path> FileDialog::getSelectedDirectories(const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options) {
	return showAndReturnSelection(Type::SelectFolder, title, dir, filters, selected, options);
}



Path FileDialog::showAndReturnItem(Type type, const std::string& title, const Path& dir, const std::string& defaultExt, const std::vector<FileFilter>& filters, SizeT selected, Options options) {

	FileDialog dialog(type);

	if (!title.empty()) {
		dialog.setTitle(title);
	}

	if (!dir.isEmpty()) {
		dialog.setDirectory(dir);
	}

	if (!filters.empty()) {
		dialog.setFileFilters(filters, selected);
	}

	if (!defaultExt.empty()) {
		dialog.setDefaultExtension(defaultExt);
	}

	dialog.setOptions(options & ~Options::MultiSelect);

	if (dialog.show()) {
		return dialog.getSelectedItem();
	} else {
		return Path();
	}

}



std::vector<Path> FileDialog::showAndReturnSelection(Type type, const std::string& title, const Path& dir, const std::vector<FileFilter>& filters, SizeT selected, Options options) {

	FileDialog dialog(type);

	if (!title.empty()) {
		dialog.setTitle(title);
	}

	if (!dir.isEmpty()) {
		dialog.setDirectory(dir);
	}

	if (!filters.empty()) {
		dialog.setFileFilters(filters, selected);
	}

	dialog.setOptions(options | Options::MultiSelect);

	if (dialog.show()) {
		return dialog.getSelection();
	} else {
		return { Path() };
	}

}



void FileDialog::createHandle() {

	handle = std::make_unique<FileDialogHandle>();

	HRESULT result;

	if (type == Type::Save) {

		IFileSaveDialog* ptr;
		result = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptr));
		handle->dialog = ptr;

	} else {

		IFileOpenDialog* ptr;
		result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptr));
		handle->dialog = ptr;

	}

	if(!SUCCEEDED(result)) {
		Log::error("File Dialog", "Failed to create dialog instance");
		handle.reset();
	}

}