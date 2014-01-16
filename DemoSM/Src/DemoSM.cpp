//==============================================================================
//
//	
//
//==============================================================================


//== INCLUDES ==================================================================

#include "DemoSM.h"


//== CLASS IMPLEMENTATION ======================================================

DemoSM::DemoSM()
: frame_( 0 )
{
	pathDataIn_ = "..\\..\\..\\Data\\";
	pathDataOut_ = "..\\..\\..\\Out\\";
	pathShaders_ = "..\\..\\..\\Shaders\\";
}


//-- update/draw related -------------------------------------------------------
void
DemoSM::init()
{
	// init GLEW
	glewExperimental = GL_TRUE;
	if ( glewInit() == GLEW_OK )
	{
		GEM_CONSOLE( "glewInit() ok." );
	}
	else
	{
		GEM_ERROR( "glewInit() failed." );
	}


	// Test for OpenGL 4.2
	if ( GLEW_VERSION_4_2 )
	{
		GEM_CONSOLE( "OpenGL 4.2 supported." );
	}
	else
	{
		GEM_ERROR( "OpenGL 4.2 not supported." );
	}


	// Why isn' this core?
	if ( GL_EXT_texture_compression_s3tc )
	{
		GEM_CONSOLE( "S3TC texture compression supported." );
	}
	else
	{
		GEM_ERROR( "S3TC texture compression not supported." );
	}


	// Setup Scene Graph
	//
	//		root
	//		|--objectPivot
	//		|--cameraPivot
	//		|  `--camera
	//      |--lightPivot
	//		|  `--light
	//
	root_.setName( "root" );
	cam_.setName( "cam" );
	lgt_.setName( "lgt" );
	objPivot_.setName( "objPivot" );
	camPivot_.setName( "camPivot" );
	lgtPivot_.setName( "lgtPivot" );
	root_.addChild( &objPivot_ );
	root_.addChild( &camPivot_ );
	root_.addChild( &lgtPivot_ );
	camPivot_.addChild( &cam_ );
	lgtPivot_.addChild( &lgt_ );


	// Setup object transform
	objPivot_.translate( 0, 10, 0 );


	// Setup camera
	cam_.translate( 0, 0, 200 );
	cam_.setFrustum( 20 * Mem::DEG2RAD, 0.1, 1000 );
	camPivot_.rotate( -25 * Mem::DEG2RAD, 0, 1, 0 );
	camPivot_.rotate( -20 * Mem::DEG2RAD, 1, 0, 0 );


	// Setup lights
	lgt_.translate( 0, 0, 200 );
	lgt_.setFrustum( 20 * Mem::DEG2RAD, 0.1, 1000 );
	lgtPivot_.rotate(  45 * Mem::DEG2RAD, 0, 1, 0 );
	lgtPivot_.rotate( -45 * Mem::DEG2RAD, 1, 0, 0 );


	// Navigation controller
	orbi_.setCameraPivot( &camPivot_ );
	orbi_.setCamera( &cam_ );


	// Load resources
	mesh_.load( pathDataIn_ + "town_triangles.obj" );
	colorTexture_.create( 1024, 1024, TEXTURE_FORMAT_R_32F );
	depthTexture_.create( 1024, 1024, TEXTURE_FORMAT_R_32F );
	depthVertexShader_.load( pathShaders_ + "depth.vert" );
	depthFragmentShader_.load( pathShaders_ + "depth.frag" );
	shadowsVertexShader_.load( pathShaders_ + "shadows.vert" );
	shadowsFragmentShader_.load( pathShaders_ + "shadows.frag" );


	// Setup depth render state
	depthRenderState_.setClear( true, Vec4f::ZERO, true, 1 );
	depthRenderState_.setCulling( false, CULL_FACE_BACK );
	depthRenderState_.setDepthTest( true, true, DEPTH_FUNC_LEQUAL );
	depthRenderState_.setMesh( &mesh_ );
	depthRenderState_.setVertexShader( &depthVertexShader_ );
	depthRenderState_.setFragmentShader( &depthFragmentShader_ );
	depthRenderState_.setFramebuffer( &depthTexture_,
										FRAMEBUFFER_ATTACHMENT_COLOR0 );
	depthRenderState_.setUniform( objPivot_.getDerivedTransformPtr(),
									"modelMatrix" );
	depthRenderState_.setUniform( lgt_.getViewMatrixPtr(), "viewMatrix" );
	depthRenderState_.setUniform( lgt_.getProjMatrixPtr(), "projMatrix" );
	

	// setup shadows render state
	shadowsRenderState_.setClear( true, Vec4f::ZERO, true, 1 );
	shadowsRenderState_.setCulling( false, CULL_FACE_BACK );
	shadowsRenderState_.setDepthTest( true, true, DEPTH_FUNC_LEQUAL );
	shadowsRenderState_.setViewportPtr( cam_.getViewportWidthPtr(),
									  cam_.getViewportHeightPtr() );
	shadowsRenderState_.setMesh( &mesh_ );
	shadowsRenderState_.setVertexShader( &shadowsVertexShader_ );
	shadowsRenderState_.setFragmentShader( &shadowsFragmentShader_ );
	shadowsRenderState_.setTexture( &depthTexture_, TEXTURE_UNIT_0 );
	shadowsRenderState_.setUniform( objPivot_.getDerivedTransformPtr(),
										"modelMatrix" );
	shadowsRenderState_.setUniform( cam_.getDerivedPositionPtr(),
										"camPosition" );
	shadowsRenderState_.setUniform( cam_.getViewMatrixPtr(),
										"camViewMatrix" );
	shadowsRenderState_.setUniform( cam_.getProjMatrixPtr(),
										"camProjMatrix" );
	shadowsRenderState_.setUniform( lgt_.getDerivedPositionPtr(),
										"lgtPosition" );
	shadowsRenderState_.setUniform( lgt_.getViewMatrixPtr(),
										"lgtViewMatrix" );
	shadowsRenderState_.setUniform( lgt_.getProjMatrixPtr(),
										"lgtProjMatrix" );
}


