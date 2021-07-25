#pragma once

#include "component.h"


class Collider : public IComponent {

public:

    enum class Type : u8 {
        Box
    };

    constexpr Collider(Type type) : handle(nullptr), type(type) {}

    void* handle;
    Type type;

};