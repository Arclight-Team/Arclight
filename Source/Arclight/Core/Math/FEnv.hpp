/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 FEnv.hpp
 */

#pragma once

#include "Common.hpp"
#include "Util/BitmaskEnum.hpp"

#include <cfenv>
#include <utility>


namespace Math::Except {

	enum class Exception {
		ZeroDivision	= FE_DIVBYZERO,
		Inexact			= FE_INEXACT,
		Invalid			= FE_INVALID,
		Overflow		= FE_OVERFLOW,
		Underflow		= FE_UNDERFLOW,
		All				= FE_ALL_EXCEPT
	};

	using enum Exception;

	ARC_CREATE_BITMASK_ENUM(Exception);

	using Flags = std::fexcept_t;
	using Enviroment = std::fenv_t;


	static const Enviroment& defaultEnviroment = *FE_DFL_ENV;


	inline bool clear(Exception exceptions) {
		return !std::feclearexcept(int(exceptions));
	}

	inline bool test(Exception exceptions) {
		return !std::fetestexcept(int(exceptions));
	}

	inline bool raise(Exception exceptions) {
		return !std::feraiseexcept(int(exceptions));
	}


	inline bool getFlags(Flags& flags, Exception exceptions) {
		return !std::fegetexceptflag(&flags, int(exceptions));
	}

	inline bool setFlags(const Flags& flags, Exception exceptions) {
		return !std::fesetexceptflag(&flags, int(exceptions));
	}


	inline bool setRounding(Rounding rounding) {

		int mode;

		switch (rounding) {
			case Rounding::Unknown:		mode = -1;
			case Rounding::TowardZero:	mode = FE_TOWARDZERO;
			case Rounding::Nearest:		mode = FE_TONEAREST;
			case Rounding::Upward:		mode = FE_UPWARD;
			case Rounding::Downward:	mode = FE_DOWNWARD;
			default:					std::unreachable();
		}

		return !std::fesetround(mode);

	}

	inline Rounding getRounding() {

		switch (std::fegetround()) {
			default:			return Rounding::Unknown;
			case FE_TOWARDZERO:	return Rounding::TowardZero;
			case FE_TONEAREST:	return Rounding::Nearest;
			case FE_UPWARD:		return Rounding::Upward;
			case FE_DOWNWARD:	return Rounding::Downward;
		}

	}


	inline bool getEnviroment(Enviroment& enviroment) {
		return !std::fegetenv(&enviroment);
	}

	inline bool setEnviroment(const Enviroment& enviroment) {
		return std::fesetenv(&enviroment);
	}

	inline bool holdEnviroment(Enviroment& enviroment) {
		return std::feholdexcept(&enviroment);
	}

	inline bool releaseEnviroment(const Enviroment& enviroment) {
		return !std::feupdateenv(&enviroment);
	}

}
