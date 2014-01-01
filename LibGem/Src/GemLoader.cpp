//== INCLUDES ==================================================================

#include "GemLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================

//-- constructors/destructor ---------------------------------------------------

Loader::Loader()
//: class initializer list
{
}


//-- private file utility ------------------------------------------------------

void
Loader::splitPath( const std::string& _path,
				   std::string* _dir, std::string* _name, std::string* _ext )
{
	// find positions for last / and last .
	unsigned int posName = _path.find_last_of( "\\/" );
	unsigned int posExt = _path.find_last_of( "." );


	// extract directory (full) and filename
	if ( std::string::npos != posName &&
		 std::string::npos != posExt )
	{
		(*_dir) = _path.substr( 0, posName + 1 );
		(*_name) = _path.substr( posName + 1, posExt - posName - 1 );
	}


	// extract extension
	if ( std::string::npos != posExt )
	{
		(*_ext) = _path.substr( posExt + 1 );
	}
}

//==============================================================================
GEM_END_NAMESPACE
//==============================================================================