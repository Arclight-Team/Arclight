/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 path.hpp
 */

#pragma once

#include "util/fixedchararray.hpp"

#include <filesystem>
#include <unordered_map>



class Path {

public:

	constexpr static auto separator = std::filesystem::path::preferred_separator;

	Path();
	Path(const char* path, bool annotated = false);
	Path(const std::string& path, bool annotated = false);
	explicit Path(const std::filesystem::path& path);

	Path& set(Path&& path);
	Path& set(const Path& path);

	Path& append(const std::string& str);
	Path& concat(const std::string& str);

	Path& setStem(const std::string& stem);
	Path& setExtension(const std::string& ext);
	Path& setFilename(const std::string& name);

	std::string toString() const;
	std::string toNativeString() const;

	std::string getStem() const;
	std::string getExtension() const;
	std::string getFilename() const;

	bool hasStem() const;
	bool hasExtension() const;
	bool hasFilename() const;

	Path& toAbsolute();
	Path& toCanonical();
	Path& toRelative();

	Path getAbsolute() const;
	Path getCanonical() const;
	Path getRelative() const;
	
	std::string relativeAgainst(const Path& base) const;

	bool isAbsolute() const;
	bool isRelative() const;

	void clear();
	bool isEmpty() const;

	bool exists() const;
	bool equivalent(const Path& other) const;

	Path root() const;
	Path parent() const;

	operator std::string() const;

	template<class S>
	Path& operator+=(const S& src) requires requires (Path&& p, S&& s) { p.concat(s); } {
		return concat(src);
	}

	template<class S>
	Path& operator/=(const S& src) requires requires (Path&& p, S&& s) { p.append(s); } {
		return append(src);
	}

	const std::filesystem::path& getHandle() const;


	static Path getCurrentWorkingDirectory();
	static void setCurrentWorkingDirectory(const Path& path);

	static Path getApplicationDirectory();


	enum class Folder {
		Desktop,
		Documents,
		Downloads,
		Images,
		Music,
		Videos,
		User,
		Recent,
		Temp
	};

	static Path getFolderPath(Folder folder);

	static std::string convertAnnotatedPath(const std::string& annotatedPath);
	
	static std::string getAnnotatedPathPrefix(char annotation);
	static void setAnnotatedPathPrefix(char annotation, const std::string& prefix);

	template<CC::Char C>
	constexpr static std::basic_string<C> quote(const std::basic_string<C>& path) {

		constexpr C whitespace(' ');
		constexpr C quotation('\"');
		constexpr C backslash('\\');

		if (!path.contains(whitespace)) {
			return path;
		}

		std::basic_string<C> s = path;
		bool escaped = false;

		for (SizeT i = 0; i < s.size(); i++) {

			C c = s[i];

			if ((c == quotation || c == backslash) && !escaped) {

				escaped = false;
				s.insert(i, std::basic_string<C>(1, backslash));
				i++;

			} else if (s[i] == backslash) {
				escaped = true;
			} else {
				escaped = false;
			}

		}

		return quotation + s + quotation;

	}

private:

	std::filesystem::path path;
	static inline std::unordered_map<char, std::string> annotatedPrefixes;

};

Path operator+(const Path& a, const Path& b);
Path operator/(const Path& a, const Path& b);


class RawLog;
RawLog& operator<<(RawLog& log, const Path& path);


//Defaults to annotated paths
template<FixedCharArray A>
inline Path operator""_path() {
    return Path(A.data, true);
}