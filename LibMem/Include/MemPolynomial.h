//==============================================================================
//	
//	Polynomial class with rootsolving
//
//==============================================================================


#ifndef MEM_POLYNOMIAL_H
#define MEM_POLYNOMIAL_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================


template<typename T>
class Polynomial
{
public:

	//-- define, typedef, enum -------------------------------------------------

	#define MAXDEG 4


	//-- constructors ----------------------------------------------------------

	// default constructor, creates uninitialized values
	Polynomial()
		: n(0)
		, c(1,0)
	{}

	// compiler generated destructor is ok.

	// compiler generated copy constructor is ok.

	// compiler generated assignment operator is ok.

	// create constant polynomial
	Polynomial( const T& c0 )
		: n(0)
	{
		n = 0;
		c.push_back( c0 );
	}

	// create linear polynomial
	Polynomial( const T& c0, const T& c1 )
	{
		n = 1;
		c.push_back( c0 );
		c.push_back( c1 );
	}

	// create quadratic polynomial
	Polynomial( const T& c0, const T& c1, const T& c2 )
	{
		n = 2;
		c.push_back( c0 );
		c.push_back( c1 );
		c.push_back( c2 );
	}

	// create cubic polynomial
	Polynomial( const T& c0, const T& c1, const T& c2, const T& c3 )
	{
		n = 3;
		c.push_back( c0 );
		c.push_back( c1 );
		c.push_back( c2 );
		c.push_back( c3 );
	}

	// create quartic polynomial
	Polynomial( const T& c0, const T& c1, const T& c2, const T& c3,
				const T& c4 )
	{
		n = 4;
		c.push_back( c0 );
		c.push_back( c1 );
		c.push_back( c2 );
		c.push_back( c3 );
		c.push_back( c4 );
	}

	// create any degree polynomial
	Polynomial( const std::vector<T>& d )
	{
		assert( d.size()<=MAXDEG+1 );
		if ( d.size()==0 )
		{
			n = 0;
			c.push_back( T(0) );
		}
		else
		{
			n = static_cast<unsigned int>(d.size()) - 1;
			c = d;
		}
	}


	//-- access operators ------------------------------------------------------

	//-- comparison operators --------------------------------------------------
	
	//-- typecast operators ----------------------------------------------------

	//-- unary operators -------------------------------------------------------

	//-- polynomial-scalar operators -------------------------------------------
	
	//-- polynomial-polynomial operators ---------------------------------------


	//-- translate and scale ---------------------------------------------------

	// scale polynomial graph horisontally
	void scalex( const T& scale )
	{
		T y = 1, scaleinv = T(1) / scale;
		for ( unsigned int i=0; i<=n; ++i )
		{
			c[i] *= y;
			y *= scaleinv;
		}
	}

	// scale polynomial graph vertically
	void scaley( const T& scale )
	{
		for ( unsigned int i=0; i<=n; ++i )
		{
			c[i] *= scale;
		}
	}

	// translate polynomial graph horisontally
	void translatex( const T& trans )
	{
		/* create a pascal triangle

			1 1 1 1 1
			0 1 2 3 4
			0 0 1 3 6 . . .
			0 0 0 1 4
			0 0 0 0 1 
				.
				.
		*/
		T P [MAXDEG+1][MAXDEG+1];
		for ( unsigned int i=0; i<=n; ++i )
		{
			P[i][0] = 0;
			P[0][i] = 1;
		}
		for ( unsigned int i=1; i<=n; ++i )
		{
			for ( unsigned int j=1; j<=n; ++j )
			{
				P[i][j] = P[i-1][j-1] + P[i][j-1];
			}
		}
	
		// translate polynomial by changing coefficients
		T t, sgn;
		std::vector<T> d(n+1,0);
		for ( unsigned int i=0; i<=n; ++i )
		{
			t = T(1);
			for ( unsigned int j=0; j<=(n-i); ++j )
			{
				sgn = j%2==0 ? T(1) : T(-1);
				d[i] += sgn * P[i][i+j] * c[i+j] * t;
				t *= trans;
			}
		}
		c = d;
	}

	// translate polynomial graph vertically
	void translatey( const T& trans )
	{
		c[0] += trans;
	}


	//-- evaluation ------------------------------------------------------------
	
	// polynomial evaluation given a value x
	T evaluate( const T& x )
	{
		// get largest coefficient
		const T lc(c[n]);

		// infinity cases x=-inf or x=inf
		const T posinf(std::numeric_limits<T>::max());
		const T neginf(-posinf);
		if ( x==posinf )
		{
			return lc>0 ? posinf : neginf;
		}
		else if ( x==neginf && n%2==0 )
		{
			return lc>0 ? posinf : neginf;
		}
		else if ( x==neginf && n%2==1 )
		{
			return lc>0 ? neginf : posinf;
		}

		// -inf < x < inf
		T fval = 0, y = 1;
		for ( unsigned int i=0; i<=n; ++i )
		{
			fval += c[i] * y;
			y *= x;
		}
		return fval;
	}


	//-- derivatives -----------------------------------------------------------

