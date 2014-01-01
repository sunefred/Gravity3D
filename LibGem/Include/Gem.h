//==============================================================================
//
//	This is a very simple but capable graphics engine build around setting up a
//	RenderStage object, i.e the OpenGL state. It is written against OpenGL 4.2
//	core profile, so no fixed pipile functions and it supports tesselation.
//	The following input and output can be set on a RenderStage instance:
//
//
//	INPUT:
//	------
//	Vertex Data
//	Texture Data*
//	Shaders (Vertex, TessControl, TessEvaluation, Geometry, Fragment)
//	Uniforms*
//	Texture Buffer Object*
//
//
//	OUTPUT:
//	-------
//	Frame Buffer Object*
//	Transform Feedback*
//
//
//	To use this engine it is helpful to know that it contains five separate
//	systems. They are: Rendering, Loaders, Nodes, Controllers and Animators.
//	Theese systems do not interact by themselves (i.e the Renderer does not
//	load meshes and Cameras does not automatically connect to uniform variables
//	on the Renderer). Instead these interactions are created in a top->down
//	approach by the user. The user instances objects and connects them to solve
//	a given problem. Most applications does the following.
//
//	1. Load or create Meshes, Textures and Shaders using Loader classes
//	2. Connect the Loader at INPUT/OUTPUT locations on a RenderStage instance
//	3. Create Nodes and connect them in a tree structure to represent object
//	transformations.
//	4. Connect Node transforms to uniform variables on the RenderStage instance
//	5. Create Controllers to capture input and connect to Cameras or SceneNodes
//	to make them movable, pickable etc.
//	
//	Repeat 1-3 for each object or render pass you want to create. The
//	RenderStage object also contains support to set OpenGL flags such depth
//	write, depth clear, and more so you should never have to write any OpenGL
//	in your own application.
//
//
//	RENDERING
//	----------
//	Holds OpenGL state. Set its INPUT/OUTPUT and call draw().
//
//	- RenderStage
//
//
//	LOADERS
//	-------
//	Contain Allocator objects that can connect to RenderStage INPUT/OUTPUT.
//
//	Allocator
//	Loader .---> MeshLoader
//	       |---> TextureLoader*
//	       |---> ShaderLoader
//	       |---> MocapLoader*
//	       |---> SkeletonLoader*
//
//
//	NODES
//	---------
//	Parent/child relationships and transform propagation. SceneNodes add
//	bounding volume information to allow picking.
//
//	TransformNode ---> SceneNode .---> LightNode
//	                             |---> CameraNode
//	                             |---> MocapNode*
//	                             |---> JointNode*
//
//
//	Controllers
//	-----------
//	Recievers for mouse and keyboard input and affects nodes.
//
//	Controller .---> ArcBallController
//	           |---> OribtalController*
//	           |---> TransformController*
//				
//
//	Animators
//	----------
//	Has update(time) function and affects nodes and storage data.
//
//	Animator .---> MocapAnimator*
//	         |---> SkeletonAnimator*
//
//
//	* not yet implemented
//
//==============================================================================

#ifndef GEM_H
#define GEM_H


//== INCLUDES ==================================================================


// Rendering
#include "GemRenderStage.h"

// Loaders
#include "GemAllocator.h"
#include "GemMeshLoader.h"
#include "GemTextureLoader.h"
#include "GemShaderLoader.h"

// Nodes
#include "GemTransformNode.h"
#include "GemSceneNode.h"
#include "GemCameraNode.h"
#include "GemLightNode.h"

// Controllers
#include "GemController.h"
#include "GemOrbitalController.h"
#include "GemArcBallController.h"

// Animators


//==============================================================================
#endif
//==============================================================================