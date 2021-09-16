#pragma once

#include <concepts>


/* Basic type concepts */
template<class T, class U>
concept Equal = std::same_as<T, U>;

template<class Base, class Derived>
concept BaseOf = std::derived_from<Derived, Base>;

template<class From, class To>
concept Convertible = std::convertible_to<From, To>;

template<class T, class U>
concept HasCommonReference = std::common_reference_with<T, U>;

template<class T, class U>
concept HasCommonType = std::common_with<T, U>;

template<class T>
concept BaseType = !std::is_pointer_v<T> && !std::is_reference_v<T> && !std::is_member_pointer_v<T> && !std::is_void_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> && !std::is_array_v<T>;


/* Misc concepts */
template<class Left, class Right>
concept AssingableBy = std::assignable_from<Left, Right>;

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
concept Integer = Integral<T> && !Equal<std::remove_cv_t<T>, bool>;

template<class T>
concept Fundamental = std::is_fundamental_v<T>;