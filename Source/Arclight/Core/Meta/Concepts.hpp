/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Concepts.hpp
 */

#pragma once

#include <concepts>



namespace CC {

	template<class T, class U>
	concept Equal = std::same_as<T, U>;

	template<class A, class B>
	concept EqualSize = sizeof(A) == sizeof(B);

	template<class A, class B>
	concept EqualAlign = alignof(A) == alignof(B);

	template<class T, class... U>
	concept AnyOf = (Equal<T, U> || ...);


	template<class Derived, class Base>
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


	template<class T, class... Args>
	concept Constructible = std::constructible_from<T, Args...>;

	template<class T, class... Args>
	concept TriviallyConstructible = std::is_trivially_constructible_v<T, Args...>;

	template<class T, class... Args>
	concept NothrowConstructible = std::is_nothrow_constructible_v<T, Args...>;

	template<class T>
	concept DefaultConstructible = std::default_initializable<T>;

	template<class T>
	concept TriviallyDefaultConstructible = std::is_trivially_default_constructible_v<T>;

	template<class T>
	concept NothrowDefaultConstructible = std::is_nothrow_default_constructible_v<T>;

	template<class T>
	concept CopyConstructible = std::copy_constructible<T>;

	template<class T>
	concept TriviallyCopyConstructible = std::is_trivially_copy_constructible_v<T>;

	template<class T>
	concept NothrowCopyConstructible = std::is_nothrow_copy_constructible_v<T>;

	template<class T>
	concept MoveConstructible = std::move_constructible<T>;

	template<class T>
	concept TriviallyMoveConstructible = std::is_trivially_move_constructible_v<T>;

	template<class T>
	concept NothrowMoveConstructible = std::is_nothrow_move_constructible_v<T>;


	template<class Left, class Right>
	concept Assignable = std::assignable_from<Left, Right>;

	template<class Left, class Right>
	concept TriviallyAssignable = std::is_trivially_assignable_v<Left, Right>;

	template<class Left, class Right>
	concept NothrowAssignable = std::is_nothrow_assignable_v<Left, Right>;

	template<class T>
	concept CopyAssignable = std::is_copy_assignable_v<T>;

	template<class T>
	concept TriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;

	template<class T>
	concept NothrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>;

	template<class T>
	concept MoveAssignable = std::is_move_assignable_v<T>;

	template<class T>
	concept TriviallyMoveAssignable = std::is_trivially_move_assignable_v<T>;

	template<class T>
	concept NothrowMoveAssignable = std::is_nothrow_move_assignable_v<T>;


	template<class T>
	concept Destructible = std::destructible<T>;

	template<class T>
	concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

	template<class T>
	concept NothrowDestructible = std::is_nothrow_destructible_v<T>;


	template<class T>
	concept VirtualDestructing = std::has_virtual_destructor_v<T>;


	template<class T>
	concept Swappable = std::swappable<T>;

	template<class T>
	concept NothrowSwappable = std::is_nothrow_swappable_v<T>;

	template<class T, class U>
	concept SwappableWith = std::swappable_with<T, U>;

	template<class T, class U>
	concept NothrowSwappableWith = std::is_nothrow_swappable_with_v<T, U>;


	template<class F, class... Args>
	concept Invocable = std::is_invocable_v<F, Args...>;

	template<class F, class... Args>
	concept NothrowInvocable = std::is_nothrow_invocable_v<F, Args...>;

	template<class F, class R, class... Args>
	concept Returns = std::is_invocable_r_v<R, F, Args...>;

	template<class F, class R, class... Args>
	concept NothrowReturns = std::is_nothrow_invocable_r_v<R, F, Args...>;


	template<class T>
	concept Integral = std::integral<T>;

	template<class T>
	concept SignedIntegral = std::signed_integral<T>;

	template<class T>
	concept UnsignedIntegral = std::unsigned_integral<T>;

	template<class T>
	concept SignedType = std::is_signed_v<T>;

	template<class T>
	concept UnsignedType = std::is_unsigned_v<T> && !Equal<T, bool>;

	template<class T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<class T>
	concept Float = std::floating_point<T>;

	template<class T>
	concept Integer = Integral<T> && !Equal<T, bool>;

	template<class T>
	concept Char = AnyOf<T, char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;

	template<class T>
	concept Fundamental = std::is_fundamental_v<T>;


	template<class T>
	concept Enum = std::is_enum_v<T>;

	template<class T>
	concept ScopedEnum = Enum<T> && !requires(T t, void(*p)(int)) { p(t); };

	template<class T>
	concept WeakEnum = Enum<T> && !ScopedEnum<T>;


	template<class T>
	concept Pointer = std::is_pointer_v<T>;

	template<class T>
	concept MemberObjectPointer = std::is_member_object_pointer_v<T>;

	template<class T>
	concept MemberFunctionPointer = std::is_member_function_pointer_v<T>;

	template<class T>
	concept MemberPointer = std::is_member_pointer_v<T>;


	template<class T>
	concept Reference = std::is_reference_v<T>;

	template<class T>
	concept LValueReference = std::is_lvalue_reference_v<T>;

	template<class T>
	concept RValueReference = std::is_rvalue_reference_v<T>;


	template<class T>
	concept Array = std::is_array_v<T>;

	template<class T>
	concept BoundedArray = std::is_bounded_array_v<T>;

	template<class T>
	concept UnboundedArray = std::is_unbounded_array_v<T>;


	template<class T>
	concept Union = std::is_union_v<T>;

	template<class T>
	concept Class = std::is_class_v<T>;


	template<class T>
	concept Function = std::is_function_v<T>;

	template<class T>
	concept Void = std::is_void_v<T>;

	template<class T>
	concept Nullptr = std::is_null_pointer_v<T>;


	template<class T>
	concept ConstType = std::is_const_v<T>;

	template<class T>
	concept VolatileType = std::is_volatile_v<T>;

	template<class T>
	concept CVType = ConstType<T> || VolatileType<T>;


	template<class T>
	concept BaseType = !Pointer<T> && !Reference<T> && !MemberPointer<T> && !Void<T> && !CVType<T> && !Array<T>;


	template<class T>
	concept Exists = (sizeof(T), true);

	template<class T>
	concept ExposesType = requires { typename T::Type; };


	namespace Detail {

		template<class T>
		struct NestedType {
			constexpr static bool Value = false;
		};

		template<template<class...> class T, class... U>
		struct NestedType<T<U...>> {
			constexpr static bool Value = true;
		};


		template<class T, template<class...> class P>
		struct SpecializationOf {
			static constexpr bool Value = false;
		};

		template<template<class...> class P, class... Args>
		struct SpecializationOf<P<Args...>, P> {
			static constexpr bool Value = true;
		};

	}

	template<class T>
	concept NestedType = Detail::NestedType<T>::Value;

	// Only handles non-value templates due to C++ limitations
	template<class T, template<class...> class P>
	concept SpecializationOf = Detail::SpecializationOf<T, P>::Value;

}
