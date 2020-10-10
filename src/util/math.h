#pragma once



template<typename T>
concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;



namespace Math {

	constexpr double pi = 3.1415926535897932384626434;
	constexpr double e =  2.7182818284590452353602875;
	constexpr double epsilon = 0.00000001;


	constexpr inline double toDegrees(double radians) {
		return radians * 180.0 / pi;
	}

	constexpr inline double toRadians(double degrees) {
		return degrees * pi / 180.0;
	}

}