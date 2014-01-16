//==============================================================================
//	
//	Prerequisite dependencies for the entire Gem library in one central location
//
//==============================================================================

#ifndef GEM_PREREQUISITES_H
#define GEM_PREREQUISITES_H

//== GLOBAL INCLUDES ===========================================================

// cpp I/O library
#include <iostream>	// cin, cout, cerr, clog
#include <istream>	// istream, iostream, ostream, streambuf
#include <fstream>	// ifstream, fstream, ofstream, filebuf
#include <sstream>	// istringstream, stringstream, ostringstream, stringbuf

// cpp containers
#include <string>	// string
#include <vector>	// vector
#include <map>		// map?

// cpp manipulators
#include <iomanip> // setprecision, 

// cpp limits
#include <limits>	// numeric_limits<int>::max()

// cpp algorithms
#include <algorithm> // for_each, sort, unique

// c libraries
#include <cassert>	// assert
#include <cmath>	// pow, sqrt, sin, cos etc in std namespace
#include <ctime>	// time, gmtime, localtime in std namespace

// GL stuff
//#include <GLXW/glxw.h>
//#include <GL/gl_core_3_2.h>
#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include <Cg/cgGL.h>

// Everything math, vectors, matrices, operations on such
#include <Mem.h>	// Vec3f, Mat4f, Quatf, etc


//== DISABLE WARNINGS ==========================================================


// disable: deprecation warnings when using CRT calls in VC8
// warning C4996: 'fopen': This function or variable may be unsafe.
// Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
#	pragma warning (disable : 4996)

// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "conversion from 'size_t' to 'unsigned int', possible loss of data
#   pragma warning (disable : 4267)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable : conversion from 'unsigned int' to 'GLvoid *' of greater size
#	pragma warning (disable : 4312)

// disable "alignment changed after including header, may be due to missing
//			#pragma pack(pop)
#	pragma warning (disable : 4103)


//== GLOBAL DEFINES ============================================================

#define GEM_BEGIN_NAMESPACE namespace Gem {
#define GEM_END_NAMESPACE	}


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE

/** 2-element vectors */
using Mem::Vec2i;
using Mem::Vec2ui;
using Mem::Vec2f;
using Mem::Vec2d;

/** 3-element vectors */
using Mem::Vec3i;
using Mem::Vec3ui;
using Mem::Vec3f;
using Mem::Vec3d;

/** 4-element vectors */
using Mem::Vec4i;
using Mem::Vec4ui;
using Mem::Vec4f;
using Mem::Vec4d;

/** 2x2-element matrices */
using Mem::Mat2i;
using Mem::Mat2ui;
using Mem::Mat2f;
using Mem::Mat2d;

/** 3x3-element matrices */
using Mem::Mat3i;
using Mem::Mat3ui;
using Mem::Mat3f;
using Mem::Mat3d;

/** 4x4-element matrices */
using Mem::Mat4i;
using Mem::Mat4ui;
using Mem::Mat4f;
using Mem::Mat4d;

/** only floating point Quaternions makes any sense */
using Mem::Quatf;
using Mem::Quatd;

/** only floating point Polynomials */
using Mem::Polyf;
using Mem::Polyd;


//== LOCAL DEFINES =============================================================

