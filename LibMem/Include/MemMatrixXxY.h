//==============================================================================
//	
//	Do not include this directly, use MemMatrix.h instead
//
//	This a generic matrix class that provide a primary template AND template
//	specializations using the copy/paste specialization paradigm using
//	preprocessor statements. See MemMatrix.h how this is usd.
//
//==============================================================================


#if !defined (S1) && !defined (S2)
// primary template
template<typename T, int S1, int S2>
class Matrix

#elif defined (S1) && defined (S2)
// template specialization on size
template<typename T>
class Matrix<T, S1, S2>
#endif

{

public:

	static const Matrix<T, S1, S2> ZERO;
	static const Matrix<T, S1, S2> IDENTITY;


public:

	//-- constructors ----------------------------------------------------------

	// default constructor, creates uninitialized values
	Matrix() {};

	// constructor that initializes values, special value along diagonal
	Matrix( const T& s, const T& d)
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				if ( i == j )
				{
					m[i][j] = d;
				}
				else
				{
					m[i][j] = s;
				}
			}
		}
	}
	
#if S1==2 && S2==2
	// create initialized values 2x2 matrix
	explicit Matrix( const T& a, const T& b,
					 const T& c, const T& d )
	{
		m[0][0] = a; m[0][1] = b;
		m[1][0] = c; m[1][1] = d;
	}
#endif

#if S1==3 && S2==3
	// create initialized values 3x3 matrix
	explicit Matrix( const T& a, const T& b, const T& c,
					 const T& d, const T& e, const T& f,
					 const T& g, const T& h, const T& i )
	{
		m[0][0] = a; m[0][1] = b; m[0][2] = c;
		m[1][0] = d; m[1][1] = e; m[1][2] = f;
		m[2][0] = g; m[2][1] = h; m[2][2] = i;
	}
#endif

#if S1==4 && S2==4
	// create initialized values for 4x4 matrix
	explicit Matrix( const T& a, const T& b, const T& c, const T& d,
					 const T& e, const T& f, const T& g, const T& h,
					 const T& i, const T& j, const T& k, const T& l,
					 const T& s, const T& t, const T& u, const T& v )
	{
		m[0][0] = a; m[0][1] = b; m[0][2] = c; m[0][3] = d;
		m[1][0] = e; m[1][1] = f; m[1][2] = g; m[1][3] = h;
		m[2][0] = i; m[2][1] = j; m[2][2] = k; m[2][3] = l;
		m[3][0] = s; m[3][1] = t; m[3][2] = u; m[3][3] = v;
	}

	// initializes values from trans + scale + rot
	explicit Matrix( const Vector<T,3>& position,
					 const Vector<T,3>& scale,
					 const Quaternion<T>& rotation )
	{
		this->fromPosScaleRot( position, scale, rotation );
	}
