#pragma once

#include "types.h"
#include "util/concepts.h"


class IComponent {};
typedef u32 ComponentID;

template<class C>
concept Component = BaseOf<IComponent, std::remove_reference_t<C>>;