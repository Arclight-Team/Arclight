/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowRendererGL.hpp
 */

#pragma once

#include <memory>



class Window;
class WindowHandle;
class WindowRendererGLHandle;

class WindowRendererGL {

public:

	WindowRendererGL();
	~WindowRendererGL();

    bool create(Window& window);
    void destroy();

    void makeCurrent();
    static void clearCurrent();

    void swapBuffers();
    void swapInterval(int interval);
    int swapInterval();

private:

    std::unique_ptr<WindowRendererGLHandle> handle;
    std::weak_ptr<WindowHandle> window;

};
