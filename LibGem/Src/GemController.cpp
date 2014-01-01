//== INCLUDES ==================================================================

#include "GemController.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//=== IMPLEMENTATION ===========================================================

Controller::Controller( )
: mIsDebug(false)
{
}

Controller::~Controller( )
{
}


//-- keyboard callbacks ----------------------------------------------------

void
Controller::keyboardPressFunc( const int key, const float x, const float y,
							   const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::keyboardPressFunc(): "
				  << "key=" << key << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}

void
Controller::keyboardReleaseFunc( const int key, const float x, const float y,
								 const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::keyboardReleaseFunc(): "
				  << "key=" << key << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}

//-- mouse callbacks -------------------------------------------------------

void
Controller::mousePressFunc( const int button, const float x, const float y,
const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::mousePressFunc(): "
				  << "button=" << button << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}

void
Controller::mouseReleaseFunc( const int button, const float x, const float y,
const int mousestate, const int modifiers )

{
	if ( mIsDebug )
	{
		std::cout << "Controller::mouseReleaseFunc(): "
				  << "button=" << button << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}

void
Controller::mouseDoubleClickFunc( const int button, const float x, const float y,
const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::mouseDoubleClickFunc(): "
				  << "button=" << button << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}

void
Controller::mouseMoveFunc( const float x, const float y,
const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::mouseMoveFunc():"
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}
void
Controller::mouseWheelFunc( const int delta, const float x, const float y,
const int mousestate, const int modifiers )
{
	if ( mIsDebug )
	{
		std::cout << "Controller::mouseWheelFunc():"
			      << "delta=" << delta << ", "
				  << "x=" << x << ", "
				  << "y=" << y << ", "
				  << "mousestate=" << mousestate << ", "
				  << "modifiers=" << modifiers << '\n';
	}
}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================