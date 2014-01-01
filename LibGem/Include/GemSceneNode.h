//==============================================================================
//
// The SceneNode extends on the TransformNode by adding properties to determine
// the fill of the node within the scene. It holds primitives for intersection
// and collision handling, i.e. it holds bounding volumes.
//
//==============================================================================


#ifndef GEM_SCENENODE_H
#define GEM_SCENENODE_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemTransformNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class SceneNode : public TransformNode
{

public:


	//-- constructors/destructor -----------------------------------------------

	// default constructor ok
	SceneNode();

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