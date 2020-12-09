#pragma once

#include "gc.h"


GLE_BEGIN

class GLObject {

public:

    virtual ~GLObject() = default;

    GLObject(const GLObject& obj) = delete;
    GLObject& operator=(const GLObject& obj) = delete;

    GLObject(GLObject&& obj) {
        id = obj.id;
        obj.id = invalidID;
    }

    GLObject& operator=(GLObject&& obj) {
        id = obj.id;
        obj.id = invalidID;
    }

    constexpr bool isCreated() const {
        return id != invalidID;
    }

    virtual bool create() = 0;
    virtual void destroy() = 0;

protected:

    constexpr GLObject() : id(invalidID) {}

    u32 id;

};

GLE_END