	// return first order derivative
	Polynomial<T> derivative( )
	{
		std::vector<T> d( n, 0 );
		for ( unsigned int i=0; i<n; ++i )
		{
			d[i] = (i+1) * c[i+1];
		}
		return Polynomial<T>( d );
	}


	//-- root finding ----------------------------------------------------------

	// find all real roots using stable numerical method
	std::vector<T> roots( )
	{
		// settings
		const unsigned int maxit = 1000;
		const T eps = std::numeric_limits<T>::epsilon();
		const T posinf = std::numeric_limits<T>::max();
		const T neginf = -posinf;
		const T two(2);


		// create empty extreme and root vector
		std::vector<T> roots, extrms;


		// degree = 0 polynomials not allowed
		if ( n==0 ) {}


		// degree = 1 polynomials are trivial
		else if ( n==1 )
		{
			if ( c[1]!=0 )
			{
				roots.push_back( -c[0]/c[1] );
			}
		}


		// degree > 1 polynomials
		else
		{
			// get extreme values by solving roots of derivative
			extrms = this->extremes();
			

			// look for roots between each pair of extreme values
			for ( unsigned int i=0; i<extrms.size()-1; ++i )
			{
				T lo  = extrms[i];
				T hi  = extrms[i+1];
				T flo = this->evaluate( lo );
				T fhi = this->evaluate( hi );


				// crude double root detection (zero proximity)
				if ( Math::iszero<T>( flo, 2*eps ) )
				{
					roots.push_back( flo );
				}


				// bracket roots and bisect
				if ( Math::signnequal<T>( flo, fhi ) )
				{
					this->bracket( &lo, &hi );
					roots.push_back( this->bisect( lo, hi, maxit ) );
				}
			}
		}

		
		// return real roots
		return roots;
	}

	// find extreme values, contains at least [-inf 0 inf]
	std::vector<T> extremes( )
	{
		// settings
		const T posinf(std::numeric_limits<T>::max());
		const T neginf(-posinf);


		// get derivative roots
		std::vector<T> extrms = this->derivative().roots();


		// if zero (if needed) and -inf and inf
		if ( extrms.empty() )
		{
			extrms.push_back( 0 );
		}
		extrms.push_back( neginf );
		extrms.push_back( posinf );


		// sort and remove doublets
		std::sort( extrms.begin(), extrms.end() );
		extrms.erase( std::unique( extrms.begin(), extrms.end() ),
					  extrms.end() );


		// return extreme values
		return extrms;
	}

	//-- class info ------------------------------------------------------------

	unsigned int degree() const { return n; }

private:

	//-- private functions -----------------------------------------------------

	// bracket around root and make sure limits are finite
	void bracket( T* low, T* high )
	{
		// settings
		const T posinf(std::numeric_limits<T>::max());
		const T neginf(-posinf);
		T one(1), two(2);

		
		// make sure low is low and high is high
		T lo = *low, hi = *high;
		if ( hi < lo )
		{
			std::swap( lo, hi );
		}


		// male sure lo and hi are finite numbers
		if ( lo == neginf )
		{
			T dx = one;
			lo = hi - dx;
			while ( Math::signequal<T>( this->evaluate( lo ),
										this->evaluate( hi ) ) )
			{
				dx *= two;
				lo -= dx;
			}
		}
		else if ( hi == posinf )
		{
			T dx = one;
			hi = lo + dx;
			while ( Math::signequal<T>( this->evaluate( lo ),
										this->evaluate( hi ) ) )
			{
				dx *= two;
				hi += dx;
			}
		}


		// return vals
		(*low) = lo, (*high) = hi;
	}


	// bisect finds a single root, x, in the range x<[lo hi] where lo and hi
	// have different signs
	T bisect( const T& low, const T& high, const unsigned int maxit )
	{
		// local vars
		T lo = low, hi = high, md, hlf(0.5);
		T eps = std::numeric_limits<T>::epsilon();
		T tolx(0), two(2);


		// binary bisection to find f(x)=0 where x < [l,h], l,h finite
		for ( unsigned int i=0; i<maxit; ++i )
		{

			tolx = two * Math::max( fabs(lo), fabs(hi) ) * eps;
			if ( fabs( hi - lo ) <  tolx )
			{
				return lo;
			}

			md = hlf * lo + hlf * hi;
			//printf( "%d: %02.15f, %02.15f, %02.15f\n", i, lo, md, hi );
			if ( Math::signequal( this->evaluate( lo ),
								  this->evaluate( md ) ) )
			{
				lo = md;
			}
			else
			{
				hi = md;
			}
		}

			
		// should never get here
		std::cout << "Too many iterations in Polynomial::bisect()\n";
		return T(0);
	}


	// create pascal triangle of some size
	//void pascal( const unsigned int size )
	//{
	//}

	//-- private variables -----------------------------------------------------

	unsigned int		n;		// polynomial degree
	std::vector<T>		c;		// polynomial coefficients c0, c1, c2, ...
};


//== TYPEDEFS ==================================================================


/** only floating point Polynomials */
typedef Polynomial<float> Polyf;
typedef Polynomial<double> Polyd;



//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================