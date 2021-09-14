#include "filestreamimpl.h"
#include "types.h"
#include "util/assert.h"
#include "util/file.h"


FileStreamImpl::FileStreamImpl(File& file) : file(file) {}


u64 FileStreamImpl::read(void* dest, u64 size) {

	arc_assert(dest != nullptr, "Destination is null");

	file.read(static_cast<u8*>(dest), size);
	return size;

}

u64 FileStreamImpl::write(const void* src, u64 size) {

	arc_assert(src != nullptr, "Source is null");

	file.write(static_cast<const u8*>(src), size);
	return size;

}



u64 FileStreamImpl::seek(i64 offset, StreamBase::SeekMode mode) {

	u64 pos = 0;

	switch (mode) {

	case StreamBase::SeekMode::Begin:
		arc_assert(offset >= 0, "Cannot seek before the start of the stream");
		pos = offset;
		break;

	case StreamBase::SeekMode::Current:
		pos = getPosition() + offset;
		break;

	case StreamBase::SeekMode::End:
		arc_assert(offset >= 0, "Cannot seek after the end of the stream");
		pos = getSize() - offset;
		break;

	}

	file.seek(pos);

	return pos;

}

u64 FileStreamImpl::getPosition() const {
	return file.tell();
}



u64 FileStreamImpl::getSize() const {
	return file.getFileSize();
}

bool FileStreamImpl::isOpen() const {
	return file.isOpen();
}