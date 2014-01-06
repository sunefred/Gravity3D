//==============================================================================
//	
//	Mixed math functions
//
//==============================================================================


#ifndef MEM_GLOBALS_H
#define MEM_GLOBALS_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"
#include "MemVector.h"
#include "MemMatrix.h"
#include "MemQuaternion.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE



//-- static constants ----------------------------------------------------------

const float PI = 3.14159265359f; // single has only seven digit precision
const float TWOPI = 6.28318530718f;
const float HALFPI = 1.570796326795f;
const float RAD2DEG = 180.0f/PI;
const float DEG2RAD = PI/180.0f;

const float POSINFF =  std::numeric_limits<float>::max();
const float NEGINFF = -std::numeric_limits<float>::max();


//-- min, max, clamp -----------------------------------------------------------
template<typename T>
T min( const T a, const T b )
{
	return a<b ? a : b;
}

template<typename T>
T max( const T a, const T b )
{
	return a<b ? b : a;
}

template<typename T>
T clamp( const T v, const T lo, const T hi )
{
	return v<lo ? lo : ( v>hi ? hi : v );
}


//-- sign ----------------------------------------------------------------------

template<typename T>
signed int signum( const T& a )
{
	if ( a < T(0) )
	{
		return -1;
	}
	else if ( a > T(0) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//-- comparators ---------------------------------------------------------------

template<typename T>
bool iszero( const T& v, const T& tol )
{
	return ( fabs( v ) < tol );
}

template<typename T>
bool signequal( const T& a, const T& b )
{
	return signum<T>( a ) == signum<T>( b );
}

template<typename T>
bool signnequal( const T& a, const T& b )
{
	return !signequal<T>( a, b );
}


//-- mesh utility --------------------------------------------------------------

Vec3f calcNormal( const Vec3f& v0, const Vec3f& v1, const Vec3f& v2 );

// for instance: http://www.terathon.com/code/tangent.html
Vec3f calcTangent( const Vec3f& v0, const Vec3f& v1,
					const Vec3f& v2, const Vec2f& c0,
					const Vec2f& c1, const Vec2f& c2 );

Vec3f calcBiNormal( const Vec3f& v0, const Vec3f& v1,
					const Vec3f& v2, const Vec2f& c0,
					const Vec2f& c1, const Vec2f& c2 );

std::vector<unsigned int>
findNeighbours( const std::vector<Vec3ui>& tri,
				unsigned int idx,
				unsigned int level );

//-- statistics ----------------------------------------------------------------

Vec3f meanVec3f( const std::vector<Vec3f>& X );

Vec3f meanVec3f( const std::vector<Vec3f>& X,
				 const std::vector<unsigned int>& idx );

Vec3f varVec3f( const std::vector<Vec3f>& X );

Vec3f varVec3f( const std::vector<Vec3f>& X,
				const std::vector<unsigned int>& idx,
				const Mat4f& M=Mat4f(1,0,0,0,
									 0,1,0,0,
									 0,0,1,0,
									 0,0,0,1) );

Vec3f stdVec3f( const std::vector<Vec3f>& X );

Vec3f stdVec3f( const std::vector<Vec3f>& X,
				const std::vector<unsigned int>& idx,
				const Mat4f& M=Mat4f(1,0,0,0,
									 0,1,0,0,
									 0,0,1,0,
									 0,0,0,1) );


//------------------------------------------------------------------------------

// Calculate affine transform between two points sets, link for details
// http://homepages.inf.ed.ac.uk/rbf/MY_DAI_OLD_FTP/rp765.pdf
Mat4f hornMethodQuat( const std::vector<Vec3f>& p0,
					  const std::vector<Vec3f>& p1 );

Mat4f hornMethodQuat( const std::vector<Vec3f>& p0,
					  const std::vector<Vec3f>& p1,
					  const std::vector<unsigned int>& idx );

//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================