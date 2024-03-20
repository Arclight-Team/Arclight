/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 typetraits.hpp
 */

#pragma once

#include "common/types.hpp"
#include "concepts.hpp"

#include <type_traits>



namespace TT {

	template<class T>
	struct TypeTag {
		using Type = T;
	};


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


	template<class T>
	using RemoveRef = std::remove_reference_t<T>;

	template<class T>
	using AddLValueRef = std::add_lvalue_reference_t<T>;

	template<class T>
	using AddRValueRef = std::add_rvalue_reference_t<T>;


	template<class T>
	using RemoveCVRef = std::remove_cvref_t<T>;


	template<class T>
	using RemovePointer = std::remove_pointer_t<T>;

	template<class T>
	using AddPointer = std::add_pointer_t<T>;


	namespace Detail {

		template<class T>
		struct RemoveObjectPointer {
			using Type = T;
		};

		template<class O, class T>
		struct RemoveObjectPointer<T O::*> {
			using Type = T;
		};

	}

	template<class T>
	using RemoveObjectPointer = typename Detail::RemoveObjectPointer<T>::Type;

	template<class T, CC::Class O>
	using AddObjectPointer = T O::*;


	namespace Detail {

		template<class T>
		struct ArraySizeHelper;

		template<class T, SizeT N>
		struct ArraySizeHelper<T[N]> {
			static constexpr SizeT Size = N;
		};

	}

	template<class T>
	using ArrayBase = std::remove_all_extents_t<T>;

	template<class T>
	using ArrayElement = std::remove_extent_t<T>;

	template<CC::Array T>
	static constexpr SizeT ArraySize = Detail::ArraySizeHelper<T>::Size;

	template<class T>
	static constexpr SizeT ArrayRank = std::rank_v<T>;


	template<class T>
	using Decay = std::decay_t<T>;

	template<class T>
	using Underlying = std::underlying_type_t<T>;

	template<class F, class... Args>
	using InvokeResult = std::invoke_result_t<F, Args...>;

	template<bool C, class T>
	using EnableIf = std::enable_if_t<C, T>;

	template<class T>
	using Identity = std::type_identity_t<T>;

	template<class... T>
	using CommonType = std::common_type_t<T...>;

	template<class... T>
	using Void = std::void_t<T...>;


	template<CC::Arithmetic T>
	using MakeSigned = std::make_signed_t<T>;

	template<CC::Arithmetic T>
	using MakeUnsigned = std::make_unsigned_t<T>;

	template<CC::Arithmetic T>
	using PromotedType = decltype(T(0) + T(0));


	template<bool C, class A, class B>
	using Conditional = std::conditional_t<C, A, B>;

	template<bool B, class T>
	using ConditionalConst = Conditional<B, const T, T>;

	template<class T, bool C, template<class> class U>
	using ConditionalUnary = Conditional<C, U<T>, T>;

	template<class T, bool C>
	using MakeSignedIf = ConditionalUnary<T, C, MakeSigned>;

	template<class T, bool C>
	using MakeUnsignedIf = ConditionalUnary<T, C, MakeUnsigned>;


	namespace Detail {

		template<class T, class U, class... V>
		struct MinType {
			using Type = MinType<MinType<T, U>, V...>;
		};

		template<class T, class U>
		struct MinType<T, U> {
			using Type = Conditional<sizeof(T) <= sizeof(U), T, U>;
		};


		template<class T, class U, class... V>
		struct MaxType {
			using Type = MaxType<MaxType<T, U>, V...>;
		};

		template<class T, class U>
		struct MaxType<T, U> {
			using Type = Conditional<sizeof(T) >= sizeof(U), T, U>;
		};

	}

	template<class T, class... U>
	using MinType = typename Detail::MinType<T, U...>::Type;

	template<class T, class... U>
	using MaxType = typename Detail::MaxType<T, U...>::Type;


	namespace Detail {

		template<SizeT N, class T, class... Pack> requires (N < sizeof...(Pack) + 1)
		struct NthPackType {
			using Type = typename NthPackType<N - 1, Pack...>::Type;
		};

		template<class T, class... Pack>
		struct NthPackType<0, T, Pack...> {
			using Type = T;
		};


		template<SizeT N, auto V, auto... Pack> requires (N < sizeof...(Pack) + 1)
		struct NthPackValue {
			static constexpr decltype(V) Value = NthPackValue<N - 1, Pack...>::Value;
		};

		template<auto V, auto... Pack>
		struct NthPackValue<0, V, Pack...> {
			static constexpr decltype(V) Value = V;
		};


