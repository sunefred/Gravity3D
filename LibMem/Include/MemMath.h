//==============================================================================
//	
//	Mixed math functions
//
//==============================================================================


#ifndef MEM_MATH_H
#define MEM_MATH_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"
#include "MemVector.h"
#include "MemMatrix.h"
#include "MemQuaternion.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================


class Math
{
public:

	//-- static constants ------------------------------------------------------

	static const float PI;
	static const float TWOPI;
	static const float HALFPI;
	static const float RAD2DEG;
	static const float DEG2RAD;

	static const float POSINFF;
	static const float NEGINFF;


	//-- constructors ----------------------------------------------------------

	Math( );
	~Math( );


	//-- min, max, clamp -------------------------------------------------------
	template<typename T>
	static T min( const T a, const T b )
	{
		return a<b ? a : b;
	}

	template<typename T>
	static T max( const T a, const T b )
	{
		return a<b ? b : a;
	}

	template<typename T>
	static T clamp( const T v, const T lo, const T hi )
	{
		return v<lo ? lo : ( v>hi ? hi : v );
	}


	//-- sign ------------------------------------------------------------------

	template<typename T>
	static signed int signum( const T& a )
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


	//-- comparators -----------------------------------------------------------

	template<typename T>
	static bool iszero( const T& v, const T& tol )
	{
		return ( fabs( v ) < tol );
	}

	template<typename T>
	static bool signequal( const T& a, const T& b )
	{
		return signum<T>( a ) == signum<T>( b );
	}

	template<typename T>
	static bool signnequal( const T& a, const T& b )
	{
		return !signequal<T>( a, b );
	}


	//-- mesh utility ----------------------------------------------------------

	static Vec3f calcNormal( const Vec3f& v0, const Vec3f& v1,
							 const Vec3f& v2 )
	{
		Vec3f normal;
		
		normal = v1-v0;
		normal = (v1-v0).cross(v2-v0);
		normal.normalize();
		
		return normal;
	}

	// for instance: http://www.terathon.com/code/tangent.html
	static Vec3f calcTangent( const Vec3f& v0, const Vec3f& v1,
							  const Vec3f& v2, const Vec2f& c0,
							  const Vec2f& c1, const Vec2f& c2)
	{
		Vec3f tangent;

		float s1 = c1[0] - c0[0];
		float s2 = c2[0] - c0[0];
		float t1 = c1[1] - c0[1];
		float t2 = c2[1] - c0[1];

		tangent = (1.0f/(s1*t2-s2*t1)) * (t2*(v1-v0) - t1*(v2-v0));
		tangent.normalize();

		return tangent;
	}

	static Vec3f calcBiNormal(const Vec3f& v0, const Vec3f& v1,
							  const Vec3f& v2, const Vec2f& c0,
							  const Vec2f& c1, const Vec2f& c2)
	{
		Vec3f biNormal;

		float t1 = c1[0] - c0[0];
		float t2 = c2[0] - c0[0];
		float b1 = c1[1] - c0[1];
		float b2 = c2[1] - c0[1];

		biNormal = (1.0f/(t1*b2-t2*b1)) * (-t2*(v1-v0) + t1*(v2-v0));
		biNormal.normalize();

		return biNormal;
	}

	std::vector<unsigned int>
	static findNeighbours( const std::vector<Vec3ui>& tri,
						   unsigned int idx,
						   unsigned int level )

	{
		// local vars
		std::vector<unsigned int> nb;
		unsigned int i, ibegin, iend;
		

		// initialize nb and index
		nb.push_back( idx );
		i = 0;


		while ( level > 0 )
		{
			// set range of current ring
			ibegin = i;
			iend = static_cast<unsigned int>(nb.size());


			// loop of current range and add neighbours to the end
			for ( i=ibegin; i!=iend; ++i )
			{
				unsigned int e = nb.at(i);

				for ( std::vector<Vec3ui>::const_iterator
					  jt=tri.begin(), jend=tri.end(); jt!=jend; ++jt )
				{
					if ( e==(*jt)[0] || e==(*jt)[1] || e==(*jt)[2] )
					{
						nb.push_back( (*jt)[0] );
						nb.push_back( (*jt)[1] );
						nb.push_back( (*jt)[2] );
					}
				}
			}


			// decrease level
			level--;
		}


		// remove duplicates (easiest to sort first and use unique)
		std::vector<unsigned int>::const_iterator it;
		std::sort( nb.begin(), nb.end() );
		nb.erase( std::unique( nb.begin(), nb.end() ), nb.end() );


		// return
		return nb;
	}

