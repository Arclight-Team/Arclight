/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowClass.hpp
 */

#include <string>
#include <string_view>
#include <memory>

class WindowClassHandle;

class WindowClass {
public:
    WindowClass();
    ~WindowClass();

    bool create(std::string_view name);
    void destroy();
    const std::string& getName() const;

    WindowClassHandle& getInternalHandle();

private:
    std::unique_ptr<WindowClassHandle> handle;
};
