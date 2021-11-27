#pragma once

#include "arcconfig.h"

#ifndef ARC_FINAL_BUILD

#include "util/arcdebug.h"


struct Class {

    Class()                         { ArcDebug() << "Constructed" << this; }
    ~Class()                        { ArcDebug() << "Destructed" << this; }
    Class(const Class&)             { ArcDebug() << "Copy-Constructed" << this; }
    Class(Class&&)                  { ArcDebug() << "Move-Constructed" << this; }
    Class& operator=(const Class&)  { ArcDebug() << "Copy-Assigned" << this; return *this; }
    Class& operator=(Class&&)       { ArcDebug() << "Move-Assigned" << this; return *this; }

};

#endif