	//-- statistics ------------------------------------------------------------

	static Vec3f meanVec3f( const std::vector<Vec3f>& X )
	{
		Vec3f u(0,0,0);
		std::vector<Vec3f>::const_iterator it;
		for ( it = X.begin(); it < X.end(); ++it )
		{
			u += (*it);
		}
		u /= static_cast<float>(X.size());
		return u;
	}

	static Vec3f meanVec3f( const std::vector<Vec3f>& X,
							const std::vector<unsigned int>& idx )
	{
		Vec3f u(0,0,0);
		std::vector<unsigned int>::const_iterator it;
		std::vector<Vec3f>::const_iterator Xbegin = X.begin();
		for ( it = idx.begin(); it < idx.end(); ++it )
		{
			u += *(Xbegin + *it);
		}
		u /= static_cast<float>(idx.size());
		return u;
	}

	static Vec3f varVec3f( const std::vector<Vec3f>& X )
	{
		Vec3f v(0,0,0), d;
		Vec3f u = Math::meanVec3f( X );
		std::vector<Vec3f>::const_iterator it;
		for ( it = X.begin(); it < X.end(); ++it )
		{
			d = (*it) - u;
			v += d * d;
		}
		v /= static_cast<float>(X.size());
		return v;
	}

	static Vec3f varVec3f( const std::vector<Vec3f>& X,
						   const std::vector<unsigned int>& idx,
						   const Mat4f& M=Mat4f(1,0,0,0,
												0,1,0,0,
												0,0,1,0,
												0,0,0,1) )
	{
		Vec3f v(0,0,0), d;
		Vec3f u = M * Math::meanVec3f( X, idx );
		std::vector<unsigned int>::const_iterator it;
		std::vector<Vec3f>::const_iterator Xbegin = X.begin();
		for ( it = idx.begin(); it < idx.end(); ++it )
		{
			d = M * (*(Xbegin + *it)) - u;
			v += d * d;
		}
		v /= static_cast<float>(idx.size());
		return v;
	}

	static Vec3f stdVec3f( const std::vector<Vec3f>& X )
	{
		Vec3f s;
		Vec3f v = Math::varVec3f( X );
		s[0] = std::sqrt( v[0] );
		s[1] = std::sqrt( v[1] );
		s[2] = std::sqrt( v[2] );
		return s;
	}

	static Vec3f stdVec3f( const std::vector<Vec3f>& X,
						   const std::vector<unsigned int>& idx,
						   const Mat4f& M=Mat4f(1,0,0,0,
												0,1,0,0,
												0,0,1,0,
												0,0,0,1) )
	{
		Vec3f s;
		Vec3f v = Math::varVec3f( X, idx, M );
		s[0] = sqrt( v[0] );
		s[1] = sqrt( v[1] );
		s[2] = sqrt( v[2] );
		return s;
	}


	//--------------------------------------------------------------------------

