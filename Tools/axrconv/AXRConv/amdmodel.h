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
#define AMD_ATTR_IS_COLOR(x) ((static_cast<u8>(x) - static_cast<u8>(AMDAttributeType::Color0)) < 8)
#define AMD_ATTR_IS_UV(x) ((static_cast<u8>(x) - static_cast<u8>(AMDAttributeType::Uv0)) < 8)


constexpr static u8 amdMajorVersion = 0;
constexpr static u8 amdMinorVersion = 1;


enum class AMDPrimitiveMode {
	Points,
	Lines,
	Triangles
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



enum class AMDBlendMode {
	Solid,
	AlphaDiscard,
	Transparent
};



enum class AMDShading {
	NoLighting,
	Gouraud,
	Phong,
	BlinnPhong
};



enum class AMDTextureMapping {
	Repeat,
	Clamp,
	Mirror
};



enum class AMDTextureType {
	Undefined,
	Ambient,
	Diffuse,
	Specular,
	Emissive,
	Shininess,
	Normal,
	Alpha,
	Lightmap,
	Displacement,
	Reflection
};


enum class AMDTextureFormat {
	RGB565,
	RGB888,
	RGBA8888
};


struct AMDVector3 {
	float x, y, z;
};


struct AMDAttribute {

	AMDAttributeType type;
	u8 dataType;

};



struct AMDTextureFlags {

	enum {
		GenerateMipmaps = 0x1
	};

};



enum class AMDTextureMagFiltering {
	None,
	Linear
};



enum class AMDTextureMinFiltering {
	None,
	Linear,
	NoneWithMipmapNone,
	LinearWithMipmapNone,
	NoneWithMipmapLinear,
	LinearWithMipmapLinear
};



struct AMDTexture {

	//Add 2D array + cube map support

	std::string name;   //Actual texture name (.atx)
	u32 flags;
	u32 width;
	u32 height;
	AMDTextureType type;
	AMDTextureFormat format;
	AMDTextureMapping mappingU;
	AMDTextureMapping mappingV;
	AMDTextureMagFiltering filterMag;
	AMDTextureMinFiltering filterMin;
	bool loaded;

	std::vector<u8> data;

};



struct AMDMaterialFlags {

	enum {
		DisableCulling = 0x1
	};

};



struct AMDMaterial {

	std::string name;
	bool shared;        //True if externally defined (.aml)

	u32 flags;
	AMDBlendMode blendMode;
	AMDVector3 ambientColor;
	AMDVector3 diffuseColor;
	AMDVector3 specularColor;
	AMDVector3 emissiveColor;
	AMDVector3 transparentColor;
	AMDVector3 reflectiveColor;
	float shininess;
	float specularFactor;
	float reflectivity;
	float refractionIndex;
	float alphaThreshold;

	//Add standard shading modes (+custom)

	std::vector<u32> textureIDs;
	std::vector<u8> uvChannels;

};


struct AMDMesh {

	u32 vertexCount;
	AMDPrimitiveMode primType;
	bool indexed;
	u32 materialID;
    std::string name;
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

	std::vector<AMDTexture> textures;
	std::vector<AMDMaterial> materials;
	std::vector<AMDMesh> meshes;
	std::unordered_map<u16, AMDNode> nodes;

};



#endif // AMDMODEL_H
