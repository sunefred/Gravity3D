//==============================================================================
//
//
//==============================================================================


#ifndef DEMOSM_H
#define DEMOSM_H


//== INCLUDES ==================================================================

#include "Gem.h"



//== NAMESPACES ================================================================

using namespace Mem;
using namespace Gem;


//== CLASS PREDEFENITIONS ======================================================



//== CLASS DEFENITIONS =========================================================

class DemoSM
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	DemoSM();

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
	MeshLoader mesh_;
	TextureLoader colorTexture_;
	TextureLoader depthTexture_;
	ShaderLoader depthVertexShader_;
	ShaderLoader depthFragmentShader_;
	ShaderLoader shadowsVertexShader_;
	ShaderLoader shadowsFragmentShader_;


	// Rendering
	RenderState depthRenderState_;
	RenderState shadowsRenderState_;

	
	// Nodes
	SceneNode root_;
	SceneNode objPivot_;
	SceneNode camPivot_;
	SceneNode lgtPivot_;
	CameraNode cam_;
	CameraNode lgt_;


	// Controllers
	OrbitalController orbi_;


	// Animators
	long long frame_;
};


//== THE END ===================================================================

#endif