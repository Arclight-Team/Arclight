#ifndef IMPORTCONFIGURATION_H
#define IMPORTCONFIGURATION_H

#include "types.h"


struct ImportConfiguration {

    constexpr ImportConfiguration() : flipUVs(false), genNormals(false),
        smoothNormals(false), maxWeigths(4), genTangents(false) {}

    bool flipUVs;
    bool genNormals;
    bool smoothNormals;
    u32 maxWeigths;
    bool genTangents;

};



#endif // IMPORTCONFIGURATION_H