#endif

	// compiler generated destructor is ok.

	// compiler generated copy constructor is ok.

	// compiler generated assignment operator is ok.


	//-- access operators ------------------------------------------------------
	
	// access to row (allows use of m[i][j]), read-write
	T* operator[] ( const int i )
	{
		assert( i>=0 && i<S1 );
		return m[i];
	}

	// access to row vector (allows use of m[i][j]), read-only
	const T* operator[] ( const int i ) const
	{
		assert( i>=0 && i<S1 );
		return m[i];
	}

	// access to element through m(i,j), read-write
	T& operator() ( const int i, const int j )
	{
		assert( i>=0 && i<S1 && j>=0 && j<S2 );
		return m[i][j];
	}

	// access to element through m(i,j), read-only
	const T& operator() ( const int i, const int j ) const
	{
		assert( i>=0 && i<S1 && j>=0 && j<S2 );
		return m[i][j];
	}


	//-- comparison operators --------------------------------------------------

	// component-wise equality
	bool operator== ( const Matrix<T,S1,S2>& rhs ) const
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				if ( m[i][j] != rhs.m[i][j] ) return false;
			}
		}
		return true;
	}

	// component-wise in-equality
	bool operator!= ( const Matrix<T,S1,S2>& rhs ) const
	{
		return !( *this == rhs );
	}


	//-- typecast operators ----------------------------------------------------

	// from Matrix<T,s1,s2> to Matrix<T,s1-1,s2-1>
	operator Matrix<T,S1-1,S2-1>() const
	{
		assert( S1>=2 && S2>=2 );
		Matrix<T,S1-1,S2-1> r;
		for ( int i=0; i<S1-1; ++i )
		{
			for ( int j=0; j<S2-1; ++j )
			{
				r[i][j] = m[i][j];
			}
		}
		return r;
	}

	// from Matrix<T,s1,s2> to Matrix<T,s1+1,s2+1>
	operator Matrix<T,S1+1,S2+1>() const
	{
		assert( S1>=1 && S2>=1 );
		Matrix<T,S1+1,S2+1> r;
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				r[i][j] = m[i][j];
			}
		}
		for ( int i=0; i<S1+1; ++i )
		{
			r[i][S2] = 0;
		}
		for ( int j=0; j<S2+1; ++j )
		{
			r[S1][j] = 0;
		}
		r[S1][S2] = 1;
		return r;
	}


	//-- unary operators -------------------------------------------------------
	
	// unary plus
	Matrix<T,S1,S2> operator+ () const
	{
		return *this;
	}

	// unary minus
	Matrix<T,S1,S2> operator- () const
	{
		Matrix<T,S1,S2> r;
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				r[i][j] = -m[i][j];
			}
		}
		return r;
	}


	//-- matrix-scalar operators -----------------------------------------------

	// self-multiplication with scalar
	Matrix<T,S1,S2>& operator*= ( const T& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				m[i][j] *= rhs;
			}
		}
		return *this;
	}

	// self-division with scalar
	Matrix<T,S1,S2>& operator/= ( const T& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				m[i][j] /= rhs;
			}
		}
		return *this;
	}

	// multiplication with scalar
	Matrix<T,S1,S2> operator* ( const T& rhs )
	{
		return Matrix<T,S1,S2>(*this) *= rhs;
	}

	// multiplication with scalar left hand version
	friend Matrix<T,S1,S2> operator*( const T& lhs, const Matrix<T,S1,S2>& rhs )
	{
		return Matrix<T,S1,S2>(*rhs) *= lhs;
	}

	// division with scalar
	Matrix<T,S1,S2> operator/ ( const T& rhs )
	{
		return Matrix<T,S1,S2>(*this) /= rhs;
	}


	//-- matrix-vector operators -----------------------------------------------

	// multiplication with vector
	Vector<T,S1> operator* ( const Vector<T,S2>& rhs ) const
	{
		Vector<T,S1> r;
		for ( int i=0; i<S1; ++i )
		{
			r[i] = 0;
			for ( int j=0; j<S2; ++j )
			{
				r[i] += m[i][j] * rhs[j];
			}
		}
		return r;
	}

	// multiplication of a square matrix with a smaller vector, last element
	// in vector is assumed to be 1. useful for Vec3f = Mat4f * Vec3f without
	// having to use typecast vectors
	Vector<T,S2-1> operator* ( const Vector<T,S1-1>& rhs ) const
	{
		Vector<T,S2-1> r;
		for ( int i=0; i<S1-1; ++i )
		{
			r[i] = m[i][S2-1];
			for ( int j=0; j<S2-1; ++j )
			{
				r[i] += m[i][j] * rhs[j];
			}
		}
		return r;
	}



	//-- matrix-matrix operators -----------------------------------------------

	// self-addition of matrices
	Matrix<T,S1,S2>& operator+= ( const Matrix<T,S1,S2>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				m[i][j] = m[i][j] + rhs.m[i][j];
			}
		}
		return *this;
	}

	// self-subtraction of matrices
	Matrix<T,S1,S2>& operator-= ( const Matrix<T,S1,S2>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				m[i][j] = m[i][j] - rhs.m[i][j];
			}
		}
		return *this;
	}

	// addition of matrices
	Matrix<T,S1,S2> operator+ ( const Matrix<T,S1,S2>& rhs )
	{
		return Matrix<T,S1,S2>(*this) += rhs;
	}

	// subtraction of matrices
	Matrix<T,S1,S2> operator- ( const Matrix<T,S1,S2>& rhs )
	{
		return Matrix<T,S1,S2>(*this) -= rhs;
	}

	// multiplication of matrices (so far only square matrices)
	Matrix<T,S1,S1> operator* ( const Matrix<T,S1,S1>& rhs )
	{
		Matrix<T,S1,S1> r;
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S1; ++j )
			{
				r[i][j] = 0;
				for ( int k=0; k<S1; ++k )
				{
					r[i][j] += m[i][k] * rhs.m[k][j];
				}
			}
		}
		return r;
	}


	//-- transpose, inverse, determinant ---------------------------------------
	
	// transpose
	Matrix<T,S2,S1> transpose( ) const
	{
		assert( S1==S2 );

		Matrix<T,S2,S1> r;
		for ( int i=0; i<S1; ++i )
		{
			for ( int j=0; j<S2; ++j )
			{
				r[j][i] = m[i][j];
			}
		}
		return r;
	}

