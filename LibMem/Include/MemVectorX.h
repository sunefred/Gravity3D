//==============================================================================
//	
//	Do not include this directly, use MemVector.h instead
//
//	This a generic vector class that provide a primary template AND template
//	specializations using the copy/paste specialization paradigm using
//	preprocessor statements. See MemVector.h how this is used.
//
//==============================================================================


#ifndef S1
// primary template
template<typename T, int S1>
class Vector

#else
// template specialization on size
template<typename T>
class Vector<T,S1>
#endif

{
public:

	static const Vector<T, S1> ZERO;
	static const Vector<T, S1> IDENTITY;


public:
	
	//-- constructors ----------------------------------------------------------

	// default constructor, creates uninitialized values
	Vector() {}

	// constructor, creates initialized values based on s
	Vector( const T& s )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] = s;
		}
	}

#if S1==2
	// specialized constructor for 2-component vector
	explicit Vector( const T& a, const T& b )
	{
		v[0] = a; v[1] = b;
	}
#endif

#if S1==3
	// specialized constructor for 3-component vector
	explicit Vector( const T& a, const T& b, const T& c )
	{
		v[0] = a; v[1] = b; v[2] = c;
	}
#endif

#if S1==4
	// specialized constructor for 4-component vector
	explicit Vector( const T& a, const T& b, const T& c, const T& d )
	{
		v[0] = a; v[1] = b; v[2] = c; v[3] = d;
	}
#endif

	// compiler generated destructor is ok.

	// compiler generated copy constructor is ok.

	// compiler generated assignment operator is ok.

	// specialized construcor for vector of size s1-1 and a scalar
	explicit Vector( const Vector<T,S1-1>& u, const T& s )
	{
		for ( int i=0; i<S1-1; ++i )
		{
			v[i] = u[i];
		}
		v[S1-1] = s;
	}


	//-- access operators ------------------------------------------------------

	// access to element through v[i], read-write
	T& operator[] ( const int i )
	{
		assert( i>=0 && i<S1 );
		return v[i];
	}

	// access to element through v[i], read-only
	const T& operator[] ( const int i ) const
	{
		assert( i>=0 && i<S1 );
		return v[i];
	}

	// access to element through v(i), read-write
	T& operator() ( const int i )
	{
		assert( i>=0 && i<S1 );
		return v[i];
	}

	// access to element through v(i), read-only
	const T& operator() ( const int i ) const
	{
		assert( i>=0 && i<S1 );
		return v[i];
	}

#if 0<S1 && S1<4
	// specialioied read and write x-variable
	T& x()
    {
        return (*this)[0];
    }
#endif

#if 1<S1 && S1<4
	// specialioied read and write y-variable
    T& y()
    {
        return (*this)[1];
    }
#endif

#if 2<S1 && S1<4
	// specialioied read and write z-variable
    T& z()
    {
        return (*this)[2];
	}
#endif

#if 3<S1 && S1<4
	// specialioied read and write w-variable
    T& w()
    {
        return (*this)[3];
	}
