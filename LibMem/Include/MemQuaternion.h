//==============================================================================
//	
//	Quaternion implementation
//
//==============================================================================

#ifndef MEM_QUATERNION_H
#define MEM_QUATERNION_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================


template<typename T>
class Quaternion
{
public:
	
	//-- static constants ------------------------------------------------------

	static const Quaternion<T> ZERO;
	static const Quaternion<T> IDENTITY;


	//-- constructors ----------------------------------------------------------

	// default constructor, creates uninitialized values
	Quaternion() {};

	// compiler generated destructor is ok.

	// compiler generated copy constructor is ok.

	// compiler generated assignment operator is ok.


	// create quaternion from scalar values
	Quaternion( const T& w, const T& x, const T& y, const T& z )
	{
		q[0] = w; q[1] = x; q[2] = y; q[3] = z;
		//this->fromAngleAxis( w, Vector<T,3>( x, y, z ) );
	}

	// create quaternion from angle and axis
	Quaternion( const T& angle, const Vector<T,3>& axis )
	{
		//q[0] = angle; q[1] = axis[0]; q[2] = axis[1]; q[3] = axis[2];
		this->fromAngleAxis( angle, axis );
	}

	// create quaternion from euler angles assuming xyz rotation order
	Quaternion( const T& anglex, const T& angley, const T& anglez )
	{
		this->fromEulerAngles( anglex, angley, anglez );
	}


	//-- access operators ------------------------------------------------------

	// access to element through q[i], read-write
	T& operator[] ( const int i )
	{
		assert( i>=0 && i<4 );
		return q[i];
	}

	// access to element through v[i], read-only
	const T& operator[] ( const int i ) const
	{
		assert( i>=0 && i<4 );
		return q[i];
	}

	// access to element through v(i), read-write
	T& operator() ( const int i )
	{
		assert( i>=0 && i<4 );
		return q[i];
	}

	// access to element through v(i), read-only
	const T& operator() ( const int i ) const
	{
		assert( i>=0 && i<4 );
		return q[i];
	}
	
	
	//-- comparison operators --------------------------------------------------
	
	// component-wise equality
	bool operator== ( const Quaternion<T>& rhs ) const
	{
		for ( int i=0; i<4; ++i )
		{
			if ( q[i] != rhs.q[i] ) return false;
		}
		return true;
	}

	// component-wise non-equality
	bool operator!= ( const Quaternion<T>& rhs ) const
	{
		return !( *this == rhs );
	}


	//-- typecast operators ----------------------------------------------------

	//-- unary operators -------------------------------------------------------

	//-- quaternion-scalar operators -------------------------------------------
	
	// self-multiplication with scalar
	Quaternion<T>& operator*= ( const T& rhs  )
	{
		q[0] *= rhs;
		q[1] *= rhs;
		q[2] *= rhs;
		q[3] *= rhs;
		return *this;
	}

	// self-divsion with scalar
	Quaternion<T>& operator/= ( const T& rhs  )
	{
		q[0] /= rhs;
		q[1] /= rhs;
		q[2] /= rhs;
		q[3] /= rhs;
		return *this;
	}

	// multiplication with scalar
	Quaternion<T> operator* ( const T& rhs ) const
	{
		return Quaternion<T>(*this) *= rhs;
	}

	// multiplication with scalar left hand version
	friend Quaternion<T> operator* ( const T& lhs, const Quaternion<T>& rhs )
	{
		return Quaternion<T>(rhs) *= lhs;
	}

	// division with scalar
	Quaternion<T> operator/ ( const T& rhs ) const
	{
		return Quaternion<T>(*this) /= rhs;
	}



	//-- quaternion-quaternion operators ---------------------------------------

	// self-addition
	Quaternion<T>& operator+= ( const Quaternion<T>& rhs )
	{
		q[0] += rhs.q[0];
		q[1] += rhs.q[1];
		q[2] += rhs.q[2];
		q[3] += rhs.q[3];
		return *this;
	}

	// self-subtraction
	Quaternion<T>& operator-= ( const Quaternion<T>& rhs )
	{
		q[0] -= rhs.q[0];
		q[1] -= rhs.q[1];
		q[2] -= rhs.q[2];
		q[3] -= rhs.q[3];
		return *this;
	}

