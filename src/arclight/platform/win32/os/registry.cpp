/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 registry.cpp
 */

#include "registry.hpp"
#include "locale/unicode.hpp"
#include "math/math.hpp"

#define NOMINMAX
#include "Windows.h"



namespace OS::Registry {

	struct KeyHandle {

		KeyHandle(HKEY key, KeyAccess keyAccess) : key(key), access(keyAccess) {}
		~KeyHandle() { RegCloseKey(key); }

		HKEY key;
		KeyAccess access;

	};

	class KeyGenerator {

	public:

		static Key generateKey(HKEY keyHandle, KeyAccess access) {

			Key key;
			key.handle = std::make_unique<KeyHandle>(keyHandle, access);

			return key;

		}

	};


	constexpr HKEY rootKeyToHKEY(RootKey key) noexcept {

		switch (key) {

			default:
			case RootKey::ClassesRoot:		return HKEY_CLASSES_ROOT;
			case RootKey::CurrentConfig:	return HKEY_CURRENT_CONFIG;
			case RootKey::CurrentUser:		return HKEY_CURRENT_USER;
			case RootKey::LocalMachine:		return HKEY_LOCAL_MACHINE;
			case RootKey::Users:			return HKEY_USERS;

		}

	}

	constexpr REGSAM keyAccessToSAM(KeyAccess access) noexcept {

		switch (access) {

			default:
			case KeyAccess::Read:		return KEY_READ;
			case KeyAccess::Write:		return KEY_WRITE;
			case KeyAccess::ReadWrite:	return KEY_READ | KEY_WRITE;

		}

	}


	std::optional<Key> openKeyParent(HKEY key, const std::string& name, KeyAccess access, bool use32BitRegistry) {

		HKEY openKey;

		if (!RegOpenKeyExW(key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(name).c_str(), 0, keyAccessToSAM(access) | (use32BitRegistry ? KEY_WOW64_32KEY : KEY_WOW64_64KEY), &openKey)) {
			return {KeyGenerator::generateKey(openKey, access)};
		} else {
			return {};
		}

	}


	Key::Key(Key&&) noexcept = default;
	Key& Key::operator=(Key&&) noexcept = default;
	Key::Key() = default;
	Key::~Key() = default;


	std::optional<Key> Key::openSubkey(const std::string& name, bool use32BitRegistry) {
		return openKeyParent(handle->key, name, handle->access, use32BitRegistry);
	}


	std::optional<Key> Key::openSubkey(const std::string& name, KeyAccess access, bool use32BitRegistry) {
		return openKeyParent(handle->key, name, access, use32BitRegistry);
	}


	Key::Type Key::getType(const std::string& value) const {
		return getValue(value).type;
	}


	Key::Value Key::getValue(const std::string& value) const {

		if (!isReadable()) {
			return {Key::Type::None, {}};
		}

		DWORD type;
		std::vector<u8> buffer(8);
		DWORD bufferSize = buffer.size();
		std::wstring valueStr = Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value);

		LSTATUS result = RegQueryValueExW(handle->key, valueStr.c_str(), nullptr, &type, buffer.data(), &bufferSize);

		if (result == ERROR_MORE_DATA) {

			buffer.resize(bufferSize);
			result = RegQueryValueExW(handle->key, valueStr.c_str(), nullptr, &type, buffer.data(), &bufferSize);

		}

		if (result) {
			return {Key::Type::None, {}};
		}

		Type typedType;

		switch (type) {

			case REG_BINARY:			typedType = Type::Binary;			break;
			case REG_DWORD:				typedType = Type::DWord;			break;
			case REG_DWORD_BIG_ENDIAN:	typedType = Type::DWordBE;			break;
			case REG_EXPAND_SZ:			typedType = Type::ExpandableString;	break;
			case REG_LINK:				typedType = Type::Link;				break;
			case REG_MULTI_SZ:			typedType = Type::MultiString;		break;
			default:
			case REG_NONE:				typedType = Type::None;				break;
			case REG_QWORD:				typedType = Type::QWord;			break;
			case REG_SZ:				typedType = Type::String;			break;

		}

