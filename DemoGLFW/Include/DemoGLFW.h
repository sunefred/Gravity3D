//==============================================================================
//
//
//==============================================================================


#ifndef DEMOGLFW_H
#define DEMOGLFW_H


//== INCLUDES ==================================================================

#include "Gem.h"



//== NAMESPACES ================================================================

using namespace Mem;
using namespace Gem;


//== CLASS PREDEFENITIONS ======================================================



//== CLASS DEFENITIONS =========================================================

class DemoGLFW
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	DemoGLFW();

	// default copy constructor is ok

	// default destructor ok


public:

	//-- update/draw related ---------------------------------------------------

	void init();
	void resize( int _width, int _height );
	void draw();


	//-- keyboard callbacks ----------------------------------------------------

	void keyboardPressFunc( const int _key,
									const float _x,
									const float _y,
									const int _mousestate,
									const int _modifiers );
	void keyboardReleaseFunc( const int _key,
									  const float _x,
									  const float _y,
									  const int _mousestate,
									  const int _modifiers );

	//-- mouse callbacks ----------------------------------------------------

	void mousePressFunc( const int _button,
								 const float _x,
								 const float _y,
								 const int _mousestate,
								 const int _modifiers );
	void mouseReleaseFunc( const int _button,
								   const float _x,
								   const float _y,
								   const int _mousestate,
								   const int _modifiers );
	void mouseDoubleClickFunc( const int _button,
									   const float _x,
									   const float _y,
									   const int _mousestate,
									   const int _modifiers );
	void mouseMoveFunc( const float _x,
								const float _y,
								const int _mousestate,
								const int _modifiers );
	void mouseWheelFunc( const int _delta,
								 const float _x,
								 const float _y,
								 const int _mousestate,
								 const int _modifiers );


private:

	void createPrimitiveSphere( const float _radius,
								const float _slices,
								const float _stacks,
								const PRIM_TYPE _primType = PRIM_TYPE_TRIANGLE);

	void createPrimitiveCube( const float _size,
							  const PRIM_TYPE _primType = PRIM_TYPE_TRIANGLE );

private:
	
	// Global paths
	std::string pathDataIn_;
	std::string pathDataOut_;
	std::string pathShaders_;


	// Loaders
	MeshLoader mesh_, meshtfb_;
	TextureLoader texture_;
	ShaderLoader vertexShader_;
	ShaderLoader fragmentShader_;
	ShaderLoader geometryShader_;
	ShaderLoader tessCtrlShader_;
	ShaderLoader tessEvalShader_;
	Allocator primitives_;
	Allocator positions_;
	Allocator normals_;
	Allocator texCoords_;


	// Rendering
	RenderState renderState_;
	float tesselationLevel_;
	Vec4f solidColor_;
	Vec4f wireColor_;

	
	// Nodes
	SceneNode root_;
	SceneNode objPivot_;
	SceneNode camPivot_;
	SceneNode lgtPivot_;
	CameraNode cam_;
	LightNode lgt_;


	// Controllers
	OrbitalController orbi_;


	// Animators
	long long frame_;
};


//== THE END ===================================================================

#endif