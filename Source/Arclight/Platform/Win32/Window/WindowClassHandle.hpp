/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowClassHandle.hpp
 */

#pragma once

#include <string>



class WindowClass;

class WindowClassHandle {

public:

    constexpr const std::wstring& getWName() const { return wname; }

private:

    std::string name;
    std::wstring wname;

    friend WindowClass;

};
