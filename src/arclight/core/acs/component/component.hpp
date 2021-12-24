#pragma once

#include "util/concepts.hpp"
#include "types.hpp"


class IComponent {};
typedef u32 ComponentID;

template<class C>
concept Component = BaseOf<IComponent, std::remove_reference_t<C>>;