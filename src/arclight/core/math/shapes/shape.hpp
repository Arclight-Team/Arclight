/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 core.hpp
 */

#pragma once

#include "common/concepts.hpp"


class IShape {

protected:

	constexpr IShape() noexcept = default;

};


namespace CC {

	template<class T>
	concept Shape3D = BaseOf<T, IShape> && requires(const T&& s) {
		{ s.area() } -> CC::AnyOf<typename T::ScalarT, typename T::Type>;
		{ s.volume() } -> CC::AnyOf<typename T::ScalarT, typename T::Type>;
	};

	template<class T>
	concept Shape2D = BaseOf<T, IShape> && requires(const T&& s) {
		{ s.perimeter() } -> CC::AnyOf<typename T::ScalarT, typename T::Type>;
		{ s.area() } -> CC::AnyOf<typename T::ScalarT, typename T::Type>;
	};

	template<class T>
	concept Shape = Shape3D<T> || Shape2D<T>;

}