		template<SizeT N, class T>
		struct NthInnerType;

		template<SizeT N, template<class...> class T, class... U>
		struct NthInnerType<N, T<U...>> {
			using Type = typename NthPackType<N, U...>::Type;
		};

	}

	template<SizeT N, class... Pack>
	using NthPackType = typename Detail::NthPackType<N, Pack...>::Type;

	template<SizeT N, auto... Pack>
	static constexpr auto NthPackValue = Detail::NthPackValue<N, Pack...>::Value;

	template<SizeT N, CC::NestedType T>
	using NthInnerType = typename Detail::NthInnerType<N, T>::Type;

	template<CC::NestedType T>
	using InnerType = NthInnerType<0, T>;


	template<class T, class... Pack>
	static constexpr bool IsAnyOf = (CC::Equal<T, Pack> || ...);

	template<class T, class... Pack>
	static constexpr bool IsAllSame = (CC::Equal<T, Pack> && ...);

	template<SizeT N, class... Pack>
	static constexpr SizeT SizeofN = sizeof(NthPackType<N, Pack...>);

	template<SizeT N, class... Pack>
	static constexpr SizeT AlignofN = alignof(NthPackType<N, Pack...>);


	namespace Detail {

		template<SizeT Size>
		static constexpr bool IsIntegralSize = Size == 1 || Size == 2 || Size == 4 || Size == 8;

		template<SizeT Size>
		static constexpr bool IsFloatSize = Size == sizeof(float) || Size == sizeof(double) || Size == sizeof(long double);


		template<SizeT Size>
		static constexpr bool IsMinIntegralSize = Size <= 8;

		template<SizeT Size>
		static constexpr bool IsMinFloatSize = Size <= sizeof(long double);


		template<CC::Arithmetic T>
		struct ToInteger {

			using Type =	Conditional<CC::Integral<T>, T,
							Conditional<CC::Equal<T, float>, i32, i64>>;

		};

		template<CC::Arithmetic T>
		struct ToFloat {

			constexpr static SizeT Size = sizeof(T);

			using Type =	Conditional<CC::Float<T>, T,
							Conditional<Size <= 2, float,
							Conditional<Size <= 4, double, long double>>>;

		};


		template<SizeT Size> requires IsIntegralSize<Size>
		struct UnsignedFromSize {

			using Type =	Conditional<Size == 1, u8,
							Conditional<Size == 2, u16,
							Conditional<Size == 4, u32, u64>>>;

		};

		template<SizeT Size> requires IsIntegralSize<Size>
		struct SignedFromSize {
			using Type = MakeSigned<typename UnsignedFromSize<Size>::Type>;
		};

		template<SizeT Size> requires IsMinIntegralSize<Size>
		struct UnsignedFromMinSize {

			using Type =	Conditional<Size <= 1, u8,
							Conditional<Size <= 2, u16,
							Conditional<Size <= 4, u32, u64>>>;

		};

		template<SizeT Size> requires IsMinIntegralSize<Size>
		struct SignedFromMinSize {
			using Type = MakeSigned<typename UnsignedFromMinSize<Size>::Type>;
		};


		template<CC::Arithmetic T> requires (CC::Integral<T> || IsIntegralSize<sizeof(T)>)
		struct ToSizedInteger {
			using Type = Conditional<CC::Integral<T>, T, typename SignedFromSize<sizeof(T)>::Type>;
		};

		template<CC::Arithmetic T> requires (CC::Float<T> || IsFloatSize<sizeof(T)>)
		struct ToSizedFloat {

			using Type =	Conditional<CC::Float<T>, T,
							Conditional<sizeof(T) == sizeof(float), float,
							Conditional<sizeof(T) == sizeof(double), double, long double>>>;

		};

	}

	/*
		It is guaranteed that any integer value is representable through the given float type,
		However, the same cannot hold true for float -> int.
		Therefore, the integer type that covers ~75% of all (usable) integer float values is provided instead.
	*/

	template<CC::Arithmetic A>
	using ToInteger = typename Detail::ToInteger<A>::Type;

	template<CC::Arithmetic A>
	using ToFloat = typename Detail::ToFloat<A>::Type;

	// Converts an Integer/Float to a Float/Integer with matching size

	template<CC::Arithmetic A>
	using ToSizedInteger = typename Detail::ToSizedInteger<A>::Type;

	template<CC::Arithmetic A>
	using ToSizedFloat = typename Detail::ToSizedFloat<A>::Type;

