#pragma once


#define GLE_HEADER	<GL/GLEW.h>
#define GLE_BEGIN	namespace GLE {
#define GLE_END		}

#define GLE_MAJOR 4
#define GLE_MINOR 3
#define GLE_VERSION_MIN(major, minor) ((major) <= GLE_MAJOR) && ((minor) <= GLE_MINOR)

/*
typedef bool	glbool;
typedef s8		gls8;
typedef u8		glu8;
typedef s16		gls16;
typedef u16		glu16;
typedef s32		gls32;
typedef u32		glu32;
typedef s64		gls64;
typedef u64		glu64;
typedef s32		glfix;
typedef u32		glsize;
typedef u32		glenum;
typedef GLintptr	glintptr;
typedef GLsizeiptr	glsizeiptr;
typedef GLsync		glsync;
typedef GLbitfield	glbits;
typedef GLhalf		glhalf;
typedef GLfloat		glfloat;
typedef GLclampf	glfclamp;
typedef GLdouble	gldouble;
typedef GLclampd	gldclamp;
*/


#include <string>

//User includes
#include "util/log.h"
#include "util/assert.h"
#include "types.h"

#define gle_assert(cond, msg, ...) arc_assert(cond, msg, __VA_ARGS__)


GLE_BEGIN

namespace {
	constexpr const char* logSystemName = "GL Engine";
}

template<typename... Args>
static inline void debug(const std::string& msg, Args&&... args) {
	Log::debug(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
static inline void info(const std::string& msg, Args&&... args) {
	Log::info(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
static inline void warn(const std::string& msg, Args&&... args) {
	Log::warn(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
static inline void error(const std::string& msg, Args&&... args) {
	Log::error(logSystemName, msg, std::forward<Args>(args)...);
}


constexpr static inline u32 maxVertexAttributes = 16;
constexpr static inline u32 maxVertexAttributeBytes = (maxVertexAttributes + 7) / 8;

constexpr static inline u32 invalidID = -1;
constexpr static inline u32 invalidBoundID = -2;

GLE_END