#endif

	//-- comparison operators --------------------------------------------------

	// component-wise equality
	bool operator== ( const Vector<T,S1>& rhs ) const
	{
		for ( int i=0; i<S1; ++i )
		{
			if ( v[i] != rhs.v[i] ) return false;
		}
		return true;
	}

	// component-wise non-equality
	bool operator!= ( const Vector<T,S1>& rhs ) const
	{
		return !( *this == rhs );
	}

	
	//-- typecast operators ----------------------------------------------------

	// from Vector<T,s1> to Vector<T,s1-1>
	operator Vector<T,S1-1>() const
	{
		assert( S1>=2 );
		Vector<T,S1-1> r;
		for ( int i=0; i<S1-1; ++i )
		{
			r[i] = v[i];
		}
		return r;
	}

	// from Vector<T,s1> to Vector<T,s1+1>
	operator Vector<T,S1+1>() const
	{
		assert( S1>=1 );
		Vector<T,S1+1> r;
		for ( int i=0; i<S1; ++i )
		{
			r[i] = v[i];
		}
		r[S1] = 1;
		return r;
	}

	
	//-- unary operators -------------------------------------------------------

	// unarys plus
	Vector<T,S1> operator+ () const
	{
		return *this;
	}

	// unary minus
	Vector<T,S1> operator- () const
	{
		Vector<T,S1> r;
		for ( int i=0; i<S1; ++i )
		{
			r[i] = -v[i];
		}
		return r;
	}


	//-- vector-scalar operators -----------------------------------------------

	// self-multiplication with scalar
	Vector<T,S1>& operator*= ( const T& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] *= rhs;
		}
		return *this;
	}

	// self-division with scalar
	Vector<T,S1>& operator/= ( const T& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] /= rhs;
		}
		return *this;
	}

	// multiplication with scalar
	Vector<T,S1> operator* ( const T& rhs ) const
	{
		return Vector<T,S1>(*this) *= rhs;
	}

	// multiplication with scalar left hand version
	friend Vector<T, S1> operator* (const T& lhs, const Vector<T, S1>& rhs)
	{
		return Vector<T, S1>( rhs ) *= lhs;
	}

	// division with scalar
	Vector<T,S1> operator/ ( const T& rhs ) const
	{
		return Vector<T,S1>(*this) /= rhs;
	}


	//-- vector-vector operators -----------------------------------------------

	// self-addition
	Vector<T,S1>& operator+= ( const Vector<T,S1>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] += rhs.v[i];
		}
		return *this;
	}

	// self-subtraction
	Vector<T,S1>& operator-= ( const Vector<T,S1>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] -= rhs.v[i];
		}
		return *this;
	}

	// component-wise self-multiplication
	Vector<T,S1>& operator*= ( const Vector<T,S1>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] *= rhs.v[i];
		}
		return *this;
	}

	// component-wise self-division
	Vector<T,S1>& operator/= ( const Vector<T,S1>& rhs )
	{
		for ( int i=0; i<S1; ++i )
		{
			v[i] /= rhs.v[i];
		}
		return *this;
	}

	// addition of vectors
	Vector<T,S1> operator+ ( const Vector<T,S1>& rhs ) const
	{
		return Vector<T,S1>(*this) += rhs;
	}

	// subtraction of vectors
	Vector<T,S1> operator- ( const Vector<T,S1>& rhs ) const
	{
		return Vector<T,S1>(*this) -= rhs;
	}

	// component-wise multiplication of vectors
	Vector<T,S1> operator* ( const Vector<T,S1>& rhs ) const
	{
		return Vector<T,S1>(*this) *= rhs;
	}

	// component-wise division of vectors
	Vector<T,S1> operator/ ( const Vector<T,S1>& rhs ) const
	{
		return Vector<T,S1>(*this) /= rhs;
	}


	//-- norm ------------------------------------------------------------------
	
	// squared euclidian norm
	T normsqr( ) const
	{
		T r(0);
		for ( int i=0; i<S1; ++i )
		{
			r += v[i] * v[i];
		}
		return r;
	}

	// euclidian norm
	T norm( ) const
	{
		return static_cast<T>( sqrt( normsqr() ) );
	}

	// normalize vector and return the normalized vector. no /0 check
	Vector<T,S1>& normalize( )
	{
		T one(1);
		operator*=( one/norm() );
		return *this;
	}


	//-- dot- and cross-products -----------------------------------------------

	// dot-product operator
	T operator| ( const Vector<T,S1>& rhs ) const
	{
		T r(0);
		for ( int i=0; i<S1; ++i )
		{
			r += v[i] * rhs.v[i];
		}
		return r;
	}

	// dot-prduct function
	T dot( const Vector<T,S1>& rhs ) const
	{
		return *this | rhs;
	}


	//-- specialized cross product ---------------------------------------------

#if S1 == 3
	// cross-product through operator
	Vector<T,S1> operator% ( const Vector<T,S1>& rhs ) const
	{
		return Vector<T,S1>( v[1]*rhs.v[2]-v[2]*rhs.v[1],
							 v[2]*rhs.v[0]-v[0]*rhs.v[2],
							 v[0]*rhs.v[1]-v[1]*rhs.v[0] );

	}

	// cross-product using function
	Vector<T,S1> cross( const Vector<T,S1>& rhs ) const
	{
		return *this % rhs;
	}
#endif


	//-- stream output ---------------------------------------------------------

	friend std::ostream& operator<< (std::ostream& lhs,
									  const Vector<T, S1>& rhs)
	{
		lhs << "Vector<" << S1 << "> = {\n";
		for ( unsigned int i = 0; i < S1; ++i )
		{
			lhs << std::setw( 10 )
				<< std::setprecision( 5 )
				<< std::setfill( ' ' )
				<< std::fixed
				<< rhs.v[i] << " ";
			lhs << ((i < (S1 - 1)) ? "\n" : "");
		}
		lhs << "};";
		return lhs;
	}


	//-- class info ------------------------------------------------------------

	// returns size of vector
	int size() const { return S1; }


private:
	
	// holds actual data
	T v[S1];
};


//== STATIC CONSTANTS (TEMPLATED) ==============================================

#ifndef S1
template<typename T, int S1>
const Vector<T, S1> Vector<T, S1>::ZERO( T( 0 ) );
template<typename T, int S1>
const Vector<T, S1> Vector<T, S1>::IDENTITY( T( 1 ) );
#else
template<typename T>
const Vector<T, S1> Vector<T, S1>::ZERO( T( 0 ) );
template<typename T>
const Vector<T, S1> Vector<T, S1>::IDENTITY( T( 1 ) );
#endif