#if S1==2 && S2==2
	// specialized inverse of 2x2 matrix
	Matrix<T,S1,S2> inverse( ) const
	{
        T m00 = m[0][0], m01 = m[0][1];
        T m10 = m[1][0], m11 = m[1][1];

		T invDet = T(1) / (m00 * m11 - m01 * m10);

		T d00 = + m11 * invDet;
		T d01 = - m01 * invDet;
		T d10 = - m10 * invDet;
		T d11 = + m00 * invDet;

        return Matrix<T,S1,S2>( d00, d01,
							    d10, d11 );
	}
#endif

#if S1==3 && S2==3
	// specialized inverse of 3x3 matrix
	Matrix<T,S1,S2> inverse( ) const
	{
        T m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];
        T m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
        T m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

		T t00 = m22 * m11 - m21 * m12;
		T t01 = m22 * m01 - m21 * m02;
		T t02 = m12 * m01 - m11 * m02;

		T invDet = T(1) / (m00 * t00 - m10 * t01 + m20 * t02);

		T d00 = + t00 * invDet;
		T d01 = - t01 * invDet;
		T d02 = + t02 * invDet;

		T d10 = - (m22 * m10 - m20 * m12) * invDet;
		T d11 = + (m22 * m00 - m20 * m02) * invDet;
		T d12 = - (m12 * m00 - m10 * m02) * invDet;

		T d20 = + (m21 * m10 - m20 * m11) * invDet;
		T d21 = - (m21 * m00 - m20 * m01) * invDet;
		T d22 = + (m11 * m00 - m10 * m01) * invDet;

        return Matrix<T,S1,S2>( d00, d01, d02,
								d10, d11, d12,
								d20, d21, d22 );
	}
#endif

#if S1==4 && S2==4
	// specialized inverse of 4x4 matrix
	Matrix<T,S1,S2> inverse( ) const
	{
        T m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
        T m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
        T m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
        T m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

        T v0 = m20 * m31 - m21 * m30;
        T v1 = m20 * m32 - m22 * m30;
        T v2 = m20 * m33 - m23 * m30;
        T v3 = m21 * m32 - m22 * m31;
        T v4 = m21 * m33 - m23 * m31;
        T v5 = m22 * m33 - m23 * m32;

        T t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
        T t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
        T t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
        T t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

        T invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

        T d00 = t00 * invDet;
        T d10 = t10 * invDet;
        T d20 = t20 * invDet;
        T d30 = t30 * invDet;

        T d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        T d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        T d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        T d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;

        T d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        T d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        T d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        T d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m21 * m10 - m20 * m11;
        v1 = m22 * m10 - m20 * m12;
        v2 = m23 * m10 - m20 * m13;
        v3 = m22 * m11 - m21 * m12;
        v4 = m23 * m11 - m21 * m13;
        v5 = m23 * m12 - m22 * m13;

        T d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        T d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        T d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        T d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        return Matrix<T,S1,S2>( d00, d01, d02, d03,
							    d10, d11, d12, d13,
							    d20, d21, d22, d23,
							    d30, d31, d32, d33 );
	}
#endif

#if S1==2 && S2==2
	// specialized determinant of 2x2 matrix
	T determinant( ) const
	{
        T m00 = m[0][0], m01 = m[0][1];
        T m10 = m[1][0], m11 = m[1][1];

		return m00 * m11 - m01 * m10;
	}
#endif

#if S1==3 && S2==3
	// specialized determinant of 2x2 matrix
	T determinant( ) const
	{
        T m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];
        T m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
        T m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

		T t00 = m22 * m11 - m21 * m12;
		T t01 = m22 * m01 - m21 * m02;
		T t02 = m12 * m01 - m11 * m02;

		return m00 * t00 - m10 * t01 + m20 * t02;
	}
#endif

