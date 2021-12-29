/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 debug.hpp
 */

#pragma once

#include "arcconfig.hpp"

#ifndef ARC_FINAL_BUILD

#include "util/arcdebug.hpp"


struct Class {

    Class()                         { ArcDebug() << "Constructed" << this; }
    ~Class()                        { ArcDebug() << "Destructed" << this; }
    Class(const Class&)             { ArcDebug() << "Copy-Constructed" << this; }
    Class(Class&&)                  { ArcDebug() << "Move-Constructed" << this; }
    Class& operator=(const Class&)  { ArcDebug() << "Copy-Assigned" << this; return *this; }
    Class& operator=(Class&&)       { ArcDebug() << "Move-Assigned" << this; return *this; }

};

#endif