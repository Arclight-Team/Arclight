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

	template<bool C, class T>
	using EnableIf = std::enable_if_t<C, T>;

	template<bool C, class A, class B>
	using Conditional = std::conditional_t<C, A, B>;


	template<class> struct TypeTag;

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

		template<Arithmetic A>
		struct ToInteger {

			constexpr static SizeT Size = sizeof(A);

			using Type =    TT::Conditional<!Float<A>, A,
							TT::Conditional<Size <= 4, i32,
							TT::Conditional<Size <= 8, i64, imax>>>;

		};

		template<Arithmetic A>
		struct ToFloat {

			constexpr static SizeT Size = sizeof(A);

			using Type =	TT::Conditional<Float<A>, A,
							TT::Conditional<Size <= sizeof(float), float,
							TT::Conditional<Size <= sizeof(double), double, long double>>>;

		};
		template<> struct ToFloat<u64> { using Type = double; };
		template<> struct ToFloat<i64> { using Type = double; };

		template<class T>
		concept HasExposedInnerType = requires { T::Type; };

		template<class T>
		struct CommonArithmeticType {};

		template<class T> requires(HasExposedInnerType<T>)
		struct CommonArithmeticType<T> {

			using Type = typename T::Type;

		};

		template<Arithmetic T>
		struct CommonArithmeticType<T> {

			using Type = T;

		};

		template<SizeT Size>
		struct UnsignedFromSize {

			static_assert(Size <= sizeof(umax), "Cannot supply unsigned type greater than sizeof(umax)");

			using Type =    TT::Conditional<Size == 1, u8,
							TT::Conditional<Size == 2, u16,
							TT::Conditional<Size <= 4, u32,
							TT::Conditional<Size <= 8, u64, umax>>>>;

		};

		template<SizeT Size>
		struct SignedFromSize {

			static_assert(Size <= sizeof(imax), "Cannot supply signed type greater than sizeof(imax)");

			using Type =    TT::Conditional<Size == 1, i8,
							TT::Conditional<Size == 2, i16,
							TT::Conditional<Size <= 4, i32,
							TT::Conditional<Size <= 8, i64, imax>>>>;

		};


		template<SizeT N, class T, class... Pack>
		struct PackHelper {
			using Type = typename PackHelper<N - 1, Pack...>::Type;
		};

		template<class T, class... Pack>
		struct PackHelper<0, T, Pack...> {
			using Type = T;
		};

		template<SizeT N, class... Pack> requires (sizeof...(Pack) >= N)
		struct NthPackType {
			using Type = typename PackHelper<N, Pack...>::Type;
		};


		template<SizeT, class>
		struct NthInnerType {};

		template<SizeT N, template<class...> class T, class... U>
		struct NthInnerType<N, T<U...>> {
			using Type = typename NthPackType<N, U...>::Type;
		};


		template<SizeT N, class... Pack>
		constexpr inline SizeT SizeofN = sizeof(typename NthPackType<N, Pack...>::Type);


		template<class T>
		struct TypeTagged {
			constexpr static bool Value = false;
		};

		template<template<class> class T, class U>
		struct TypeTagged<T<U>> {
			constexpr static bool Value = Equal<T<U>, TT::TypeTag<U>>;
		};

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
	template<Arithmetic A>
	using ToInteger = typename Detail::ToInteger<A>::Type;

	template<Arithmetic A>
	using ToFloat = typename Detail::ToFloat<A>::Type;


	/* Defines the corresponding integral type fitting at least the given amount of bytes */
	template<SizeT Size>
	using UnsignedFromSize = typename Detail::UnsignedFromSize<Size>::Type;

	template<SizeT Size>
	using SignedFromSize = typename Detail::SignedFromSize<Size>::Type;


	/* Extracts the arithmetic type from a mathematical construct T or uses the arithmetic type T */
	template<class T>
	using CommonArithmeticType = typename Detail::CommonArithmeticType<T>::Type;


	/* Specialized traits */
	template<SizeT N, class... Pack>
	using NthPackType = typename Detail::NthPackType<N, Pack...>::Type;

	template<SizeT N, NestedType T>
	using NthInnerType = typename Detail::NthInnerType<N, T>::Type;

	template<NestedType T>
	using InnerType = NthInnerType<0, T>;


	/* Type Tag */
	template<class T>
	struct TypeTag {};

	template<class T>
	constexpr static bool TypeTagged = Detail::TypeTagged<T>::Value;

}