#if S1==4 && S2==4
	// specialized determinant of 4x4 matrix
	T determinant( ) const
	{
        T m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
        T m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
        T m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
        T m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

        T v0 = m20 * m31 - m21 * m30;
        T v1 = m20 * m32 - m22 * m30;
        T v2 = m20 * m33 - m23 * m30;
        T v3 = m21 * m32 - m22 * m31;
        T v4 = m21 * m33 - m23 * m31;
        T v5 = m22 * m33 - m23 * m32;

        T t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
        T t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
        T t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
        T t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

        return t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03;
	}
#endif


	//-- eigen values/vectors --------------------------------------------------

	// eigenvalues for normal square matrix using newmat library
	void eig( Matrix<T,S1,S2>* V, Vector<T,S1>* D )
	{
		assert( S1==S2 );

		NEWMAT::Matrix MN;
		NEWMAT::DiagonalMatrix DN;
		NEWMAT::Matrix VN;
		MN.ReSize( S1, S2 );	
	}

	// eigenvalues for symmetric matrix using newmat library
	void eigsym( Matrix<T,S1,S2>* V, Vector<T,S1>* D )
	{
		assert( S1==S2 );

		NEWMAT::SymmetricMatrix AN;
		NEWMAT::DiagonalMatrix DN;
		NEWMAT::Matrix VN;
		AN.ReSize( S1 );

		for ( unsigned int i=0; i<S1; ++i )
		{
			for ( unsigned int j=0; j<=i; ++j )
			{
				AN(i+1,j+1) = m[i][j];
			}
		}

		NEWMAT::EigenValues( AN, DN, VN );

		for ( unsigned int i=0; i<S1; ++i )
		{
			(*D)[i] = static_cast<T>(DN(i+1,i+1));
			for ( unsigned int j=0; j<S1; ++j )
			{
				(*V)[i][j] = static_cast<T>(VN(i+1,j+1));
			}
		}
	}

	//-- specialized R^3 functions ---------------------------------------------

#if S1==4 && S2==4
	// create Matrix<T,4,4> from position, scale and rotation
	void fromPosScaleRot( const Vector<T,3>& position,
						  const Vector<T,3>& scale,
						  const Quaternion<T>& rotation )
	{
		// create scale transform
		Matrix<T,4,4> S( scale[0],0,0,0,
						 0,scale[1],0,0,
						 0,0,scale[2],0,
						 0,0,0,       1 );

		// create rotate transform
		Matrix<T,4,4> R;
		rotation.toRotationMatrix( &R );

		// create matrix from R * S;
		(*this) = R * S;

		// set translation
		m[0][3] = position[0];
		m[1][3] = position[1];
		m[2][3] = position[2];
	}
#endif

#if S1==4 && S2==4
	// get position, scale and rotation from Mat<T,4,4>
	void toPosScaleRot(   const Vector<T,3>* position,
						  const Vector<T,3>* scale,
						  const Quaternion<T>* rotation )
	{
		// TODO: Implement this
	}
#endif


	//-- stream output ---------------------------------------------------------

	friend std::ostream& operator<< (std::ostream& lhs,
									  const Matrix<T, S1, S2>& rhs)
	{
		lhs << "Matrix<" << S1 << "," << S2 << "> = {\n";
		for ( unsigned int i = 0; i < S1; ++i )
		{
			for ( unsigned int j = 0; j < S2; ++j )
			{
				lhs << std::setw( 10 )
					<< std::setprecision( 5 )
					<< std::setfill( ' ' )
					<< std::fixed
					<< rhs.m[i][j] << " ";
			}
			lhs << ((i < (S1 - 1)) ? "\n" : "");
		}
		lhs << "};";
		return lhs;
	}


	//-- class info ------------------------------------------------------------

	// returns size of vector
	int size1() const { return S1; }
	int size2() const { return S2; }

private:

	T m[S1][S2];
};


//== STATIC CONSTANTS (TEMPLATED) ==============================================

#if !defined (S1) && !defined (S2)
template<typename T, int S1, int S2>
const Matrix<T,S1,S2> Matrix<T,S1,S2>::ZERO( T( 0 ), T( 0 ) );
template<typename T, int S1, int S2>
const Matrix<T,S1,S2> Matrix<T,S1,S2>::IDENTITY( T( 0 ), T( 1 ) );
#elif defined (S1) && defined (S2)
template<typename T>
const Matrix<T,S1,S2> Matrix<T,S1,S2>::ZERO( T( 0 ), T( 0 ) );
template<typename T>
const Matrix<T,S1,S2> Matrix<T,S1,S2>::IDENTITY( T( 0 ), T( 1 ) );
#endif
