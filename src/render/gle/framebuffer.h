#pragma once

#include "globject.h"


GLE_BEGIN


class Framebuffer : public GLObject {

public:

    virtual bool create() override;

    void bind();
    void bindRead();
    void bindRender();

    virtual void destroy() override;

    bool isBound();

private:

    static inline u32 boundReadID = invalidBoundID;
    static inline u32 boundDrawID = invalidBoundID;

};


GLE_END