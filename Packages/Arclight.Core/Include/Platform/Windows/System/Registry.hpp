/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Registry.hpp
 */

#pragma once

#include "Common/Types.hpp"

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <variant>


namespace System::Registry {

	enum class RootKey {
		ClassesRoot,
		CurrentConfig,
		CurrentUser,
		LocalMachine,
		Users
	};

	enum class KeyAccess {
		Read,
		Write,
		ReadWrite
	};

	class Key {

	public:

		enum class Type {
			Binary,
			DWord,
			DWordBE,
			ExpandableString,
			Link,
			MultiString,
			None,
			QWord,
			String
		};

		using Content = std::variant<bool, u32, u64, std::string, std::vector<u8>, std::vector<std::string>>;

		struct Value {
			Type type;
			Content content;
		};

		Key(Key&&) noexcept;
		Key& operator=(Key&&) noexcept;
		~Key();

		std::optional<Key> openSubkey(const std::string& name, bool use32BitRegistry = false);
		std::optional<Key> openSubkey(const std::string& name, KeyAccess access, bool use32BitRegistry = false);

		Type getType(const std::string& value) const;

		Value getValue(const std::string& value) const;
		std::optional<u32> getDWord(const std::string& value) const;
		std::optional<u64> getQWord(const std::string& value) const;
		std::optional<std::string> getString(const std::string& value) const;
		std::optional<std::vector<u8>> getBinary(const std::string& value) const;
		std::optional<std::vector<std::string>> getStringArray(const std::string& value) const;

		bool setDWord(const std::string& value, u32 data, bool bigEndian = false);
		bool setQWord(const std::string& value, u64 data);
		bool setString(const std::string& value, const std::string& data, bool expandable = false);
		bool setBinary(const std::string& value, const std::vector<u8>& data);
		bool setStringArray(const std::string& value, const std::vector<std::string>& data);

		bool hasValue(const std::string& value);

		bool rename(const std::string& newName);
		bool renameSubkey(const std::string& subkey, const std::string& newName);

		bool eraseKey(const std::string& subkey, bool use32BitRegistry = false);	//Fails if there are more subkeys
		bool eraseAll(const std::string& subkey);

		bool eraseValue(const std::string& name);
		bool eraseValue(const std::string& subkey, const std::string& name);

		bool isReadable() const;
		bool isWritable() const;

	private:

		friend class KeyGenerator;
		Key();

		std::unique_ptr<class KeyHandle> handle;

	};

	std::optional<Key> createKey(RootKey root, const std::string& name, KeyAccess access = KeyAccess::Read, bool use32BitRegistry = false);
	std::optional<Key> openKey(RootKey root, const std::string& name, KeyAccess access = KeyAccess::Read, bool use32BitRegistry = false);

}

