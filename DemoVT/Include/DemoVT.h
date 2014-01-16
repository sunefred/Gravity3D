//==============================================================================
//
//
//==============================================================================


#ifndef DEMOVT_H
#define DEMOVT_H


//== INCLUDES ==================================================================

#include "Gem.h"



//== NAMESPACES ================================================================

using namespace Mem;
using namespace Gem;


//== CLASS PREDEFENITIONS ======================================================



//== CLASS DEFENITIONS =========================================================

class DemoVT
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	DemoVT();

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
	
	// Global paths
	std::string pathDataIn_;
	std::string pathDataOut_;
	std::string pathShaders_;

	// Animators
	long long frame_;
};


//== THE END ===================================================================

#endif