// HOW TO USE ERROR MACROS
//
//	CONSOLE:	Notification to user, preceded by timestamp
//	WARNING:	Issues a textual warning but does not break program flow.
//				Use anywhere.
//	ERROR:		Issues a textual warning and RETURNS from the function.
//				Make sure you CLEAN UP before calling ERROR.
//				Use in user reachable functions. Otherwise you throw errors.
//	THROW:		For private functions, any error. catch in public functions
//				and handle with ERROR
//
#define GEM_CONSOLE( msg ) \
{ \
	std::time_t rawtime; \
	std::tm* timeinfo; \
	std::time( &rawtime ); \
	timeinfo = std::localtime( &rawtime ); \
	std::cout << std::setfill('0') << std::setw( 2 ) \
			  << timeinfo->tm_hour << ':' \
			  << std::setw( 2 ) << timeinfo->tm_min << ':' \
			  << std::setw( 2 ) << timeinfo->tm_sec << "> " \
			  << msg << '\n'; \
}
#ifdef _MSC_VER
#define GEM_WARNING( msg ) \
{ \
	std::cout	<< std::string( 80, '-' ) \
				<< "WARNING: " << __FUNCTION__ << "():\n\n" \
				<< msg << '\n' \
				<< std::string( 80, '-' ); \
}
#else
#define GEM_WARNING( msg ) \
{ \
	std::cout	<< std::string( 80, '-' ) \
				<< "WARNING: " << __func__ << "():\n\n" \
				<< msg << '\n' \
				<< std::string( 80, '-' ); \
}
#endif
#ifdef _MSC_VER
#define GEM_ERROR( msg ) \
{ \
	std::cout	<< std::string( 80, '-' ) \
				<< "ERROR: " << __FUNCTION__ << "():\n\n" \
				<< msg << '\n' \
				<< std::string( 80, '-' ); \
	return; \
}
#else
#define GEM_ERROR( msg ) \
{ \
	std::cout	<< std::string( 80, '-' ) \
				<< "ERROR: " << __func__ << "():\n\n" \
				<< msg << '\n' \
				<< std::string( 80, '-' ); \
	return; \
}
#endif
#define GEM_THROW( msg ) \
{ \
	throw( std::runtime_error( std::string( msg ) ) ); \
}


// global constants
#define MAX_STR_LENGTH 255
#define MAX_VERTEX_ATTRIBUTES 16
#define MAX_TEXTURE_UNITS 32
#define MAX_MIP_LEVELS 16
#define MAX_FRAMEBUFFER_ATTACHMENTS 6 // 4 color, depth, stencil
#define MAX_TRANSFORMFEEDBACK_ATTACHMENTS 8

// still want to be able to use NULL when stdio.h is removed
#ifndef NULL
#define NULL 0
#endif


