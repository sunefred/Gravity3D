//==============================================================================
//
//	I only implemented a GLFW app so I could run the nVidia nSight profiler
//	which does not work with Qt. 
//
//==============================================================================


//== INCLUDES ==================================================================

#include "DemoGLFW.h"
#include <GLFW\glfw3.h>


//== GLOBALS ===================================================================

DemoGLFW* demo_ = new DemoGLFW();
GLFWwindow* window_;
int mousestate_ = 0;
int modifiers_ = 0;


//== CALLBACKS =================================================================

void frameBufferSizeCallback( GLFWwindow* _window, int _width, int _height )
{
	demo_->resize( _width, _height );
}

void keyCallback( GLFWwindow* _window, int _key,
				  int _scancode, int _action, int _modifiers )
{
	// translate modifier
	KEYBOARD_MODIFIER modifier;
	switch ( _key )
	{
		case GLFW_KEY_LEFT_SHIFT:
			modifier = KEYBOARD_MODIFIER_SHIFT;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			modifier = KEYBOARD_MODIFIER_SHIFT;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			modifier = KEYBOARD_MODIFIER_CTRL;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			modifier = KEYBOARD_MODIFIER_CTRL;
			break;
		case GLFW_KEY_LEFT_ALT:
			modifier = KEYBOARD_MODIFIER_ALT;
			break;
		case GLFW_KEY_RIGHT_ALT:
			modifier = KEYBOARD_MODIFIER_ALT;
			break;
		default:
			modifier = KEYBOARD_MODIFIER_NONE;
			break;
	}


	// update modifiers
	if ( _action==GLFW_PRESS     )
	{
		modifiers_ |= modifier;
	}
	else if ( _action==GLFW_RELEASE )
	{
		modifiers_ &= ~modifier;
	}


	// translate key
	KEYBOARD_KEY key;
	switch ( _key )
	{
	case GLFW_KEY_R:
		key = KEYBOARD_KEY_R;
		break;
	case GLFW_KEY_S:
		key = KEYBOARD_KEY_S;
		break;
	default:
		key = KEYBOARD_KEY_NONE;
		break;
	}


	// call keyboard function
	double x, y;
	glfwGetCursorPos( _window, &x, &y );
	if ( _action==GLFW_PRESS  )
	{
		demo_->keyboardPressFunc( key, x, y, mousestate_, modifiers_ );
	}
	else if ( _action==GLFW_RELEASE )
	{
		demo_->keyboardReleaseFunc( key, x, y, mousestate_, modifiers_ );
	}
}

void mouseButtonCallback( GLFWwindow* _window, int _button,
						  int _action, int _modifiers )
{
	// update mousestate
	if ( _action==GLFW_PRESS     )
	{
		switch ( _button )
		{
		case GLFW_MOUSE_BUTTON_1:
			mousestate_ |= 0x00000001;
			break;
		case GLFW_MOUSE_BUTTON_2:
			mousestate_ |= 0x00000002;
			break;
		case GLFW_MOUSE_BUTTON_3:
			mousestate_ |= 0x00000004;
			break;
		}
	}
	else if ( _action==GLFW_RELEASE )
	{
		switch ( _button )
		{
		case GLFW_MOUSE_BUTTON_1:
			mousestate_ &= ~0x00000001;
			break;
		case GLFW_MOUSE_BUTTON_2:
			mousestate_ &= ~0x00000002;
			break;
		case GLFW_MOUSE_BUTTON_3:
			mousestate_ &= ~0x00000004;
			break;
		}
	}


	// function doesn't come with cursor positions
	double x, y;
	glfwGetCursorPos( _window, &x, &y );


	// double click, press down, release
	if ( _action==GLFW_PRESS && ( glfwGetTime() < 0.2 ) )
	{
		glfwSetTime( 0 );
		demo_->mouseDoubleClickFunc( _button, x, y, mousestate_, modifiers_ );
	}
	else if ( _action==GLFW_PRESS )
	{
		glfwSetTime( 0 );
		demo_->mousePressFunc( _button, x, y, mousestate_, modifiers_ );
	}
	else if ( _action==GLFW_RELEASE )
	{
		demo_->mouseReleaseFunc( _button, x, y, mousestate_, modifiers_ );
	}
}

void cursorPosCallback( GLFWwindow* _window, double _x, double _y )
{
	demo_->mouseMoveFunc( _x, _y, mousestate_, modifiers_ );
}

void scrollCallback( GLFWwindow* _window, double _dunno, double _delta )
{
	double x, y;
	glfwGetCursorPos( _window, &x, &y );
	demo_->mouseWheelFunc( 120*_delta, x, y, mousestate_, modifiers_ );
}


//== APPLICATION ENTRY POINT ===================================================

int main(void)
{
    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context 
	// http://www.glfw.org/docs/latest/window.html
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 4 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    window_ = glfwCreateWindow( 512, 512, "AppGLFW", NULL, NULL );
    if (!window_)
    {
        glfwTerminate();
        return -1;
    }

	// Make the window's context current
    glfwMakeContextCurrent( window_ );

	// set callbacks
	glfwSetFramebufferSizeCallback( window_, frameBufferSizeCallback );
	glfwSetKeyCallback ( window_, keyCallback );
	glfwSetMouseButtonCallback( window_, mouseButtonCallback );
	glfwSetCursorPosCallback( window_, cursorPosCallback );
	glfwSetScrollCallback( window_, scrollCallback );

	// init app
	demo_->init();

    // Loop until the user closes the window 
    while ( !glfwWindowShouldClose( window_ ) )
    {
        // Render here 
		demo_->draw();

        // Swap front and back buffers 
        glfwSwapBuffers( window_ );

        // Poll for and process events
        glfwPollEvents();
    }

	// clean up
	delete demo_;
    glfwTerminate();
}

//== THE END ===================================================================
