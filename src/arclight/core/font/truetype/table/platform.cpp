/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 platform.cpp
 */

#include "font/truetype/truetype.hpp"
#include "locale/codeconv.hpp"



namespace TrueType {


	bool verifyPlatformID(u16 platformID, u16 specificID, bool cmapExt) {

		if(platformID > 3) {
			return false;
		}

		PlatformID id = static_cast<PlatformID>(platformID);

		switch(id) {

			case PlatformID::Unicode:

				if((cmapExt && specificID > 6) || (!cmapExt && specificID > 4)) {
					return false;
				}

				break;

			default:
			case PlatformID::Reserved:
				return false;

			case PlatformID::Macintosh:

				if(specificID > 32) {
					return false;
				}

				break;

			case PlatformID::Microsoft:
							
				if(specificID > 6 && specificID != 10) {
					return false;
				}

				break;

		}

		return true;

	}


	std::string decodeText(PlatformID platformID, u16 specificID, const std::string& in) {

		switch(platformID) {

			case PlatformID::Unicode:
				//Unicode doesn't have to be re-converted
				break;

			case PlatformID::Macintosh:
				//Macintosh will not be supported
				break;

			case PlatformID::Microsoft:
			{
				MicrosoftEncoding encoding = static_cast<MicrosoftEncoding>(specificID);

				switch(encoding) {

					case MicrosoftEncoding::ShiftJIS:
						return CodeConv::fromShiftJIS<Unicode::Encoding::UTF8>(in);

					case MicrosoftEncoding::Big5:
						return CodeConv::fromBig5<Unicode::Encoding::UTF8>(in);
						
					case MicrosoftEncoding::Wansung:
						return CodeConv::fromWansung<Unicode::Encoding::UTF8>(in);
						
					case MicrosoftEncoding::Johab:
						return CodeConv::fromJohab<Unicode::Encoding::UTF8>(in);

					case MicrosoftEncoding::PRC:
						//PRC is not supported as of now
						break;

					case MicrosoftEncoding::Symbol:
						//No conversion necessary
						break;

					default:
					case MicrosoftEncoding::UnicodeBMP:
					case MicrosoftEncoding::UnicodeFull:
						break;

				}

			}
			break;

			default:
			case PlatformID::Reserved:
				break;

		}

		return in;

	}


}