// Ripped from ddraw.h
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
			((unsigned int)(ch0) | ((unsigned int)(ch1) << 8) | \
			((unsigned int)(ch2) << 16) | ((unsigned int)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)


enum ALLOC_FORMAT
{
	ALLOC_FORMAT_NONE,
	// scalar formats
	ALLOC_FORMAT_SCALAR_8I,
	ALLOC_FORMAT_SCALAR_8UI,
	ALLOC_FORMAT_SCALAR_32I,
	ALLOC_FORMAT_SCALAR_32UI,
	ALLOC_FORMAT_SCALAR_32F,
	// vector formats
	ALLOC_FORMAT_VEC2_8I,
	ALLOC_FORMAT_VEC2_8UI,
	ALLOC_FORMAT_VEC2_32I,
	ALLOC_FORMAT_VEC2_32UI,
	ALLOC_FORMAT_VEC2_32F,
	ALLOC_FORMAT_VEC3_8I,
	ALLOC_FORMAT_VEC3_8UI,
	ALLOC_FORMAT_VEC3_32I,
	ALLOC_FORMAT_VEC3_32UI,
	ALLOC_FORMAT_VEC3_32F,
	ALLOC_FORMAT_VEC4_8I,
	ALLOC_FORMAT_VEC4_8UI,
	ALLOC_FORMAT_VEC4_32I,
	ALLOC_FORMAT_VEC4_32UI,
	ALLOC_FORMAT_VEC4_32F,
	// matrix formats
	ALLOC_FORMAT_MAT2_8I,
	ALLOC_FORMAT_MAT2_8UI,
	ALLOC_FORMAT_MAT2_32I,
	ALLOC_FORMAT_MAT2_32UI,
	ALLOC_FORMAT_MAT2_32F,
	ALLOC_FORMAT_MAT3_8I,
	ALLOC_FORMAT_MAT3_8UI,
	ALLOC_FORMAT_MAT3_32I,
	ALLOC_FORMAT_MAT3_32UI,
	ALLOC_FORMAT_MAT3_32F,
	ALLOC_FORMAT_MAT4_8I,
	ALLOC_FORMAT_MAT4_8UI,
	ALLOC_FORMAT_MAT4_32I,
	ALLOC_FORMAT_MAT4_32UI,
	ALLOC_FORMAT_MAT4_32F,
	// compressed texture formats
	ALLOC_FORMAT_VEC3_DXT1,
	ALLOC_FORMAT_VEC3_DXT3,
};

enum ALLOC_DIM
{
	ALLOC_DIM_NONE,
	ALLOC_DIM_SCALAR,
	ALLOC_DIM_VEC2,
	ALLOC_DIM_VEC3,
	ALLOC_DIM_VEC4,
	ALLOC_DIM_MAT2,
	ALLOC_DIM_MAT3,
	ALLOC_DIM_MAT4,
};

enum ALLOC_TYPE
{
	ALLOC_TYPE_NONE,
	ALLOC_TYPE_8I,
	ALLOC_TYPE_8UI,
	ALLOC_TYPE_32I,
	ALLOC_TYPE_32UI,
	ALLOC_TYPE_32F,
	ALLOC_TYPE_DXT1,
};

enum PRIM_TYPE
{
	PRIM_TYPE_NONE					= ALLOC_FORMAT_NONE,
	PRIM_TYPE_POINT					= ALLOC_FORMAT_SCALAR_32UI,
	PRIM_TYPE_LINE					= ALLOC_FORMAT_VEC2_32UI,
	PRIM_TYPE_TRIANGLE				= ALLOC_FORMAT_VEC3_32UI,
	PRIM_TYPE_QUAD					= ALLOC_FORMAT_VEC4_32UI
};

enum VERTEX_FORMAT
{
	VERTEX_FORMAT_NONE				= ALLOC_FORMAT_NONE,
	VERTEX_FORMAT_X_32UI			= ALLOC_FORMAT_SCALAR_32UI,
	VERTEX_FORMAT_X_32F				= ALLOC_FORMAT_SCALAR_32F,
	VERTEX_FORMAT_XY_32UI			= ALLOC_FORMAT_VEC2_32UI,
	VERTEX_FORMAT_XY_32F			= ALLOC_FORMAT_VEC2_32F,
	VERTEX_FORMAT_XYZ_32UI			= ALLOC_FORMAT_VEC3_32UI,
	VERTEX_FORMAT_XYZ_32F			= ALLOC_FORMAT_VEC3_32F,
	VERTEX_FORMAT_XYZW_32UI			= ALLOC_FORMAT_VEC4_32UI,
	VERTEX_FORMAT_XYZW_32F			= ALLOC_FORMAT_VEC4_32F
};

enum TEXTURE_FORMAT
{
	TEXTURE_FORMAT_NONE				= ALLOC_FORMAT_NONE,
	TEXTURE_FORMAT_R_8UI			= ALLOC_FORMAT_SCALAR_8UI,
	TEXTURE_FORMAT_R_32F			= ALLOC_FORMAT_SCALAR_32F,
	TEXTURE_FORMAT_RG_8UI			= ALLOC_FORMAT_VEC2_8UI,
	TEXTURE_FORMAT_RG_32F			= ALLOC_FORMAT_VEC2_32F,
	TEXTURE_FORMAT_RGB_8UI			= ALLOC_FORMAT_VEC3_8UI,
	TEXTURE_FORMAT_RGB_32F			= ALLOC_FORMAT_VEC3_32F,
	TEXTURE_FORMAT_RGBA_8UI			= ALLOC_FORMAT_VEC4_8UI,
	TEXTURE_FORMAT_RGBA_32F			= ALLOC_FORMAT_VEC4_32F,
	TEXTURE_FORMAT_RGB_DXT1			= ALLOC_FORMAT_VEC3_DXT1,
};

enum TEXTURE_UNIT
{
	TEXTURE_UNIT_0,
	TEXTURE_UNIT_1,
	TEXTURE_UNIT_2,
	TEXTURE_UNIT_3,
	TEXTURE_UNIT_4,
	TEXTURE_UNIT_5,
	TEXTURE_UNIT_6,
	TEXTURE_UNIT_7,
	TEXTURE_UNIT_8,
	TEXTURE_UNIT_9,
	TEXTURE_UNIT_10,
	TEXTURE_UNIT_11,
	TEXTURE_UNIT_12,
	TEXTURE_UNIT_13,
	TEXTURE_UNIT_14,
	TEXTURE_UNIT_15,
	TEXTURE_UNIT_16,
	TEXTURE_UNIT_17,
	TEXTURE_UNIT_18,
	TEXTURE_UNIT_19,
	TEXTURE_UNIT_20,
	TEXTURE_UNIT_21,
	TEXTURE_UNIT_22,
	TEXTURE_UNIT_23,
	TEXTURE_UNIT_24,
	TEXTURE_UNIT_25,
	TEXTURE_UNIT_26,
	TEXTURE_UNIT_27,
	TEXTURE_UNIT_28,
	TEXTURE_UNIT_29,
	TEXTURE_UNIT_30,
	TEXTURE_UNIT_31,
};

enum FRAMEBUFFER_ATTACHMENT
{
	FRAMEBUFFER_ATTACHMENT_COLOR0,
	FRAMEBUFFER_ATTACHMENT_COLOR1,
	FRAMEBUFFER_ATTACHMENT_COLOR2,
	FRAMEBUFFER_ATTACHMENT_COLOR3,
	FRAMEBUFFER_ATTACHMENT_DEPTH,
	FRAMEBUFFER_ATTACHMENT_STENCIL,
};

enum FILE_FORMAT
{
	FILE_FORMAT_NONE,
	FILE_FORMAT_OBJ,
	FILE_FORMAT_OBX,
	FILE_FORMAT_BMP,
	FILE_FORMAT_PFM,
	FILE_FORMAT_DDS
};

enum SHADER_TYPE
{
	SHADER_TYPE_NONE,
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_TESSELATION_CONTROL,
	SHADER_TYPE_TESSELATION_EVALUATION,
};

enum DEPTH_FUNC
{
	DEPTH_FUNC_NEVER				= GL_NEVER,
	DEPTH_FUNC_LESS					= GL_LESS,
	DEPTH_FUNC_EQUAL				= GL_EQUAL,
	DEPTH_FUNC_LEQUAL				= GL_LEQUAL,
	DEPTH_FUNC_GREATER				= GL_GREATER,
	DEPTH_FUNC_NOTEQUAL				= GL_NOTEQUAL,
	DEPTH_FUNC_GEQUAL				= GL_GEQUAL,
	DEPTH_FUNC_ALWAYS				= GL_ALWAYS,
};

enum CULL_FACE
{
	CULL_FACE_FRONT					= GL_FRONT,
	CULL_FACE_BACK					= GL_BACK,
	CULL_FACE_FRONT_AND_BACK		= GL_FRONT_AND_BACK,
};

enum POLYGON_MODE
{
	POLYGON_MODE_POINT				= GL_POINT,
	POLYGON_MODE_LINE				= GL_LINE,
	POLYGON_MODE_FILL				= GL_FILL,
};

enum KEYBOARD_KEY
{
	KEYBOARD_KEY_NONE				= 0x00000000,
	KEYBOARD_KEY_A					= 0x00000041,
	KEYBOARD_KEY_B					= 0x00000042,
	KEYBOARD_KEY_C					= 0x00000043,
	KEYBOARD_KEY_D					= 0x00000044,
	KEYBOARD_KEY_E					= 0x00000045,
	KEYBOARD_KEY_F					= 0x00000046,
	KEYBOARD_KEY_G					= 0x00000047,
	KEYBOARD_KEY_H					= 0x00000048,
	KEYBOARD_KEY_I					= 0x00000049,
	KEYBOARD_KEY_J					= 0x0000004A,
	KEYBOARD_KEY_K					= 0x0000004B,
	KEYBOARD_KEY_L					= 0x0000004C,
	KEYBOARD_KEY_M					= 0x0000004D,
	KEYBOARD_KEY_N					= 0x0000004E,
	KEYBOARD_KEY_O					= 0x0000004F,
	KEYBOARD_KEY_P					= 0x00000050,
	KEYBOARD_KEY_Q					= 0x00000051,
	KEYBOARD_KEY_R					= 0x00000052,
	KEYBOARD_KEY_S					= 0x00000053,
	KEYBOARD_KEY_T					= 0x00000054,
	KEYBOARD_KEY_U					= 0x00000055,
	KEYBOARD_KEY_V					= 0x00000056,
	KEYBOARD_KEY_W					= 0x00000057,
	KEYBOARD_KEY_X					= 0x00000058,
	KEYBOARD_KEY_Y					= 0x00000059,
	KEYBOARD_KEY_Z					= 0x0000005A,
	KEYBOARD_KEY_ESCAPE				= 0x01000000
};

enum KEYBOARD_MODIFIER
{
	KEYBOARD_MODIFIER_NONE			= 0x00000000,
	KEYBOARD_MODIFIER_SHIFT			= 0x02000000,
	KEYBOARD_MODIFIER_CTRL			= 0x04000000,
	KEYBOARD_MODIFIER_ALT			= 0x08000000
};

enum MOUSE_BUTTON
{
	MOUSE_BUTTON_NONE				= 0x00000000,
	MOUSE_BUTTON_LEFT				= 0x00000001,
	MOUSE_BUTTON_RIGHT				= 0x00000002,
	MOUSE_BUTTON_MIDDLE				= 0x00000004
};

enum TRANSFORM_MODE
{
	TRANSFORM_MODE_NONE,
	TRANSFORM_MODE_LOCAL,
	TRANSFORM_MODE_PARENT,
	TRANSFORM_MODE_WORLD,
	TRANSFORM_MODE_SCREEN
};


//== CLASS PRE-DEFINITIONS =====================================================

// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files

// Rendering
class RenderState;

// Loaders
class Allocator;
class MeshLoader;
class TextureLoader;
class ShaderLoader;

// Nodes
class TransformNode;
class SceneNode;
class CameraNode;
class LightNode;

// Controllers
class Controller;
class OrbitalController;
class ArcBallController;

// Animators

// Utility
template<typename T> class Tracker;


//== TYPEDEFS ==================================================================

//-- primitive types -----------------------------------------------------------

//typedef signed char int8;
//typedef signed short int16;
//typedef signed int int32;
//typedef signed long int64;
//
//typedef unsigned char uint8;
//typedef unsigned short uint16;
//typedef unsigned int uint32;
//typedef unsigned long uint64;
//
//typedef float float32;
//typedef double float64;
//
//typedef std::string stdstring;


//-- commonly used containers --------------------------------------------------

//typedef std::vector<unsigned int> stdvecui;
//typedef std::vector<Vec2ui> stdvec2ui;
//typedef std::vector<Vec3ui> stdvec3ui;
//typedef std::vector<Vec4ui> stdvec4ui;
//
//typedef std::vector<float> stdvecf;
//typedef std::vector<Vec2f> stdvec2f;
//typedef std::vector<Vec3f> stdvec3f;
//typedef std::vector<Vec4f> stdvec4f;
//typedef std::vector<Mat4f> stdvec4x4f;
//
//typedef std::vector<stdvecui> stdvecvecui;
//typedef std::vector<stdvec2ui> stdvecvec2ui;
//typedef std::vector<stdvec3ui> stdvecvec3ui;
//typedef std::vector<stdvec4ui> stdvecvec4ui;
//
//typedef std::vector<stdvecf> stdvecvecf;
//typedef std::vector<stdvec2f> stdvecvec2f;
//typedef std::vector<stdvec3f> stdvecvec3f;
//typedef std::vector<stdvec4f> stdvecvec4f;
//
//typedef std::vector<TransformNode*> stdvecnodeptr;
//typedef std::pair<TransformNode*,TransformNode*> stdpairnodeptr;
//typedef std::vector<Point> stdvecpoint;


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================

