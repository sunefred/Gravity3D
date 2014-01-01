//==============================================================================
//	
//	Prerequisite dependencies for the entire Mem library in one central location
//
//==============================================================================


#ifndef MEM_PREREQUISITES_H
#define MEM_PREREQUISITES_H


//== GLOBAL INCLUDES ===========================================================

//// cpp I/O library
//#include <iostream>	// cin, cout, cerr, clog
//#include <istream>	// istream, iostream, ostream, streambuf
//#include <fstream>	// ifstream, fstream, ofstream, filebuf
//#include <sstream>	// istringstream, stringstream, ostringstream, stringbuf
//
//// cpp containers
//#include <string>	// string
#include <vector>	// vector
//
//// cpp manipulators
//#include <iomanip> // setprecision, 
//
//// cpp limits
#include <limits>	// numeric_limits<int>::max()
//
//// cpp algorithms
#include <algorithm> // for_each, sort, unique
//
//// c libraries
#include <cassert>	// assert
#include <cmath>	// pow, sqrt, sin, cos


//== DISABLE WARNINGS ==========================================================


//== GLOBAL DEFINES ============================================================

#define MEM_BEGIN_NAMESPACE namespace Mem {
#define MEM_END_NAMESPACE	}


#ifdef _WIN32
#define NOMINMAX	/* protect against min/max macro */
#endif

// still want to be able to use NULL when stdio.h is removed
#ifndef NULL
#define NULL 0
#endif


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE
using namespace Mem;


//== TYPEDEFS ==================================================================



//== CLASS PRE-DEFINITIONS =====================================================

// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
class Math;
template<typename T,int s1,int s2> class Matrix;
template<typename T> class Polynomial;
template<typename T> class Quaternion;
template<typename T,int s1> class Vector;


//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================

