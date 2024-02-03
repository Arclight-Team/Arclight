/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 constants.hpp
 */

#pragma once

#include "common/concepts.hpp"

#include <numbers>



namespace Math::inline Constants {

	using ConstantT = double;


	template<CC::Float F> static constexpr F TEuler		= std::numbers::e_v<F>;
	template<CC::Float F> static constexpr F TLog2E		= std::numbers::log2e_v<F>;
	template<CC::Float F> static constexpr F TLog10E	= std::numbers::log10e_v<F>;
	template<CC::Float F> static constexpr F TPi		= std::numbers::pi_v<F>;
	template<CC::Float F> static constexpr F TInvPi		= std::numbers::inv_pi_v<F>;
	template<CC::Float F> static constexpr F TInvSqrtPi	= std::numbers::inv_sqrtpi_v<F>;
	template<CC::Float F> static constexpr F TLn2		= std::numbers::ln2_v<F>;
	template<CC::Float F> static constexpr F TLn10		= std::numbers::ln10_v<F>;
	template<CC::Float F> static constexpr F TSqrt2		= std::numbers::sqrt2_v<F>;
	template<CC::Float F> static constexpr F TSqrt3		= std::numbers::sqrt3_v<F>;
	template<CC::Float F> static constexpr F TInvSqrt3	= std::numbers::inv_sqrt3_v<F>;
	template<CC::Float F> static constexpr F TEGamma	= std::numbers::egamma_v<F>;
	template<CC::Float F> static constexpr F TPhi		= std::numbers::phi_v<F>;

	static constexpr ConstantT Euler		= TEuler<ConstantT>;
	static constexpr ConstantT Log2E		= TLog2E<ConstantT>;
	static constexpr ConstantT Log10E		= TLog10E<ConstantT>;
	static constexpr ConstantT Pi			= TPi<ConstantT>;
	static constexpr ConstantT InvPi		= TInvPi<ConstantT>;
	static constexpr ConstantT InvSqrtPi	= TInvSqrtPi<ConstantT>;
	static constexpr ConstantT Ln2			= TLn2<ConstantT>;
	static constexpr ConstantT Ln10			= TLn10<ConstantT>;
	static constexpr ConstantT Sqrt2		= TSqrt2<ConstantT>;
	static constexpr ConstantT Sqrt3		= TSqrt3<ConstantT>;
	static constexpr ConstantT InvSqrt3		= TInvSqrt3<ConstantT>;
	static constexpr ConstantT EGamma		= TEGamma<ConstantT>;
	static constexpr ConstantT Phi			= TPhi<ConstantT>;

}
