//==============================================================================
//
//	Basic interface for file loaders. Also contains some file specific
//	utility functions,
//
//==============================================================================


#ifndef GEM_LOADER_H
#define GEM_LOADER_H


//== INCLUDES ==================================================================


#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class Loader
{

public:


	//-- constructors/destructor -----------------------------------------------

	// default constructor ok
	Loader();

	// default copy constructor is ok

	// default destructor ok


	//-- copy and clear --------------------------------------------------------

	// implement this if you have pointers
	void copy() {}

	// implement this if you have pointers
	void clear() {}


public:

	virtual void load( const std::string& _path,
					   const FILE_FORMAT _fileType = FILE_FORMAT_NONE ) {};

	virtual void save( const std::string& _path,
					   const FILE_FORMAT _fileType = FILE_FORMAT_NONE ) {};


protected:

	//-- private file utility --------------------------------------------------

	void splitPath( const std::string& _path,
					std::string* _dir, std::string* _name, std::string* _ext );

protected:


private:

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================