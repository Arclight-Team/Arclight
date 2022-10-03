/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 exception.hpp
 */

#pragma once

#include "common.hpp"
#include "common/exception.hpp"

#include "util/bool.hpp"
#include <fmod_errors.h>
#include <source_location>
#include <stdexcept>



// ASX - Arclight FMOD wrapper
namespace ASX
{

	namespace __DebugLocation
	{
		inline std::source_location loc;
	}

	class ASXException : public ArclightException
	{
	public:

		using ArclightException::ArclightException;
		virtual const char* name() const noexcept override { return "ASX Exception"; }

	};

	// Accepts an FMOD result code and throws if not FMOD_OK or not present in the template filters
	template<Result... Filters>
	struct CaptureErrorCode
	{

		constexpr auto& capture(const std::source_location& loc) const {

			__DebugLocation::loc = loc;

			return *this;

		}

		constexpr void operator=(Result&& code) const {

			// Check if it matches FMOD_OK or any given filter
			if (Bool::any(code, FMOD_OK, Filters...))
				return;

			// Sadly never triggered...
			if (std::is_constant_evaluated()) {
				throw "Compile time failure";
			}
			else {

#if ARC_DEBUG

				LogE("ASX") << "FMOD failed with:";

				LogE("ASX") << FMOD_ErrorString(code);

				LogE("ASX").print("> in '%s'",
					__DebugLocation::loc.function_name());

				LogE("ASX").print("> at line [%d,%d] in '%s'",
					__DebugLocation::loc.line(),
					__DebugLocation::loc.column(),
					__DebugLocation::loc.file_name());

#endif

				throw ASXException(FMOD_ErrorString(code));

			}

		}

	};

	template<Result Default = FMOD_OK, Result... Filters>
	constexpr CaptureErrorCode<Default, Filters...> captureCode;

#define ASX_TRY(...) ASX::captureCode<FMOD_OK, __VA_ARGS__>.capture(std::source_location::current())

}


// Expose to global scope
using ASX::ASXException;
