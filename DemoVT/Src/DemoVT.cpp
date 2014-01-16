//==============================================================================
//
//	
//
//==============================================================================


//== INCLUDES ==================================================================

#include "DemoVT.h"


//== CLASS IMPLEMENTATION ======================================================

DemoVT::DemoVT()
: frame_( 0 )
{
	pathDataIn_ = "..\\..\\..\\Data\\";
	pathDataOut_ = "..\\..\\..\\Out\\";
	pathShaders_ = "..\\..\\..\\Shaders\\";
}


//-- update/draw related -------------------------------------------------------
void
DemoVT::init()
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
}


void
DemoVT::resize( int _width, int _height )
{
}


void
DemoVT::draw()
{
	frame_++;
}


//-- keyboard callbacks ----------------------------------------------------

void
DemoVT::keyboardPressFunc( const int _key, const float _x, const float _y,
							const int _mousestate, const int _modifiers )
{
	if ( _modifiers & KEYBOARD_MODIFIER_CTRL )
	{
		switch ( _key )
		{
		case KEYBOARD_KEY_R:
			break;
		case KEYBOARD_KEY_S:
			break;
		default:
			break;
		}
	}
}

void
DemoVT::keyboardReleaseFunc( const int _key, const float _x, const float _y,
							  const int _mousestate, const int _modifiers )
{
}

//-- mouse callbacks -------------------------------------------------------

void
DemoVT::mousePressFunc( const int _button, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
}

void
DemoVT::mouseReleaseFunc( const int _button, const float _x, const float _y,
						   const int _mousestate, const int _modifiers )
{
}

void
DemoVT::mouseDoubleClickFunc( const int _button, const float _x, const float _y,
							   const int _mousestate, const int _modifiers )
{
}

void
DemoVT::mouseMoveFunc( const float _x, const float _y,
						const int _mousestate, const int _modifiers )
{
}
void
DemoVT::mouseWheelFunc( const int _delta, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
}

//== THE END ===================================================================