void
DemoSM::resize( int _width, int _height )
{
	cam_.setViewport( _width, _height );
}


void
DemoSM::draw()
{
	frame_++;


	// update scene
	root_.update();


	// Draw each Render State
	depthRenderState_.draw();
	shadowsRenderState_.draw();


	// draw coordinate grid
	//Primitives::drawGrid();


	// draw axis, gizmos etc
	//root_.draw();


	// draw transform controller gizmos
	//trns.draw();
}


//-- keyboard callbacks ----------------------------------------------------

void
DemoSM::keyboardPressFunc( const int _key, const float _x, const float _y,
							const int _mousestate, const int _modifiers )
{
	if ( _modifiers & KEYBOARD_MODIFIER_CTRL )
	{
		switch ( _key )
		{
		case KEYBOARD_KEY_R:
			//mesh_.reload();
			depthVertexShader_.reload();
			depthFragmentShader_.reload();
			shadowsVertexShader_.reload();
			shadowsFragmentShader_.reload();
			break;
		case KEYBOARD_KEY_S:
			depthRenderState_.downloadBuffers();
			shadowsRenderState_.downloadBuffers();
			colorTexture_.save( pathDataOut_ + "colorTexture_.pfm" );
			depthTexture_.save( pathDataOut_ + "depthTexture_.pfm" );
			mesh_.save( pathDataOut_ + "mesh_.obj" );
			break;
		default:
			//
			break;
		}
	}

	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

void
DemoSM::keyboardReleaseFunc( const int _key, const float _x, const float _y,
							  const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

//-- mouse callbacks -------------------------------------------------------

void
DemoSM::mousePressFunc( const int _button, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoSM::mouseReleaseFunc( const int _button, const float _x, const float _y,
						   const int _mousestate, const int _modifiers )
{
	orbi_.mouseReleaseFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoSM::mouseDoubleClickFunc( const int _button, const float _x, const float _y,
							   const int _mousestate, const int _modifiers )
{
	orbi_.mouseDoubleClickFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoSM::mouseMoveFunc( const float _x, const float _y,
						const int _mousestate, const int _modifiers )
{
	orbi_.mouseMoveFunc( _x, _y,_mousestate, _modifiers );
}
void
DemoSM::mouseWheelFunc( const int _delta, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	orbi_.mouseWheelFunc( _delta, _x, _y, _mousestate, _modifiers );
}

//== THE END ===================================================================