	// Tells whether the given type has a sized equivalent

	template<CC::Arithmetic A>
	static constexpr bool HasSizedInteger = Detail::IsIntegralSize<sizeof(A)>;

	template<CC::Arithmetic A>
	static constexpr bool HasSizedFloat = Detail::IsFloatSize<sizeof(A)>;

	// Integral with the given amount of bytes

	template<SizeT Size>
	using UnsignedFromSize = typename Detail::UnsignedFromSize<Size>::Type;

	template<SizeT Size>
	using SignedFromSize = typename Detail::SignedFromSize<Size>::Type;

	// Integral type fitting at least the given amount of bytes

	template<SizeT Size>
	using UnsignedFromMinSize = typename Detail::UnsignedFromMinSize<Size>::Type;

	template<SizeT Size>
	using SignedFromMinSize = typename Detail::SignedFromMinSize<Size>::Type;


	namespace Detail {

		template<class T>
		struct BiggerType;

		template<CC::Equal<float> F> requires (sizeof(double) > sizeof(float))
		struct BiggerType<F> { using Type = double; };

		template<CC::Equal<double> F> requires (sizeof(long double) > sizeof(double))
		struct BiggerType<F> { using Type = long double; };

		template<> struct BiggerType<i8>	{ using Type = i16; };
		template<> struct BiggerType<i16>	{ using Type = i32; };
		template<> struct BiggerType<i32>	{ using Type = i64; };
		template<> struct BiggerType<u8>	{ using Type = u16; };
		template<> struct BiggerType<u16>	{ using Type = u32; };
		template<> struct BiggerType<u32>	{ using Type = u64; };
		template<> struct BiggerType<char>	{ using Type = Conditional<CC::SignedType<char>, i16, u16>; };


		template<class T>
		struct SmallerType;

		template<CC::Equal<double> F> requires (sizeof(float) < sizeof(double))
		struct SmallerType<F> { using Type = float; };

		template<CC::Equal<long double> F> requires (sizeof(double) < sizeof(long double))
		struct SmallerType<F> { using Type = double; };

		template<> struct SmallerType<i16> { using Type = i8; };
		template<> struct SmallerType<i32> { using Type = i16; };
		template<> struct SmallerType<i64> { using Type = i32; };
		template<> struct SmallerType<u16> { using Type = u8; };
		template<> struct SmallerType<u32> { using Type = u16; };
		template<> struct SmallerType<u64> { using Type = u32; };

	}

	template<CC::Arithmetic T>
	using BiggerType = typename Detail::BiggerType<T>::Type;

	template<CC::Arithmetic T>
	using SmallerType = typename Detail::SmallerType<T>::Type;


	template<CC::Arithmetic T>
	static constexpr bool HasBiggerType = CC::Exists<BiggerType<T>>;

	template<CC::Arithmetic T>
	static constexpr bool HasSmallerType = CC::Exists<SmallerType<T>>;


	namespace Detail {

		template<class From, class To>
		struct CopyQualifiers {

			using C = Conditional<CC::ConstType<From>, AddConst<To>, To>;
			using V = Conditional<CC::VolatileType<From>, AddVolatile<C>, C>;
			using L = Conditional<CC::LValueReference<From>, AddLValueRef<V>, V>;
			using R = Conditional<CC::LValueReference<From>, AddRValueRef<L>, L>;

			using Type = Conditional<CC::Pointer<From>, AddPointer<R>, R>;

		};


		template<class T>
		struct ExtractType {
			using Type = T;
		};

		template<CC::ExposesType T>
		struct ExtractType<T> {
			using Type = typename T::Type;
		};


		template<class T>
		struct TypeTagged {
			constexpr static bool Value = false;
		};

		template<template<class> class T, class U>
		struct TypeTagged<T<U>> {
			constexpr static bool Value = CC::Equal<T<U>, TypeTag<U>>;
		};

	}

	template<class From, class To>
	using CopyQualifiers = typename Detail::CopyQualifiers<From, To>::Type;

	template<class T>
	using ExtractType = typename Detail::ExtractType<T>::Type;

	template<class T>
	constexpr static bool TypeTagged = Detail::TypeTagged<T>::Value;


	// Type-dependent constants (for scoped static asserts)

	template<auto V, class... T>
	static constexpr decltype(V) Constant = V;

	template<class... T>
	static constexpr bool False = Constant<false, T...>;

	template<class... T>
	static constexpr bool True = Constant<true, T...>;

}
