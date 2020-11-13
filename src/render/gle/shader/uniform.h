#pragma once

#include "../gc.h"


GLE_BEGIN


class Uniform {

public:

	constexpr Uniform(u32 location) : id(location) {}

	template<typename T>

private:

	u32 id;

};


GLE_END