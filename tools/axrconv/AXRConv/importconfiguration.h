#ifndef IMPORTCONFIGURATION_H
#define IMPORTCONFIGURATION_H

#include "types.h"


struct ImportConfiguration {

    constexpr ImportConfiguration() : flipUVs(false), genNormals(false),
        smoothNormals(false), limitWeigths(false), maxWeigths(1), genTangents(false) {}

    bool flipUVs;
    bool genNormals;
    bool smoothNormals;
    bool limitWeigths;
    u32 maxWeigths;
    bool genTangents;

};



#endif // IMPORTCONFIGURATION_H
