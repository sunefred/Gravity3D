//==============================================================================
//
// Light Node = Camera Node for now
//
//==============================================================================


#ifndef GEM_LIGHTNODE_H
#define GEM_LIGHTNODE_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemCameraNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class LightNode : public CameraNode
{

public:


	//-- constructors/destructor -----------------------------------------------

	// default constructor ok
	LightNode();

	// default copy constructor is ok

	// default destructor ok


	//-- copy and clear --------------------------------------------------------

	// implement this if you have pointers
	void copy() {}

	// implement this if you have pointers
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