	// self-multiplication, Hamilton-product, (non commutative) 
	Quaternion<T>& operator*= ( const Quaternion<T>& rhs )
	{
		// q1 = [s1 v1], q2 = [s2 v2]
		// q1 * q2 = [s1*s2-v1.v2 s1*v2+s2*v1+v1xv2]
		T q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];
		T r0 = rhs.q[0], r1 = rhs.q[1], r2 = rhs.q[2], r3 = rhs.q[3];
		q[0] = q0*r0 - q1*r1 - q2*r2 - q3*r3;
		q[1] = q0*r1 + q1*r0 + q2*r3 - q3*r2;
		q[2] = q0*r2 + q2*r0 + q3*r1 - q1*r3;
		q[3] = q0*r3 + q3*r0 + q1*r2 - q2*r1;
		return *this;
	}

	// addition of quaternions
	Quaternion<T> operator+ ( const Quaternion<T>& rhs ) const
	{
		return Quaternion<T>(*this) += rhs;
	}

	// subtraction of quaternions
	Quaternion<T> operator- ( const Quaternion<T>& rhs ) const
	{
		return Quaternion<T>(*this) -= rhs;
	}

	// multiplication of quaternions (non commutative)
	Quaternion<T> operator* ( const Quaternion<T>& rhs ) const
	{
		return Quaternion<T>(*this) *= rhs;
	}



	//-- norm, conjugate, inverse ----------------------------------------------

	// norm
	T norm( ) const
	{
		return q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
	}

	// normalize quaternion and return normalized quaternion. no /0 check
	Quaternion<T>& normalize( )
	{
		T one(1);
		operator*= ( one / sqrt( this->norm() ) );
		return *this;
	}

	// conjugate
	Quaternion<T> conjugate( )
	{
		return Quaternion<T>( q[0], -q[1], -q[2], -q[3] );
	}

	// inverse. no /0 check
	Quaternion<T> inverse( )
	{
		T one(1);
		return this->conjugate() * ( one / this->norm() );
	}


	//-- dot product -----------------------------------------------------------

	// dot-product operator
	T operator| ( const Quaternion<T>& rhs ) const
	{
		T r(0);
		r = q[0]*rhs.q[0] + q[1]*rhs.q[1] + q[2]*rhs.q[2] + q[3]*rhs.q[3];
		return r;
	}

	// dot-prduct function
	T dot( const Quaternion<T>& rhs ) const
	{
		return *this | rhs;
	}


	//-- interpolators ---------------------------------------------------------

	// self-lerp
	//Quaterntion<T> lerp( const Quaternion<T>& rhs )
	//{
	//}

	// linear interpolation
	static Quaternion<T> lerp( const Quaternion<T>& q0,
							   const Quaternion<T>& q1,
							   const T& weight )
	{
		Quaternion<T> r;
		r[0] = weight * q0[0] + (1-weight) * q1[0];
		r[1] = weight * q0[1] + (1-weight) * q1[1];
		r[2] = weight * q0[2] + (1-weight) * q1[2];
		r[3] = weight * q0[3] + (1-weight) * q1[3];
		return r;
	}
	
	// normalized lerp, commutative, NOT constant velocity, torque minimal
	static Quaternion<T> nlerp( const Quaternion<T>& q0,
							    const Quaternion<T>& q1,
							    const T& weight )
	{
		return lerp( q0, q1, weight ).normalize();
	}

	// spherical lerp, NOT commutative, constant velocity, torque minimal
	// note that q0, q1 are assumed to be normalized
	// 1. dot product
	// 2. check for linear dependance
	// 3. calculate angle theta from q0 to r
	// 4. gram-schmidt orthogonalize
	// 5. return interpolated vector
	static Quaternion<T> slerp( const Quaternion<T>& q0,
							    const Quaternion<T>& q1,
							    const T& weight )
	{
		T one(1), tol(9999/10000);
		T q0dotq1 = q0.dot( q1 );
		if ( q0dotq1 > tol )
		{
			return nlerp( q0, q1, weight );
		}
		q0dotq1 = q0dotq1<-one ? -one : ( q0dotq1>one ? one : q0dotq1 );
		T theta = ( 1 - weight ) * acos( q0dotq1 );
		Quaternion<T> q2 = (q1 - q0dotq1 * q0).normalize();
		return q0 * cos( theta ) + q2 * sin( theta );;
	}

	//-- R^3 specific functions ------------------------------------------------

	// rotate a vector in R^3, i.e conjugation of vector v: q*v*q^-1
	Vector<T,3> operator*( const Vector<T,3>& v ) const
	{
		// nVidia SDK implementation
		T two(2);
		Vector<T,3> uv, uuv;
		Vector<T,3> qvec( q[1], q[2], q[3] );
		uv = qvec.cross( v );
		uuv = qvec.cross( uv );
		uv *= ( two * q[0] );
		uuv *= two;
		return uuv + uv + v;
    }

	// set quaternion from angle and axis
	void fromAngleAxis( const T& angle, const Vector<T,3>& axis )
	{
		// axis has to be normalized or rotations will fail
		Vector<T,3> axisn = axis;
		axisn.normalize();

		// q = s + v = cos(a/2) + sin(a/2)*(x*i+y*j+z*k)
		T cosa = static_cast<T>( cos( 0.5 * angle ) );
		T sina = static_cast<T>( sin( 0.5 * angle ) );
		q[0] = cosa;
		q[1] = sina * axisn[0];
		q[2] = sina * axisn[1];
		q[3] = sina * axisn[2];
		int breakpoint = 0;
	}

	// get angle and axis from quaternion
	void toAngleAxis( T* angle, Vector<T,3>* axis ) const
	{
		// q = s + v = cos(a/2) + sin(a/2)*(x*i+y*j+z*k)
		T one(1), two(2);
		*angle = two * acos( q[0] );
		T lensqr = q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
		T leninv = one / sqrt( lensqr );
		(*axis)[0] = leninv * q[1];
		(*axis)[1] = leninv * q[2];
		(*axis)[2] = leninv * q[3];
	}

	// get quaternion from euler angles with rotation order xyz,
	// i.e u = Rz * Ry * Rx * v
	// ref: http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290_1977024290.pdf
	void fromEulerAngles( const T& anglex, const T& angley, const T& anglez )
	{
		// temp vars for speed
		T two(2);
		T hx = anglex/two;
		T hy = angley/two;
		T hz = anglez/two;
		T coshx = cos( hx );
		T sinhx = sin( hx );
		T coshy = cos( hy );
		T sinhy = sin( hy );
		T coshz = cos( hz );
		T sinhz = sin( hz );

		// 
		q[0] = coshx*coshy*coshz + sinhx*sinhy*sinhz;
		q[1] = sinhx*coshy*coshz - coshx*sinhy*sinhz;
		q[2] = coshx*sinhy*coshz + sinhx*coshy*sinhz;
		q[3] = coshx*coshy*sinhz - sinhx*sinhy*coshz;
	}

	// get euler angles from quaternion with rotation order xyz
	// (does not handle singularities)
	void toEulerAngles( T* anglex, T* angley, T* anglez )
	{
		T q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];
		*anglex = atan2( 2 * ( q0*q1 + q2*q3 ), 1 - 2 * ( q1*q1 + q2*q2 ) );
		*angley = asin ( 2 * ( q0*q2 - q3*q1 ) ); 
		*anglez = atan2( 2 * ( q0*q3 + q1*q2 ), 1 - 2 * ( q2*q2 + q3*q3 ) );
	}

	// set quaternion from rotation matrix
	void fromRotationMatrix( const Matrix<T,4,4>& rot )
	{
		
	}

	// get rotation matrix from quaternion
	void toRotationMatrix( Matrix<T,4,4>* rot ) const
	{
		T one(1), two(2);
		(*rot)[0][0] = one - two*( q[2]*q[2] + q[3]*q[3] );
		(*rot)[0][1] =		 two*( q[1]*q[2] - q[0]*q[3] );
		(*rot)[0][2] =		 two*( q[1]*q[3] + q[0]*q[2] );
		(*rot)[0][3] = 0;
		(*rot)[1][0] =		 two*( q[1]*q[2] + q[0]*q[3] );
		(*rot)[1][1] = one - two*( q[1]*q[1] + q[3]*q[3] );
		(*rot)[1][2] =		 two*( q[2]*q[3] - q[0]*q[1] );
		(*rot)[1][3] = 0;
		(*rot)[2][0] =		 two*( q[1]*q[3] - q[0]*q[2] );
		(*rot)[2][1] =		 two*( q[2]*q[3] + q[0]*q[1] );
		(*rot)[2][2] = one - two*( q[1]*q[1] + q[2]*q[2] );
		(*rot)[2][3] = 0;
		(*rot)[3][0] = 0;
		(*rot)[3][1] = 0;
		(*rot)[3][2] = 0;
		(*rot)[3][3] = one;
	}

	// get rotation matrix from quaternion
	Matrix<T,4,4> toRotationMatrix() const
	{
		Matrix<T,4,4> rot;
		this->toRotationMatrix( &rot );
		return rot;
	}


private:

	T q[4];
};


//== STATIC CONSTANTS (TEMPLATED) ==============================================

template<typename T>
const Quaternion<T> Quaternion<T>::ZERO(T(0), T(0), T(0), T(0));
template<typename T>
const Quaternion<T> Quaternion<T>::IDENTITY(T(1), T(0), T(0), T(0));


//== TYPEDEFS ==================================================================

/** only floating point Quaternions makes any sense */
typedef Quaternion<float> Quatf;
typedef Quaternion<double> Quatd;



//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================
