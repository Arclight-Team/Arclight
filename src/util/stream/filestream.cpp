#include "filestream.h"


FileStream::FileStream(File& file) : file(file) {
	arc_assert(isOpen(), "Attempted to create a FileStream with an unopened file");
}



u64 FileStream::read(void* dest, u64 size) {

	arc_assert(dest != nullptr, "Destination is null");

	file.read(static_cast<u8*>(dest), size);
	return size;

}

u64 FileStream::write(const void* src, u64 size) {

	arc_assert(src != nullptr, "Source is null");

	file.write(static_cast<const u8*>(src), size);
	return size;

}



u64 FileStream::seek(i64 offset, SeekMode mode) {

	u64 pos = 0;

	switch (mode) {

	case SeekMode::Begin:
		arc_assert(offset >= 0, "Cannot seek before the start of the stream");
		pos = offset;
		break;

	case SeekMode::Current:
		pos = tell() + offset;
		break;

	case SeekMode::End:
		arc_assert(offset >= 0, "Cannot seek after the end of the stream");
		pos = getSize() - offset;
		break;

	}

	file.seek(pos);

	return pos;

}

u64 FileStream::tell() const {
	return file.tell();
}



u64 FileStream::getSize() const {
	return file.getFileSize();
}

bool FileStream::isOpen() const {
	return file.isOpen();
}
