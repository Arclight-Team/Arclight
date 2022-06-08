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


		template<CC::Arithmetic T>
		struct ToInteger {

			using Type =    TT::Conditional<CC::Integral<T>, T,
							TT::Conditional<CC::Equal<T, float>, i32, i64>>;

		};

		template<CC::Arithmetic T>
		struct ToFloat {

			constexpr static SizeT Size = sizeof(T);

			using Type =	TT::Conditional<CC::Float<T>, T,
							TT::Conditional<Size <= 2, float,
							TT::Conditional<Size <= 4, double, long double>>>;

		};

		template<class T>
		concept HasExposedInnerType = requires { T::Type; };

		template<class T>
		struct CommonArithmeticType {};

		template<class T> requires(HasExposedInnerType<T>)
		struct CommonArithmeticType<T> {

			using Type = typename T::Type;

		};

		template<CC::Arithmetic T>
		struct CommonArithmeticType<T> {

			using Type = T;

		};

		template<SizeT Size>
		struct HasSizedIntegral {
			constexpr static bool Value = Size == 1 || Size == 2 || Size == 4 || Size == 8;
		};

		template<SizeT Size>
		struct HasSizedMinIntegral {
			constexpr static bool Value = Size <= 8;
		};

		template<SizeT Size>
		struct HasSizedFloat {
			constexpr static bool Value = Size == sizeof(float) || Size == sizeof(double) || Size == sizeof(long double);
		};

		template<SizeT Size>
		struct HasSizedMinFloat {
			constexpr static bool Value = Size <= sizeof(long double);
		};


		template<SizeT Size> requires HasSizedIntegral<Size>::Value
		struct UnsignedFromSize {

			using Type =    TT::Conditional<Size == 1, u8,
							TT::Conditional<Size == 2, u16,
							TT::Conditional<Size == 4, u32, u64>>>;

		};

		template<SizeT Size> requires HasSizedIntegral<Size>::Value
		struct SignedFromSize {
			using Type = MakeSigned<typename UnsignedFromSize<Size>::Type>;
		};

		template<SizeT Size> requires HasSizedMinIntegral<Size>::Value
		struct UnsignedFromMinSize {

			using Type =    TT::Conditional<Size <= 1, u8,
							TT::Conditional<Size <= 2, u16,
							TT::Conditional<Size <= 4, u32, u64>>>;

		};

		template<SizeT Size> requires HasSizedMinIntegral<Size>::Value
		struct SignedFromMinSize {
			using Type = MakeSigned<typename UnsignedFromMinSize<Size>::Type>;
		};


		template<CC::Arithmetic T> requires (CC::Integral<T> || HasSizedIntegral<sizeof(T)>::Value)
		struct ToSizedInteger {
			using Type = TT::Conditional<CC::Integral<T>, T, typename SignedFromSize<sizeof(T)>::Type>;
		};

		template<CC::Arithmetic T> requires (CC::Float<T> || HasSizedFloat<sizeof(T)>::Value)
		struct ToSizedFloat {

			using Type =    TT::Conditional<CC::Float<T>, T,
							TT::Conditional<sizeof(T) == sizeof(float), float,
							TT::Conditional<sizeof(T) == sizeof(double), double, long double>>>;

		};


		template<CC::Arithmetic T> struct BiggerType { static_assert("Illegal type"); };
		template<> struct BiggerType<i8>    { using Type = i16; };
		template<> struct BiggerType<i16>   { using Type = i32; };
		template<> struct BiggerType<i32>   { using Type = i64; };
		template<> struct BiggerType<u8>    { using Type = u16; };
		template<> struct BiggerType<u16>   { using Type = u32; };
		template<> struct BiggerType<u32>   { using Type = u64; };
		template<> struct BiggerType<char>  { using Type = TT::Conditional<CC::SignedType<char>, i16, u16>; };

		template<CC::Arithmetic T> struct SmallerType { static_assert("Illegal type"); };
		template<> struct SmallerType<i16>  { using Type = i8;  };
		template<> struct SmallerType<i32>  { using Type = i16; };
		template<> struct SmallerType<i64>  { using Type = i32; };
		template<> struct SmallerType<u16>  { using Type = u8;  };
		template<> struct SmallerType<u32>  { using Type = u16; };
		template<> struct SmallerType<u64>  { using Type = u32; };

		template<class T> concept HasBiggerType = CC::Arithmetic<T> && requires { BiggerType<T>::Type; };
		template<class T> concept HasSmallerType = CC::Arithmetic<T> && requires { SmallerType<T>::Type; };

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
		struct ArraySizeHelper {};

		template<class T, SizeT N>
		struct ArraySizeHelper<T[N]> {
			static constexpr SizeT Size = N;
		};


		template<class T>
		struct TypeTagged {
			constexpr static bool Value = false;
		};

		template<template<class> class T, class U>
		struct TypeTagged<T<U>> {
			constexpr static bool Value = CC::Equal<T<U>, TT::TypeTag<U>>;
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


	/* Converts an Integer/Float type to a Float/Integer type */
	/*
	 *  It is guaranteed that any integer value is representable through the given float type,
	 *  However, the same cannot hold true for float -> int.
	 *  Therefore, the integer type that covers ~75% of all (usable) integer float values is provided instead.
	 */
	template<CC::Arithmetic A>
	using ToInteger = typename Detail::ToInteger<A>::Type;

	template<CC::Arithmetic A>
	using ToFloat = typename Detail::ToFloat<A>::Type;


	/* Converts an Integer/Float type to a Float/Integer type given that the resulting type has the same size as the original type */
	template<CC::Arithmetic A>
	using ToSizedInteger = typename Detail::ToSizedInteger<A>::Type;

	template<CC::Arithmetic A>
	using ToSizedFloat = typename Detail::ToSizedFloat<A>::Type;


	/* Tells whether the given type has a sized equivalent */
	template<CC::Arithmetic A>
	constexpr inline bool HasSizedInteger = Detail::HasSizedIntegral<sizeof(A)>::Value;

	template<CC::Arithmetic A>
	constexpr inline bool HasSizedFloat = Detail::HasSizedFloat<sizeof(A)>::Value;


	/* Defines the corresponding integral type with the given amount of bytes */
	template<SizeT Size>
	using UnsignedFromSize = typename Detail::UnsignedFromSize<Size>::Type;

	template<SizeT Size>
	using SignedFromSize = typename Detail::SignedFromSize<Size>::Type;


	/* Defines the corresponding integral type fitting at least the given amount of bytes */
	template<SizeT Size>
	using UnsignedFromMinSize = typename Detail::UnsignedFromMinSize<Size>::Type;

	template<SizeT Size>
	using SignedFromMinSize = typename Detail::SignedFromMinSize<Size>::Type;


	/* Extracts the size of an array */
	template<class T>
	constexpr inline SizeT ArraySize = Detail::ArraySizeHelper<T>::Size;


	/* Returns the smaller/bigger power of two type */
	template<CC::Arithmetic T>
	using BiggerType = typename Detail::BiggerType<T>::Type;

	template<CC::Arithmetic T>
	using SmallerType = typename Detail::SmallerType<T>::Type;


	/* True if the given type has a smaller/bigger power of two type */
	template<CC::Arithmetic T>
	constexpr inline bool HasBiggerType = Detail::HasBiggerType<T>;

	template<CC::Arithmetic T>
	constexpr inline bool HasSmallerType = Detail::HasSmallerType<T>;


	/* Selects a type depending on the size */
	template<class T, class U>
	using MinType = TT::Conditional<sizeof(T) <= sizeof(U), T, U>;

	template<class T, class U>
	using MaxType = TT::Conditional<sizeof(T) >= sizeof(U), T, U>;


	/* Returns the type after arithmetic promotion */
	template<CC::Arithmetic T>
	using PromotedType = decltype(T(0) + T(0));


	/* Returns the common type of multiple types */
	template<class... T>
	using CommonType = std::common_type_t<T...>;


	/* Extracts the arithmetic type from a mathematical construct T or uses the arithmetic type T */
	template<class T>
	using CommonArithmeticType = typename Detail::CommonArithmeticType<T>::Type;


	/* Specialized traits */
	template<SizeT N, class... Pack>
	using NthPackType = typename Detail::NthPackType<N, Pack...>::Type;

	template<SizeT N, CC::NestedType T>
	using NthInnerType = typename Detail::NthInnerType<N, T>::Type;

	template<CC::NestedType T>
	using InnerType = NthInnerType<0, T>;


	/* Type Tag */
	template<class T>
	struct TypeTag {};

	template<class T>
	constexpr static bool TypeTagged = Detail::TypeTagged<T>::Value;

}