	// Calculate affine transform between two points sets, link for details
	// http://homepages.inf.ed.ac.uk/rbf/MY_DAI_OLD_FTP/rp765.pdf
	Mat4f
	static hornMethodQuat( const std::vector<Vec3f>& p0,
						   const std::vector<Vec3f>& p1 )
	{
		// define local vars
		Mat4f A(1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1);
		Mat4f V;
		Vec4f D;
		Vec3f meanp0, meanp1;
		float x0, y0, z0, x1, y1, z1;
		float h00=0, h01=0, h02=0,
			  h10=0, h11=0, h12=0,
			  h20=0, h21=0, h22=0;


		// dummy check
		if ( p0.empty() || p1.empty() )
			return A;

		
		// calculate mean of the two point sets
		meanp0 = Math::meanVec3f( p0 );
		meanp1 = Math::meanVec3f( p1 );


		// calc coeffs of correlation matrix H = (p0-meanp0)' * (p1-meanp1)
		std::vector<Vec3f>::const_iterator it0;
		std::vector<Vec3f>::const_iterator it1;
		for ( it0 = p0.begin(), it1 = p1.begin();
			  it0 < p0.end(), it1 < p1.end(); ++it0, ++it1 )
		{
			x0 = (*it0)[0] - meanp0[0];
			y0 = (*it0)[1] - meanp0[1];
			z0 = (*it0)[2] - meanp0[2];
			x1 = (*it1)[0] - meanp1[0];
			y1 = (*it1)[1] - meanp1[1];
			z1 = (*it1)[2] - meanp1[2];
			h00 += x0*x1;
			h01 += x0*y1;
			h02 += x0*z1;
			h10 += y0*x1;
			h11 += y0*y1;
			h12 += y0*z1;
			h20 += z0*x1;
			h21 += z0*y1;
			h22 += z0*z1;
		}


		// calculate eigenvalues of symmetric matrix.
		Mat4f P( h00+h11+h22,	0,				0,				0,
				 h12-h21,		h00-h11-h22,	0,				0,
				 h20-h02,		h01+h10,		h11-h00-h22,	0,
				 h01-h10,		h20+h02,		h12+h21,		h22-h11-h00 );
		P.eigsym( &V, &D );


		// get rotation from first eigen vector
		Quatf Q( V(0,3), V(1,3), V(2,3), V(3,3) );
		Q.normalize();


		// calculate and return rigid transformation matrix
		A.fromPosScaleRot( meanp1 - Q*meanp0, Vec3f(1,1,1), Q );
		return A;
	}

	Mat4f
	static hornMethodQuat( const std::vector<Vec3f>& p0,
						   const std::vector<Vec3f>& p1,
						   const std::vector<unsigned int>& idx )
	{
		// define local vars
		Mat4f A(1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1);
		Mat4f V;
		Vec4f D;
		Vec3f meanp0, meanp1;
		float x0, y0, z0, x1, y1, z1;
		float h00=0, h01=0, h02=0,
			  h10=0, h11=0, h12=0,
			  h20=0, h21=0, h22=0;


		// dummy check
		if ( p0.empty() || p1.empty() )
			return A;

		
		// calculate mean of the two point sets
		meanp0 = Math::meanVec3f( p0, idx );
		meanp1 = Math::meanVec3f( p1, idx );


		// calc coeffs of correlation matrix H = (p0-meanp0)' * (p1-meanp1)
		std::vector<Vec3f>::const_iterator p0begin = p0.begin();
		std::vector<Vec3f>::const_iterator p1begin = p1.begin();
		std::vector<unsigned int>::const_iterator it;
		for ( it = idx.begin(); it < idx.end(); ++it )
		{
			x0 = (*(p0begin + *it))[0] - meanp0[0];
			y0 = (*(p0begin + *it))[1] - meanp0[1];
			z0 = (*(p0begin + *it))[2] - meanp0[2];
			x1 = (*(p1begin + *it))[0] - meanp1[0];
			y1 = (*(p1begin + *it))[1] - meanp1[1];
			z1 = (*(p1begin + *it))[2] - meanp1[2];
			h00 += x0*x1;
			h01 += x0*y1;
			h02 += x0*z1;
			h10 += y0*x1;
			h11 += y0*y1;
			h12 += y0*z1;
			h20 += z0*x1;
			h21 += z0*y1;
			h22 += z0*z1;
		}


		// calculate eigenvalues of symmetric matrix.
		Mat4f P( h00+h11+h22,	0,				0,				0,
				 h12-h21,		h00-h11-h22,	0,				0,
				 h20-h02,		h01+h10,		h11-h00-h22,	0,
				 h01-h10,		h20+h02,		h12+h21,		h22-h11-h00 );
		P.eigsym( &V, &D );


		// get rotation from first eigen vector
		Quatf Q( V(0,3), V(1,3), V(2,3), V(3,3) );
		Q.normalize();


		// calculate and return rigid transformation matrix
		A.fromPosScaleRot( meanp1 - Q*meanp0, Vec3f(1,1,1), Q );
		return A;
	}

protected:
private:

	//-- private variables -----------------------------------------------------

private:


};


//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================