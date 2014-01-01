//==============================================================================
//	
//	This is the interface for classes that want to handle mouse and keyboard
//	input and modify the scene in some way. The intention is to have it hold
//	the required input states, but currently the requirements on the application
//	is small so the entire required state (mouse pos, modifiers) is passed
//	for each event handler.
//
//==============================================================================


#ifndef GEM_CONTROLLER_H
#define GEM_CONTROLLER_H


//== INCLUDES ==================================================================


#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================

class Controller
{

public:

	//-- constructors ----------------------------------------------------------

	Controller();
	~Controller();


	//-- sets and gets ---------------------------------------------------------


	void enableDebug( ) { mIsDebug = true;  }

	void disableDebug( ) { mIsDebug = false; }


	//-- keyboard callbacks ----------------------------------------------------

	virtual void keyboardPressFunc( const int key,
									const float x,
									const float y,
									const int mousestate,
									const int modifiers );
	virtual void keyboardReleaseFunc( const int key,
									  const float x,
									  const float y,
									  const int mousestate,
									  const int modifiers );

	virtual void keyboardFunc( unsigned char key, int x, int y ) {}
	virtual void keyboardUpFunc( unsigned char key, int x, int y ) {}
	virtual void specialFunc( int key, int x, int y ) {}
	virtual void specialUpFunc( int key, int x, int y ) {}


	//-- mouse callbacks -------------------------------------------------------

	virtual void mousePressFunc( const int button,
								 const float x,
								 const float y,
								 const int mousestate,
								 const int modifiers );
	virtual void mouseReleaseFunc( const int button,
								   const float x,
								   const float y,
								   const int mousestate,
								   const int modifiers );
	virtual void mouseDoubleClickFunc( const int button,
									   const float x,
									   const float y,
									   const int mousestate,
									   const int modifiers );
	virtual void mouseMoveFunc( const float x,
								const float y,
								const int mousestate,
								const int modifiers );
	virtual void mouseWheelFunc( const int delta,
								 const float x,
								 const float y,
								 const int mousestate,
								 const int modifiers );

	virtual void mouseFunc( int button, int state, int x, int y ) {}
	virtual void motionFunc( int x, int y ) {}
	virtual void passiveMotionFunc( int x, int y ) {}
	virtual void entryFunc( int state ) {}


protected:

	bool mIsDebug;

private:

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================