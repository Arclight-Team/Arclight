/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 log.hpp
 */

#pragma once

#include "string.hpp"
#include "filesystem/path.hpp"
#include "memory/memory.hpp"

#include <sstream>
#include <string>


namespace CC {

	template<class T>
	concept StringStreamable = requires(T&& t, std::stringstream stream) {
		stream << t;
	};

	template<class T>
	concept Iterable = requires(T&& t) {
		t.begin();
		t.end();
		requires std::input_iterator<decltype(t.begin())>;
		requires CC::Equal<decltype(t.begin()), decltype(t.end())>;
	};

	template<class T>
	concept ReverseIterable = requires(T&& t) {
		t.rbegin();
		t.rend();
		requires std::input_iterator<decltype(t.rbegin())>;
		requires CC::Equal<decltype(t.rbegin()), decltype(t.rend())>;
	};

}


class RawLog {

public:

	static inline SizeT maxContainerElements = 20;


	enum class Token {
		Endl,
		Dec,
		Hex,
		Upper,
		NoUpper,
		BoolAlpha,
		NoBoolAlpha,
		Forward,
		Reversed,
		Flush,

		Lower = NoUpper,
	};

	struct TabToken {
		SizeT width = 4;
		char c = ' ';
	};


	inline RawLog() noexcept : reversed(false) {}

	inline ~RawLog() noexcept {
		flush();
	};


	constexpr const std::stringstream& getBuffer() const noexcept {
		return buffer;
	}


	inline void print(const std::string& message) {
		buffer << message;
	};

	template<class... Args>
	inline void print(const std::string& message, Args&&... args) {
		print(String::format(message, std::forward<Args>(args)...));
	}


	inline RawLog& operator<<(CC::Char auto value) {
		buffer << value;
		return *this;
	}

	inline RawLog& operator<<(u8 value) {
		buffer << +value;
		return *this;
	}

	inline RawLog& operator<<(i8 value) {
		buffer << +value;
		return *this;
	}


	RawLog& operator<<(Token token);

	RawLog& operator<<(TabToken tab);

	RawLog& operator<<(const std::type_info& info);


	template<class A, class B>
	RawLog& operator<<(const std::pair<A, B>& pair){

		buffer << "[";
		*this << pair.first;
		buffer << ", ";
		*this << pair.second;
		buffer << "]";

		return *this;

	}

	template<CC::StringStreamable S> requires (!CC::Char<S>)
	RawLog& operator<<(const S& value) {

		if constexpr (CC::PointerType<S> && !CC::Equal<S, const char*>) {
			buffer << Memory::pointerAddress(value);
		} else {
			buffer << value;
		}

		return *this;

	}

	template<CC::Iterable T> requires (!CC::StringStreamable<T>)
	RawLog& operator<<(const T& container){

		const SizeT size = container.size();

		bool lineStart = true;

		if (!size) {
			buffer << "[Container empty]";
			return *this;
		}

		buffer << '[';

		auto exec = [&](const auto& begin, const auto& end) {

			SizeT i = 0;

			for (auto it = begin; it != end; ++it) {

				*this << *it;

				if (i++ == size) {
					buffer << ']';
					break;
				} else if (i >= maxContainerElements) {
					buffer << " ... " << (size - i) << " more elements]";
					lineStart = false;
					break;
				} else {
					buffer << ", ";
				}

			}

		};

		if constexpr (CC::ReverseIterable<T>) {
			if (reversed) {
				exec(container.rbegin(), container.rend());
				return *this;
			}
		}
		
		exec(container.begin(), container.end());

		return *this;

	}

protected:

	void flush() noexcept;


	std::stringstream buffer;
	bool reversed;

};


template<char C>
class PrefixedLog : public RawLog {

public:

	static constexpr char Char = C;


	inline PrefixedLog() : RawLog() {
		printPrefix();
	}

	explicit inline PrefixedLog(const std::string& name) : RawLog(), name(name) {
		printPrefix();
	}

	inline ~PrefixedLog() {
		buffer << '\n';
	}

	
	constexpr const std::string& getName() const noexcept {
		return name;
	}

private:

	void printPrefix() {

		buffer << '[' << Char;

		if (name.empty()) {
			buffer << "] ";
		} else {
			buffer << ": " << name << "] ";
		}

	}


	std::string name;

};


using LogR = RawLog;
using LogI = PrefixedLog<'I'>;
using LogW = PrefixedLog<'W'>;
using LogE = PrefixedLog<'E'>;
using LogD = PrefixedLog<'D'>;


namespace Log {

	using enum RawLog::Token;

	using Tab = RawLog::TabToken;


	void init() noexcept;

	void openLogFile(Path path) noexcept;

	void closeLogFile() noexcept;


	constexpr void setContainerLimits(SizeT elementsMax) noexcept {
		RawLog::maxContainerElements = elementsMax;
	};

}