		switch (typedType) {

			case Type::DWord:
			case Type::DWordBE:
			{
				u32 v = Bits::assemble<u32>(buffer.data(), Math::min(buffer.size(), 4));

				if (BigEndian ^ (typedType == Type::DWordBE)) {
					v = Bits::swap(v);
				}

				return {typedType, v};
			}

			case Type::QWord:
			{
				u64 v = Bits::assemble<u64>(buffer.data(), Math::min(buffer.size(), 8));
				v = Bits::little(v);

				return {typedType, v};
			}

			case Type::Binary:
			{
				buffer.resize(bufferSize);
				return {typedType, buffer};
			}

			default:
			case Type::None:
				return {typedType, false};

			case Type::String:
			case Type::ExpandableString:
			case Type::Link:

				if (bufferSize && buffer[bufferSize - 1] == '\0') {
					bufferSize--;
				}

				return {typedType, Unicode::convert<char, Unicode::UTF16LE, char, Unicode::UTF8>(std::string(buffer.begin(), buffer.begin() + bufferSize))};

			case Type::MultiString:
			{
				std::vector<std::string> strings;
				SizeT strStart = 0;

				if (!bufferSize || bufferSize % 2 || (bufferSize == 2 && buffer[0] == '\0' && buffer[1] == '\0')) {
					return {typedType, strings};
				}

				for (SizeT i = 0; i < bufferSize; i += 2) {

					u16 c = Bits::assemble<u16>(buffer.data() + i);

					//Null terminator (UTF-16)
					if (c == 0) {

						//Empty string, stop
						if (strStart == i) {

							strStart = bufferSize;
							break;

						}

						strings.emplace_back(Unicode::convert<char, Unicode::UTF16LE, char, Unicode::UTF8>(std::string(buffer.data() + strStart, buffer.data() + i)));

						strStart = i + 2;

					}

				}

				return {typedType, strings};

			}

		}

	}

	std::optional<u32> Key::getDWord(const std::string& value) const {

		Value v = getValue(value);
		return v.type == Type::DWord || v.type == Type::DWordBE ? std::get<u32>(getValue(value).content) : std::optional<u32>{};

	}

	std::optional<u64> Key::getQWord(const std::string& value) const {

		Value v = getValue(value);
		return v.type == Type::QWord ? std::get<u64>(getValue(value).content) : std::optional<u64>{};

	}

	std::optional<std::string> Key::getString(const std::string& value) const {

		Value v = getValue(value);
		return v.type == Type::String || v.type == Type::Link || v.type == Type::ExpandableString ? std::get<std::string>(getValue(value).content) : std::optional<std::string>{};

	}

	std::optional<std::vector<u8>> Key::getBinary(const std::string& value) const {

		Value v = getValue(value);
		return v.type == Type::Binary ? std::get<std::vector<u8>>(getValue(value).content) : std::optional<std::vector<u8>>{};

	}

	std::optional<std::vector<std::string>> Key::getStringArray(const std::string& value) const {

		Value v = getValue(value);
		return v.type == Type::MultiString ? std::get<std::vector<std::string>>(getValue(value).content) : std::optional<std::vector<std::string>>{};

	}


	bool Key::setDWord(const std::string& value, u32 data, bool bigEndian) {

		if (!isWritable()) {
			return false;
		}

		return !RegSetKeyValueW(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), bigEndian ? REG_DWORD_BIG_ENDIAN : REG_DWORD, Bits::toByteArray(&data), 4);

	}

	bool Key::setQWord(const std::string& value, u64 data) {

		if (!isWritable()) {
			return false;
		}

		return !RegSetKeyValueW(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), REG_QWORD, Bits::toByteArray(&data), 8);

	}

	bool Key::setString(const std::string& value, const std::string& data, bool expandable) {

		if (!isWritable()) {
			return false;
		}

		std::wstring wideString = Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(data);
		return !RegSetKeyValueW(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), expandable ? REG_EXPAND_SZ : REG_SZ, Bits::toByteArray(wideString.c_str()), (wideString.size() + 1) * sizeof(wchar_t));

	}

	bool Key::setBinary(const std::string& value, const std::vector<u8>& data) {

		if (!isWritable()) {
			return false;
		}

		return !RegSetKeyValueW(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), REG_BINARY, data.data(), data.size());

	}

	bool Key::setStringArray(const std::string& value, const std::vector<std::string>& data) {

		if (!isWritable()) {
			return false;
		}

		std::vector<std::wstring> wideStrings;
		wideStrings.reserve(data.size());

		for (const std::string& s : data) {
			wideStrings.emplace_back(Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(s));
		}

		SizeT bufferSize = data.size() + 1;

		for (const std::wstring& s : wideStrings) {
			bufferSize += s.size();
		}

		std::vector<wchar_t> reducedBuffer(bufferSize, 0);

		for (SizeT i = 0; const std::wstring& s : wideStrings) {

			std::copy(s.begin(), s.end(), reducedBuffer.begin() + i);
			i += s.size() + 1;

		}

		return !RegSetKeyValueW(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), REG_MULTI_SZ, Bits::toByteArray(reducedBuffer.data()), bufferSize * sizeof(wchar_t));

	}


	bool Key::hasValue(const std::string& value) {

		if (!isReadable()) {
			return false;
		}

		return !RegQueryValueExW(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(value).c_str(), nullptr, nullptr, nullptr, nullptr);

	}


	bool Key::rename(const std::string& newName) {

		if (!isWritable()) {
			return false;
		}

		return !RegRenameKey(handle->key, nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(newName).c_str());

	}

	bool Key::renameSubkey(const std::string& subkey, const std::string& newName) {

		if (!isWritable()) {
			return false;
		}

		return !RegRenameKey(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(subkey).c_str(), Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(newName).c_str());

	}

	bool Key::eraseKey(const std::string& subkey, bool use32BitRegistry) {

		if (!isWritable()) {
			return false;
		}

		return !RegDeleteKeyExW(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(subkey).c_str(), use32BitRegistry ? KEY_WOW64_32KEY : KEY_WOW64_64KEY, 0);

	}

	bool Key::eraseAll(const std::string& subkey) {

		if (!isReadable() || !isWritable()) {
			return false;
		}

		return !RegDeleteTreeW(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(subkey).c_str());

	}

	bool Key::eraseValue(const std::string& name) {

		if (!isWritable()) {
			return false;
		}

		return !RegDeleteValueW(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(name).c_str());

	}


	bool Key::eraseValue(const std::string& subkey, const std::string& name) {

		if (!isWritable()) {
			return false;
		}

		return !RegDeleteKeyValueW(handle->key, Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(subkey).c_str(), Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(name).c_str());

	}


	bool Key::isReadable() const {
		return handle->access == KeyAccess::Read || handle->access == KeyAccess::ReadWrite;
	}

	bool Key::isWritable() const {
		return handle->access == KeyAccess::Write || handle->access == KeyAccess::ReadWrite;
	}


	std::optional<Key> createKey(RootKey root, const std::string& name, KeyAccess access, bool use32BitRegistry) {

		HKEY key;

		if (!RegCreateKeyExW(rootKeyToHKEY(root), Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(name).c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, keyAccessToSAM(access) | (use32BitRegistry ? KEY_WOW64_32KEY : KEY_WOW64_64KEY), nullptr, &key, nullptr)) {
			return {KeyGenerator::generateKey(key, access)};
		} else {
			return {};
		}

	}


	std::optional<Key> openKey(RootKey root, const std::string& name, KeyAccess access, bool use32BitRegistry) {
		return openKeyParent(rootKeyToHKEY(root), name, access, use32BitRegistry);
	}

}

