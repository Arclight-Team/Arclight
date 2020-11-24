#ifndef AMDMODEL_H
#define AMDMODEL_H

#include "types.h"

#include <vector>
#include <unordered_map>

#define AMD_DATA_TYPE(x, y) (static_cast<u8>(static_cast<u8>(x) | (y - 1) << 6))
#define AMD_ATTR_COLOR(x) (static_cast<AMDAttributeType>(static_cast<u8>(AMDAttributeType::Color0) + x))
#define AMD_ATTR_UV(x) (static_cast<AMDAttributeType>(static_cast<u8>(AMDAttributeType::Uv0) + x))
#define AMD_ATTR_GET_ELEMENTS(x) ((x >> 6) + 1)
#define AMD_ATTR_GET_TYPE(x) (static_cast<AMDDataType>(x & 0x3F))
#define AMD_ATTR_GET_COLOR(x) (static_cast<u8>(x) - static_cast<u8>(AMDAttributeType::Color0))
#define AMD_ATTR_GET_UV(x) (static_cast<u8>(x) - static_cast<u8>(AMDAttributeType::Uv0))


constexpr static u8 amdMajorVersion = 0;
constexpr static u8 amdMinorVersion = 1;


enum class AMDPrimitiveMode {
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Triangles,
    TriangleStrip,
    TriangleFan
};


enum class AMDAttributeType {
    Position,
    Color0,
    Color1,
    Color2,
    Color3,
    Color4,
    Color5,
    Color6,
    Color7,
    Uv0,
    Uv1,
    Uv2,
    Uv3,
    Uv4,
    Uv5,
    Uv6,
    Uv7,
    Normal,
    Tangent,
    Bitangent,
    BoneWeight,
    BoneIndex,
    Custom = 255 - 31
};


enum class AMDDataType {
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    HalfFloat,
    Float,
    Double,
    Fixed,
    Int2_10,
    UInt2_10
};



struct AMDAttribute {

    AMDAttributeType type;
    u8 dataType;

};


struct AMDMesh {

    u32 vertexCount;
    AMDPrimitiveMode primType;
    bool indexed;
    std::vector<AMDAttribute> attributes;
    std::vector<std::vector<u8>> meshData;

};



struct AMDNode {

    std::string name;
    u32 parentID;
    std::vector<u32> childIDs;
    std::vector<u32> meshIDs;

};


struct AMDModel {

    std::vector<AMDMesh> meshes;
    std::unordered_map<u16, AMDNode> nodes;

};



#endif // AMDMODEL_H
