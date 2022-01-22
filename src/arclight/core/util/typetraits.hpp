/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 typetraits.hpp
 */

#pragma once

#include "types.hpp"
#include "concepts.hpp"

#include <type_traits>


namespace TT {


	/* cv-qualifier traits */
	template<class T>
	using RemoveCV = std::remove_cv_t<T>;

	template<class T>
	using RemoveConst = std::remove_const_t<T>;

	template<class T>
	using RemoveVolatile = std::remove_volatile_t<T>;


	template<class T>
	using AddCV = std::add_cv_t<T>;

	template<class T>
	using AddConst = std::add_const_t<T>;

	template<class T>
	using AddVolatile = std::add_volatile_t<T>;


	/* Reference traits */
	template<class T>
	using RemoveRef = std::remove_reference_t<T>;

	template<class T>
	using AddLValueRef = std::add_lvalue_reference_t<T>;

	template<class T>
	using AddRValueRef = std::add_rvalue_reference_t<T>;


	/* Pointer traits */
	template<class T>
	using RemovePointer = std::remove_pointer_t<T>;

	template<class T>
	using AddPointer = std::add_pointer_t<T>;


	/* Integral traits */
	template<class T>
	using MakeSigned = std::make_signed_t<T>;

	template<class T>
	using MakeUnsigned = std::make_unsigned_t<T>;


	/* Array traits */
	template<class T>
	using RemoveArrayExtent = std::remove_extent_t<T>;

	template<class T>
	using ArrayBase = std::remove_all_extents_t<T>;


	/* Misc traits */
	template<class T>
	using Decay = std::decay_t<T>;

	template<class T>
	using RemoveCVRef = std::remove_cvref_t<T>;

	template<class T>
	using Underlying = std::underlying_type_t<T>;

	template<class F, class... Args>
	using InvokeResult = std::invoke_result_t<F, Args...>;

	template<class T>
	using TypeIdentity = std::type_identity_t<T>;

	template<bool C, class A, class B>
	using Conditional = std::conditional_t<C, A, B>;


	/* Implementation of new TTs */
	namespace Detail {

		template<class T, class... Pack>
		struct IsAnyOf {
			constexpr static bool Value = (std::is_same_v<T, Pack> || ...);
		};

		template<class T, class... Pack>
		struct IsAllSame {
			constexpr static bool Value = (std::is_same_v<T, Pack> && ...);
		};

		template<Float F> struct ToInteger { using Type = i32; };
		template<> struct ToInteger<double> { using Type = i64; };
		template<> struct ToInteger<long double> { using Type = imax; };

		template<Integer I> struct ToFloat { using Type = float; };
		template<> struct ToFloat<u64> { using Type = double; };
		template<> struct ToFloat<i64> { using Type = double; };

		template<SizeT Size> struct UnsignedFromSize {

			static_assert(Size <= sizeof(umax), "Cannot supply unsigned type greater than sizeof(umax)");

			using Type =    TT::Conditional<Size == 1, u8,
							TT::Conditional<Size == 2, u16,
							TT::Conditional<Size <= 4, u32,
							TT::Conditional<Size <= 8, u64, umax>>>>;

		};

		template<SizeT Size> struct SignedFromSize {

			static_assert(Size <= sizeof(imax), "Cannot supply signed type greater than sizeof(imax)");

			using Type =    TT::Conditional<Size == 1, i8,
							TT::Conditional<Size == 2, i16,
							TT::Conditional<Size <= 4, i32,
							TT::Conditional<Size <= 8, i64, imax>>>>;

		};


		template<SizeT N, class... Pack>
		constexpr inline SizeT SizeofN = []() {

			SizeT i = 0;
			SizeT s = 0;
			((i++ == N ? (s = sizeof(Pack), true) : false) || ...);

			return s;

		}();

	}


	/* True if any type in Pack matches T */
    template<class T, class... Pack>
    constexpr inline bool IsAnyOf = Detail::IsAnyOf<T, Pack...>::Value;

	/* True if all types in Pack equal T */
    template<class T, class... Pack>
    constexpr inline bool IsAllSame = Detail::IsAllSame<T, Pack...>::Value;

	/* Returns the size of the nth type in Pack */
    template<SizeT N, class... Pack> requires (N < sizeof...(Pack))
    constexpr inline SizeT SizeofN = Detail::SizeofN<N, Pack...>;


	/* Conditionally adds the const qualifier to T */
	template<bool B, class T>
	using ConditionalConst = std::conditional_t<B, const T, T>;


	/* Converts an Integer type to a roughly equivalent Float type */
    template<class T>
    using ToInteger = typename Detail::ToInteger<T>::Type;

    template<class T>
    using ToFloat = typename Detail::ToFloat<T>::Type;


	/* Defines the corresponding integral type fitting at least the given amount of bytes */
	template<SizeT Size>
	using UnsignedFromSize = typename Detail::UnsignedFromSize<Size>::Type;

	template<SizeT Size>
	using SignedFromSize = typename Detail::SignedFromSize<Size>::Type;

}

