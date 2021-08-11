#pragma once
#include "stream.h"
#include <vector>


class BinaryStream
{
public:

	using SeekMode = Stream::SeekMode;

	BinaryStream(Stream& stream);
	BinaryStream(Stream& stream, bool bigEndian);

	u64 read(void* dest, u64 size);
	u64 write(const void* src, u64 size);

	void seek(i64 offset, SeekMode mode = SeekMode::Begin);
	u64 tell() const;

	u64 getSize() const;
	bool isOpen() const;

	template<class T>
	T read() {

		using Type = std::remove_cv_t<T>;

		arc_assert(getSize() != 0, "Stream is empty");
		arc_assert((getSize() / sizeof(Type)) != 0, "Stream does not contain enough data to fill an object of type T");

		Type out;

		if (read(&out, sizeof(Type)) != sizeof(Type)) {
			return {};
		}

		if (bigEndian) {
			if constexpr (sizeof(Type) == 2) out = static_cast<Type>(arc_big16(static_cast<u16>(out)));
			if constexpr (sizeof(Type) == 4) out = static_cast<Type>(arc_big32(static_cast<u32>(out)));
			if constexpr (sizeof(Type) == 8) out = static_cast<Type>(arc_big64(static_cast<u64>(out)));
		}
		else {
			if constexpr (sizeof(Type) == 2) out = static_cast<Type>(arc_little16(static_cast<u16>(out)));
			if constexpr (sizeof(Type) == 4) out = static_cast<Type>(arc_little32(static_cast<u32>(out)));
			if constexpr (sizeof(Type) == 8) out = static_cast<Type>(arc_little64(static_cast<u64>(out)));
			if constexpr (sizeof(Type) == 8) out = static_cast<Type>(arc_little64(static_cast<u64>(out)));
		}

		return out;
	}
	template<class T>
	std::vector<T> read(u64 count) {

		u64 max = getSize() / sizeof(T);

		arc_assert(getSize() != 0, "Stream is empty");
		arc_assert(max != 0, "Stream does not contain enough data to fill an object of type T");
		arc_assert(count != 0, "Count must be greater than zero");

		if (count == UINT64_MAX) {
			count = max;
		}

		arc_assert(count <= max, "Count must be less than or equal to the stream' size");

		std::vector<T> out;
		out.resize(count);

		read(out.data(), out.size() * sizeof(T));

		return out;

	}
	template<class T>
	std::vector<T> readAll() {
		return read<T>(UINT64_MAX);
	}

	template<class T>
	void write(const T& value) {

		using Type = std::remove_cv_t<T>;

		Type in = value;

		if (bigEndian) {
			if constexpr (sizeof(Type) == 2) in = static_cast<Type>(arc_big16(static_cast<u16>(in)));
			if constexpr (sizeof(Type) == 4) in = static_cast<Type>(arc_big32(static_cast<u32>(in)));
			if constexpr (sizeof(Type) == 8) in = static_cast<Type>(arc_big64(static_cast<u64>(in)));
		}
		else {
			if constexpr (sizeof(Type) == 2) in = static_cast<Type>(arc_little16(static_cast<u16>(in)));
			if constexpr (sizeof(Type) == 4) in = static_cast<Type>(arc_little32(static_cast<u32>(in)));
			if constexpr (sizeof(Type) == 8) in = static_cast<Type>(arc_little64(static_cast<u64>(in)));
		}

		if (write(&in, sizeof(Type)) != sizeof(Type)) {
			arc_force_assert("Failed to write data to stream");
		}

	}
	template<class T>
	void write(const std::vector<T>& data, u64 count) {

		u64 max = data.size() / sizeof(T);

		arc_assert(data.size() != 0, "Source vector must not be empty");
		arc_assert(count != 0, "Count must be greater than zero");

		if (count == UINT64_MAX) {
			count = max;
		}

		arc_assert(count <= max, "Source vector's size must be greater than count");

		write(data.data(), data.size() * sizeof(T));

	}
	template<class T>
	void writeAll(const std::vector<T>& data) {
		write<T>(data, UINT64_MAX);
	}

private:

	Stream& stream;
	bool bigEndian;

};