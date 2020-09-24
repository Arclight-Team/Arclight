#pragma once


class Mat4 {

public:


	/*
	inline constexpr void setIdentity() {
		m = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}

	inline constexpr Mat4() {
		setIdentity();
	}


	inline Mat4 getIdentity() const {
		static Mat4 identity = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}

	*/
	double m[16];

};