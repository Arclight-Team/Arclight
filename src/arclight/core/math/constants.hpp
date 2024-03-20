/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 constants.hpp
 */

#pragma once

#include "meta/concepts.hpp"



namespace Math::inline Constants {

	using ConstantT = double;


	template<CC::Float F> static constexpr F TEuler		= 2.7182818284590452353602874713526624977572470936999595749669676277L;
	template<CC::Float F> static constexpr F TLog2E		= 1.4426950408889634073599246810018921374266459541529859341354494069L;
	template<CC::Float F> static constexpr F TLogE		= 0.4342944819032518276511289189166050822943970058036665661144537831L;
	template<CC::Float F> static constexpr F TPi		= 3.1415926535897932384626433832795028841971693993751058209749445923L;
	template<CC::Float F> static constexpr F TInvPi		= 0.3183098861837906715377675267450287240689192914809128974953346881L;
	template<CC::Float F> static constexpr F TInvSqrtPi	= 0.5641895835477562869480794515607725858440506293289988568440857217L;
	template<CC::Float F> static constexpr F TLn2		= 0.6931471805599453094172321214581765680755001343602552541206800094L;
	template<CC::Float F> static constexpr F TLn10		= 2.3025850929940456840179914546843642076011014886287729760333279009L;
	template<CC::Float F> static constexpr F TSqrt2		= 1.4142135623730950488016887242096980785696718753769480731766797379L;
	template<CC::Float F> static constexpr F TSqrt3		= 1.7320508075688772935274463415058723669428052538103806280558069794L;
	template<CC::Float F> static constexpr F TInvSqrt3	= 0.5773502691896257645091487805019574556476017512701268760186023264L;
	template<CC::Float F> static constexpr F TEGamma	= 0.5772156649015328606065120900824024310421593359399235988057672348L;
	template<CC::Float F> static constexpr F TPhi		= 1.6180339887498948482045868343656381177203091798057628621354486227L;
	template<CC::Float F> static constexpr F TLog2		= 0.3010299956639811952137388947244930267681898814621085413104274611L;
	template<CC::Float F> static constexpr F THalfPi	= 1.5707963267948966192313216916397514420985846996875529104874722961L;

	static constexpr ConstantT Euler		= TEuler<ConstantT>;
	static constexpr ConstantT Log2E		= TLog2E<ConstantT>;
	static constexpr ConstantT LogE			= TLogE<ConstantT>;
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
	static constexpr ConstantT Log2			= TLog2<ConstantT>;
	static constexpr ConstantT HalfPi		= THalfPi<ConstantT>;

}
