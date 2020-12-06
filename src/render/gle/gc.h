#pragma once


#define GLE_HEADER	<GL/GLEW.h>
#define GLE_BEGIN	namespace GLE {
#define GLE_END		}
#define GLE_BEGIN_ENUM(t)	struct t { \
								static u32 get(t x); \
								enum {
#define GLE_END_ENUM			};};

#define GLE_EXT_SUPPORTED(x) GLEW_##x


//#define GLE_DISABLE_DEBUG



#include <string>

//User includes
#include "util/log.h"
#include "util/assert.h"
#include "types.h"

#define GLE_MATRIX_HEADER "util/matrix.h"
#define GLE_DEFINE_MATRIX	typedef Vec2f GLEVec2; \
							typedef Vec3f GLEVec3; \
							typedef Vec4f GLEVec4; \
							typedef Matrix2f GLEMat2; \
							typedef Matrix3f GLEMat3; \
							typedef Matrix4f GLEMat4;

#define GLE_VECTOR_VALUE_PTR(v) (&v.x)
#define GLE_VECTOR_ARRAY_PTR(v) (&v->x)
#define GLE_MATRIX_VALUE_PTR(m) (&m[0].x)
#define GLE_MATRIX_ARRAY_PTR(m) (&(*m)[0].x)

#define gle_assert(cond, msg, ...) arc_assert(cond, msg, __VA_ARGS__)


GLE_BEGIN

namespace {
	constexpr const char* logSystemName = "GL Engine";
}

template<typename... Args>
inline void debug(const std::string& msg, Args&&... args) {
	Log::debug(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(const std::string& msg, Args&&... args) {
	Log::info(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(const std::string& msg, Args&&... args) {
	Log::warn(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(const std::string& msg, Args&&... args) {
	Log::error(logSystemName, msg, std::forward<Args>(args)...);
}


constexpr static inline u32 maxVertexAttributes = 16;
constexpr static inline u32 maxVertexAttributeBytes = (maxVertexAttributes + 7) / 8;

constexpr static inline u32 invalidID = -1;
constexpr static inline u32 invalidBoundID = -2;

GLE_END