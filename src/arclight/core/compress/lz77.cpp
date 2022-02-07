/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 lz77.cpp
 */

#include "lz77.hpp"
#include "stream/inputstream.hpp"
#include "stream/binaryreader.hpp"
#include "math/math.hpp"
#include "debug.hpp"


constexpr static u32 wrapIndexDown(u32 value, u32 wrap) {
	
	if(value >= wrap) [[unlikely]] {
		return value - wrap;
	}

	return value;

}


constexpr static i32 wrapIndexUp(i32 value, i32 wrap) {
	
	if(value < 0) [[unlikely]] {
		return value + wrap;
	}

	return value;

}



std::vector<u8> LZ77::compress(InputStream& stream) {

	BinaryReader reader(stream);

	std::vector<u8> outputBuffer;
	outputBuffer.reserve(static_cast<SizeT>(stream.getSize() * 0.90));   //Assume compression ratio of 0.9

	//4KB dictionary, 14 byte preview buffer
	constexpr u32 bufferSize = 4095 + 14;
	u8 buffer[bufferSize];

	//Stream byte count
	SizeT streamSize = stream.getSize();

	//Fill our start window
	u32 startReadSize = Math::min(streamSize, 14);
	reader.read(std::span<u8>{buffer}, startReadSize);

	//Position of our window
	u32 windowPos = 0;

	//Dictionary end
	u32 dictEnd = 0;

	//Preview buffer end
	u32 previewBufEnd = startReadSize;


	//LZ77 compression
	while(wrapIndexUp(windowPos - 1, bufferSize) != previewBufEnd) {

		u32 searchPos = dictEnd;
		u32 bestPos = -1;
		u32 bestSize = 0;

		//Find the best candidate from the dictionary
		while(searchPos != windowPos) {

			if(buffer[searchPos] == buffer[windowPos]) {

				u32 curSize = 1;
				u32 curSearchPos = wrapIndexDown(searchPos + 1, bufferSize);
				u32 curWindowPos = wrapIndexDown(windowPos + 1, bufferSize);

				//Get the size of the current candidate
				while(buffer[curSearchPos] == buffer[curWindowPos] && curSize < 15 && curWindowPos != previewBufEnd) {

					curSize++;
					curSearchPos = wrapIndexDown(curSearchPos + 1, bufferSize);
					curWindowPos = wrapIndexDown(curWindowPos + 1, bufferSize);

				}

				//If we got the maximum size, set this as best candidate
				if(curSize == 15) {

					bestPos = searchPos;
					bestSize = 15;
					break;

				}

				//If we found a better match, save position + length
				if(curSize > bestSize) {

					bestPos = searchPos;
					bestSize = curSize;

				}

			}

			searchPos = wrapIndexDown(searchPos + 1, bufferSize);

		}

		//If no occurence has been found in the dictionary, emit a 0-0-C triplet
		if(bestPos == -1) {
			
			outputBuffer.push_back(0);
			outputBuffer.push_back(0);
			outputBuffer.push_back(buffer[windowPos]);
			if(outputBuffer.size() < 30) ArcDebug() << "Cursor" << windowPos << ArcEndl << outputBuffer << std::span<u8>{buffer, windowPos + 14};

		} else {

			//Calculate the offset for our triplet
			u32 offset = bestPos > windowPos ? windowPos + bufferSize - bestPos : windowPos - bestPos;

			//Emit a S-O-C triplet
			outputBuffer.push_back(bestSize << 4 | offset >> 8);
			outputBuffer.push_back(offset & 0xFF);
			outputBuffer.push_back(buffer[wrapIndexDown(windowPos + bestSize, bufferSize)]);

		}

		u32 advance = bestSize + 1;
		u32 readAdvance = advance;

		if(streamSize < advance) {
			readAdvance = streamSize;
		}

		streamSize -= readAdvance;

		//Read new data into preview buffer
		u32 readPos = previewBufEnd;

		if((readPos + readAdvance) >= bufferSize) {
			
			u32 breakCount = bufferSize - readPos;
			reader.read(std::span<u8>{&buffer[readPos], breakCount});
			reader.read(std::span<u8>{&buffer[0], readAdvance - breakCount});
			
		} else {

			reader.read(std::span<u8>{&buffer[readPos], readAdvance});

		}

		//Recalculate positions
		u32 dictSize = wrapIndexUp(windowPos - dictEnd, bufferSize);
		windowPos = wrapIndexDown(windowPos + advance, bufferSize);

		if(dictSize + advance > 4095) {
			dictEnd = wrapIndexUp(windowPos - 4095, bufferSize);
		}

		//Enough bytes left, move the window end
		previewBufEnd = wrapIndexDown(previewBufEnd + readAdvance, bufferSize);

	}

	return outputBuffer;

}



std::vector<u8> LZ77::decompress(InputStream& stream) {

	BinaryReader reader(stream);

	std::vector<u8> outputBuffer;
	outputBuffer.reserve(static_cast<SizeT>(stream.getSize() / 0.90));   //Assume compression ratio of 0.9

	//Stream must consist of triplets -> divisible by 3
	arc_assert(!(stream.getSize() % 3), "LZ77 stream corrupted");

	//Dictionary buffer
	constexpr u32 bufferSize = 4095;
	u8 dict[bufferSize];

	//Dictionary cursor
	u32 dictStart = 0;

	for(SizeT i = 0; i < stream.getSize() / 3; i++) {

		u32 t0 = reader.read<u8>();
		u32 t1 = reader.read<u8>();
		u32 t2 = reader.read<u8>();

		u32 size = (t0 & 0xF0) >> 4;
		u32 offset = (t0 & 0x0F) << 8 | t1;
		u32 c = t2;

		u32 sizeLeft = size;

		while(sizeLeft) {

			//Number of bytes to copy over
			u32 curCopy = Math::min(sizeLeft, offset);

			//Copy bytes over
			for(u32 j = 0; j < curCopy; j++) {

				dict[dictStart] = dict[wrapIndexUp(dictStart - offset, bufferSize)];
				outputBuffer.push_back(dict[dictStart]);
				dictStart = wrapIndexDown(dictStart + curCopy, bufferSize);

			}

			//Decrement bytes to copy
			sizeLeft -= curCopy;

			//Move dictionary cursor
			dictStart = wrapIndexDown(dictStart + curCopy, bufferSize);

		}            

		//Push last char
		dict[dictStart] = c;
		outputBuffer.push_back(c);

		//Increment cursor
		dictStart = wrapIndexDown(dictStart + 1, bufferSize);

	}

	return outputBuffer;

}
