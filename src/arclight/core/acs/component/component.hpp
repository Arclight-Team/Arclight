/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 component.hpp
 */

#pragma once

#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "types.hpp"


class IComponent {};
typedef u32 ComponentID;

template<class C>
concept Component = BaseOf<IComponent, TT::RemoveRef<C>>;