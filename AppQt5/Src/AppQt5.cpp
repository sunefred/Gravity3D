//==============================================================================
//
//	
//
//==============================================================================


//== INCLUDES ==================================================================

#include "AppQt5.h"
#include <QtGui\QtGui>
#include <QtCore\QElapsedTimer>

//== CLASS IMPLEMENTATION ======================================================

AppQt5::AppQt5( const QGLFormat& format )
: QGLWidget( format )
, frame_(0)
, tesselationLevel_( 3 )
{
	pathDataIn_ = "..\\..\\..\\Data\\";
	pathDataOut_ = "..\\..\\..\\Out\\";
	pathShaders_ = "..\\..\\..\\Shaders\\";

	// post resize event
	resize( 512, 512 );
}


//-- update/draw related -------------------------------------------------------

void
AppQt5::initializeGL()
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


	// Why isn' this core?
	if ( GL_EXT_texture_compression_s3tc )
	{
		GEM_CONSOLE( "S3TC texture compression supported." );
	}
	else
	{
		GEM_ERROR( "S3TC texture compression not supported." );
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
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


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
	cam_.setFrustum( 20 * Mem::DEG2RAD, 0.1, 1000 );
	camPivot_.rotate( -5 * Mem::DEG2RAD, 1, 0, 0 );


	// Setup lights
	lgt_.translate( 0, 0, 20 );
	//lgt_.setColor( 0.90, 0.90, 0.80 );
	lgtPivot_.rotate( 20 * Mem::DEG2RAD, 0, 1, 0 );


	// Navigation controller
	orbi_.setCameraPivot( &camPivot_ );
	orbi_.setCamera( &cam_ );


	// Load resources
	mesh_.load( pathDataIn_ + "box_quads.obj" );
	vertexShader_.load( pathShaders_ + "pass_through.vert" );
	tessCtrlShader_.load( pathShaders_ + "pass_through.tesc" );
	tessEvalShader_.load( pathShaders_ + "pass_through.tese" );
	geometryShader_.load( pathShaders_ + "pass_through.geom" );
	fragmentShader_.load( pathShaders_ + "pass_through.frag" );


	// Setup render stages
	renderState_.setMesh( &mesh_ );
	renderState_.setVertexShader( &vertexShader_ );
	renderState_.setTessCtrlShader( &tessCtrlShader_ );
	renderState_.setTessEvalShader( &tessEvalShader_ );
	renderState_.setGeometryShader( &geometryShader_ );
	renderState_.setFragmentShader( &fragmentShader_ );
	renderState_.setUniform( objPivot_.getDerivedTransformPtr(),
							 "modelMatrix" );
	renderState_.setUniform( cam_.getViewMatrixPtr(), "viewMatrix" );
	renderState_.setUniform( cam_.getProjMatrixPtr(), "projMatrix" );
	renderState_.setUniform( &tesselationLevel_, "tesselationLevel" );
}


void
AppQt5::resizeGL( int _width, int _height)
{
	cam_.setSize( _width, _height );
}


void
AppQt5::paintGL()
{
	frame_++;


	// update scene
	root_.update();


	// setup viewport
	glViewport( 0, 0, cam_.getWidth(), cam_.getHeight() );


	// clear and draw background
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//Primitives::drawBackground();


	// Draw each Render State
	renderState_.draw();


	// draw coordinate grid
	//Primitives::drawGrid();


	// draw axis, gizmos etc
	//root_.draw();


	// draw transform controller gizmos
	//trns.draw();
}

//-- keyboard callbacks ----------------------------------------------------

void
AppQt5::keyPressEvent( QKeyEvent* _e )
{
	if ( _e->modifiers() & KEYBOARD_MODIFIER_CTRL )
	{
		switch ( _e->key() )
		{
		case KEYBOARD_KEY_R:
			mesh_.reload();
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

	orbi_.keyboardPressFunc( _e->key(), 0, 0,
							 0, _e->modifiers() );
}

void
AppQt5::keyReleaseEvent( QKeyEvent* _e )
{
	orbi_.keyboardReleaseFunc( _e->key(), 0, 0,
							   0, _e->modifiers() );
}


//-- mouse callbacks -----------------------------------------------------------

void
AppQt5::mousePressEvent( QMouseEvent* _e )
{
	orbi_.mousePressFunc( _e->button(), _e->pos().x(), _e->pos().y(),
						  _e->buttons(), _e->modifiers() );
}

void
AppQt5::mouseReleaseEvent( QMouseEvent* _e )
{
	orbi_.mouseReleaseFunc( _e->button(), _e->pos().x(), _e->pos().y(),
							_e->buttons(), _e->modifiers() );
}

void
AppQt5::mouseDoubleClickEvent( QMouseEvent* _e )
{
	orbi_.mouseDoubleClickFunc( _e->button(), _e->pos().x(), _e->pos().y(),
								_e->buttons(), _e->modifiers() );
}

void
AppQt5::mouseMoveEvent( QMouseEvent* _e )
{
	orbi_.mouseMoveFunc( _e->pos().x(), _e->pos().y(),
						 _e->buttons(), _e->modifiers() );
}

void
AppQt5::wheelEvent( QWheelEvent* _e )
{
	if ( _e->modifiers() == KEYBOARD_MODIFIER_NONE )
	{
		tesselationLevel_ = tesselationLevel_+Mem::signum( _e->delta() );
		tesselationLevel_ = Mem::clamp<float>( tesselationLevel_, 1, 20 );
	}

	orbi_.mouseWheelFunc( _e->delta(), _e->pos().x(), _e->pos().y(),
						  _e->buttons(), _e->modifiers() );
}


//== THE END ===================================================================