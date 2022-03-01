/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 concepts.hpp
 */

#pragma once

#include <concepts>


/* Basic type concepts */
template<class T, class U>
concept Equal = std::same_as<T, U>;

template<class Base, class Derived>
concept BaseOf = std::derived_from<Derived, Base>;

template<class From, class To>
concept ImpConvertible = std::is_convertible_v<From, To>;

template<class From, class To>
concept Convertible = std::convertible_to<From, To>;

template<class From, class To>
concept ExpConvertible = Convertible<From, To> && !ImpConvertible<From, To>;

template<class T, class U>
concept HasCommonReference = std::common_reference_with<T, U>;

template<class T, class U>
concept HasCommonType = std::common_with<T, U>;


/* Assign/Swap concepts */
template<class Left, class Right>
concept AssignableBy = std::assignable_from<Left, Right>;

template<class T>
concept Swappable = std::swappable<T>;

template<class T, class U>
concept SwappableWith = std::swappable_with<T, U>;


/* Special member function concepts */
template<class T>
concept Destructible = std::destructible<T>;

template<class T, class... Args>
concept Constructible = std::constructible_from<T, Args...>;

template<class T>
concept DefaultInitializable = std::default_initializable<T>;

template<class T>
concept CopyConstructible = std::copy_constructible<T>;

template<class T>
concept MoveConstructible = std::move_constructible<T>;


/* Fundamental type concepts */
template<class T>
concept Integral = std::integral<T>;

template<class T>
concept SignedIntegral = std::signed_integral<T>;

template<class T>
concept UnsignedIntegral = std::unsigned_integral<T>;

template<class T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<class T>
concept Float = std::floating_point<T>;

template<class T>
concept Integer = Integral<T> && !Equal<T, bool>;

template<class T>
concept Fundamental = std::is_fundamental_v<T>;

template<class T>
concept Char = Equal<T, char> || Equal<T, unsigned char> || Equal<T, signed char> || Equal<T, wchar_t> || Equal<T, char8_t> || Equal<T, char16_t> || Equal<T, char32_t>;


/* Enumeration type concepts */
template<class T>
concept Enum = std::is_enum_v<T>;

template<class T>
concept ScopedEnum = Enum<T> && !requires(T t, void(*p)(int)) { p(t); };

template<class T>
concept WeakEnum = Enum<T> && !ScopedEnum<T>;


/* Type categories */
template<class T>
concept PointerType = std::is_pointer_v<T>;

template<class T>
concept MemberObjectPointerType = std::is_member_object_pointer_v<T>;

template<class T>
concept MemberFunctionPointerType = std::is_member_function_pointer_v<T>;

template<class T>
concept MemberPointerType = std::is_member_pointer_v<T>;

template<class T>
concept ReferenceType = std::is_reference_v<T>;

template<class T>
concept LValueRefType = std::is_lvalue_reference_v<T>;

template<class T>
concept RValueRefType = std::is_rvalue_reference_v<T>;

template<class T>
concept ArrayType = std::is_array_v<T>;

template<class T>
concept BoundedArrayType = std::is_bounded_array_v<T>;

template<class T>
concept UnboundedArrayType = std::is_unbounded_array_v<T>;

template<class T>
concept UnionType = std::is_union_v<T>;

template<class T>
concept FunctionType = std::is_function_v<T>;

template<class T>
concept ClassType = std::is_class_v<T>;


template<class T>
concept SignedType = std::is_signed_v<T>;

template<class T>
concept UnsignedType = std::is_unsigned_v<T>;


template<class T>
concept Void = std::is_void_v<T>;

template<class T>
concept Nullptr = std::is_null_pointer_v<T>;


/* cv-qualified concepts */
template<class T>
concept ConstType = std::is_const_v<T>;

template<class T>
concept VolatileType = std::is_volatile_v<T>;

template<class T>
concept CVType = ConstType<T> || VolatileType<T>;


namespace __ConceptDetail {

	template<class>
	struct NestedType {
		constexpr static bool Value = false;
	};

	template<template<class> class T, class U>
	struct NestedType<T<U>> {
		constexpr static bool Value = true;
	};

}



/* Misc concepts */
template<class T>
concept BaseType = !PointerType<T> && !ReferenceType<T> && !MemberPointerType<T> && !Void<T> && !CVType<T> && !ArrayType<T>;

template<class T>
concept NestedType = __ConceptDetail::NestedType<T>::Value;

template<class F, class... A>
concept Invocable = std::is_invocable_v<F, A...>;

template<class F, class R>
concept Returns = Invocable<F> && Equal<std::invoke_result_t<F>, R>;