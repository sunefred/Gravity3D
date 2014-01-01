//==============================================================================
//
//	
//
//==============================================================================


//== INCLUDES ==================================================================

#include "AppGLFW.h"


//== CLASS IMPLEMENTATION ======================================================

AppGLFW::AppGLFW()
: frame_( 0 )
, tesselationLevel_( 3 )
{
	pathDataIn_ = "..\\..\\..\\Data\\";
	pathDataOut_ = "..\\..\\..\\Out\\";
	pathShaders_ = "..\\..\\..\\Shaders\\";
}


//-- update/draw related -------------------------------------------------------
void
AppGLFW::init()
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


	//// z-buffer
	//glEnable(GL_DEPTH_TEST);		// enables z-buffer
	//glDepthMask(GL_TRUE);			// place new values in z-buffer
	//glDepthFunc(GL_LEQUAL);		// comparison mode


	//// culling
	glEnable( GL_CULL_FACE );		// enables culling
	glCullFace( GL_BACK );			// GL_BACK or GL_FRONT


	// clear
	glClearColor( 0, 0, 0, 0 );		// clear color when clearing colorbit
	glClearDepth( 1 );				// clear value when clearing dephtbit
	

	// polygon mode
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


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


	// Setup camera
	cam_.translate( 0, 0, 20 );
	cam_.setFrustum( 20 * Math::DEG2RAD, 0.1, 1000 );
	camPivot_.rotate( -5 * Math::DEG2RAD, 1, 0, 0 );


	// Setup lights
	lgt_.translate( 0, 0, 20 );
	//lgt_.setColor( 0.90, 0.90, 0.80 );
	lgtPivot_.rotate( 20 * Math::DEG2RAD, 0, 1, 0 );


	// Navigation controller
	orbi_.setCameraPivot( &camPivot_ );
	orbi_.setCamera( &cam_ );


	// Load resources
	mesh_.load( pathDataIn_ + "box.obj" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512.bmp" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512_monochrome.bmp" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512.pfm" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512_monochrome.pfm" );
	texture_.load( pathDataIn_ + "world.200406.3x512x512.dds" );
	vertexShader_.load( pathShaders_ + "basic.vert" );
	//tessCtrlShader_.load( pathShaders_ + "basic.tesc" );
	//tessEvalShader_.load( pathShaders_ + "basic.tese" );
	//geometryShader_.load( pathShaders_ + "basic.geom" );
	fragmentShader_.load( pathShaders_ + "basic.frag" );


	// Setup render stages
	renderStage_.setMesh( &mesh_ );
	renderStage_.setTexture( &texture_, 0 );
	renderStage_.setVertexShader( &vertexShader_ );
	renderStage_.setTessCtrlShader( &tessCtrlShader_ );
	renderStage_.setTessEvalShader( &tessEvalShader_ );
	renderStage_.setGeometryShader( &geometryShader_ );
	renderStage_.setFragmentShader( &fragmentShader_ );
	renderStage_.setUniform( objPivot_.getDerivedTransformPtr(),
							 "modelMatrix" );
	renderStage_.setUniform( cam_.getViewMatrixPtr(), "viewMatrix" );
	renderStage_.setUniform( cam_.getProjMatrixPtr(), "projMatrix" );
	//renderStage_.setUniform( &tesselationLevel_, "tesselationLevel" );

}


void
AppGLFW::resize( int _width, int _height )
{
	cam_.setSize( _width, _height );
}


void
AppGLFW::draw()
{
	frame_++;


	// update scene
	root_.update();


	// setup viewport
	glViewport(0, 0, cam_.getWidth(), cam_.getHeight());


	// clear and draw background
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//Primitives::drawBackground();


	// Draw each Render Stage
	renderStage_.draw();


	// draw coordinate grid
	//Primitives::drawGrid();


	// draw axis, gizmos etc
	//root_.draw();


	// draw transform controller gizmos
	//trns.draw();
}


//-- keyboard callbacks ----------------------------------------------------

void
AppGLFW::keyboardPressFunc( const int _key, const float _x, const float _y,
							const int _mousestate, const int _modifiers )
{
	if ( _modifiers & KEYBOARD_MODIFIER_CTRL )
	{
		switch ( _key )
		{
		case KEYBOARD_KEY_R:
			mesh_.reload();
			texture_.reload();
			vertexShader_.reload();
			tessCtrlShader_.reload();
			tessEvalShader_.reload();
			geometryShader_.reload();
			fragmentShader_.reload();
			break;
		case KEYBOARD_KEY_S:
			mesh_.save( pathDataOut_ + "box_quads.obj" );
			break;
		default:
			//
			break;
		}
	}

	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

void
AppGLFW::keyboardReleaseFunc( const int _key, const float _x, const float _y,
							  const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

//-- mouse callbacks -------------------------------------------------------

void
AppGLFW::mousePressFunc( const int _button, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
AppGLFW::mouseReleaseFunc( const int _button, const float _x, const float _y,
						   const int _mousestate, const int _modifiers )
{
	orbi_.mouseReleaseFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
AppGLFW::mouseDoubleClickFunc( const int _button, const float _x, const float _y,
							   const int _mousestate, const int _modifiers )
{
	orbi_.mouseDoubleClickFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
AppGLFW::mouseMoveFunc( const float _x, const float _y,
						const int _mousestate, const int _modifiers )
{
	orbi_.mouseMoveFunc( _x, _y,_mousestate, _modifiers );
}
void
AppGLFW::mouseWheelFunc( const int _delta, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	if ( _modifiers == KEYBOARD_MODIFIER_NONE )
	{
		tesselationLevel_ = tesselationLevel_+Math::signum( _delta );
		tesselationLevel_ = Math::clamp<float>( tesselationLevel_, 1, 20 );
	}

	orbi_.mouseWheelFunc( _delta, _x, _y, _mousestate, _modifiers );
}

//== THE END ===================================================================