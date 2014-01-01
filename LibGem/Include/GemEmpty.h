//==============================================================================
//
// An empty class, copy this when creating a new class.h
//
//==============================================================================


#ifndef GEM_EMPTY_H
#define GEM_EMPTY_H


//== INCLUDES ==================================================================


#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class Empty
{

public:


	//-- constructors/destructor -----------------------------------------------

	// default constructor
	Empty();

	// default copy constructor is ok
	// implement this if you allocate data
	Empty( const Empty& other ) {}

	// destructor
	~Empty();

	// default assignment operator is ok
	// implement this if you allocate data
	Empty& Empty::operator=( const Empty& other ) {}


	//-- copy and clear --------------------------------------------------------

	// implement this if you allocate data
	void copy( const Empty& other ) {}

	// implement this if you allocate data
	void clear() {}


protected:


protected:


private:


private:

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================