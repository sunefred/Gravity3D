//== INCLUDES ==================================================================

#include "GemOrbitalController.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== IMPLEMENTATION ============================================================


//-- constructors --------------------------------------------------------------

OrbitalController::OrbitalController( )
	: Controller()
	, x_(0)
	, y_(0)
	, cameraPtr_(NULL)
	, cameraPivotPtr_(NULL)
	, rotSpeed_(-Mem::HALFPI/360)
	, zoomSpeed_(2.0f/1000.0f)
{}


OrbitalController::~OrbitalController( )
{}


//-- keyboard callbacks --------------------------------------------------------

void
OrbitalController::keyboardPressFunc( const int _key, const float _x, const float _y,
						    const int _mousestate, const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

void
OrbitalController::keyboardReleaseFunc( const int _key, const float _x, const float _y,
							  const int _mousestate, const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}


//-- mouse callbacks -------------------------------------------------------

void
OrbitalController::mousePressFunc( const int _button, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	//this->enableDebug();
	//Controller::mousePressFunc( _button, _x, _y, _mousestate, _modifiers );


	// Maya style control with Alt
	if ( !(_modifiers & KEYBOARD_MODIFIER_ALT) )
		return;


	// update mouse states
	x_ = _x;
	y_ = _y;

	// copy node content, movement is done relative this copy rather than the
	// live version for stability reasons
	cameraCpy_ = (*cameraPtr_);
	cameraPivotCpy_ = (*cameraPivotPtr_);
}

void
OrbitalController::mouseReleaseFunc( const int _button, const float _x, const float _y,
						   const int _mousestate, const int _modifiers )
{
	//this->enableDebug();
	//Controller::mouseReleaseFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
OrbitalController::mouseDoubleClickFunc( const int _button, const float _x, const float _y,
							   const int _mousestate, const int _modifiers )
{
	//this->enableDebug();
	//Controller::mouseDoubleClickFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
OrbitalController::mouseMoveFunc( const float _x, const float _y, 
						const int _mousestate, const int _modifiers )
{
	//this->enableDebug();
	//Controller::mouseMoveFunc( _x, _y, _mousestate, _modifiers );


	// Maya style control with Alt
	if ( !(_modifiers & KEYBOARD_MODIFIER_ALT) )
		return;


	// rotate in response to alt+left
	if ( cameraPivotPtr_ && cameraPtr_ &&
	   ( _mousestate & MOUSE_BUTTON_LEFT ) )
	{
		//	     y
		//	     |
		//  x ---'
		float dx = _x - x_;
		float dy = y_ - _y;
		float ry = dx * rotSpeed_;
		float rx = -dy * rotSpeed_;
		Vec3f axisx = Vec3f( 1, 0, 0 );
		Vec3f axisy = Vec3f( 0, 1, 0 );

		/** reset to saved state before rotating **/
		(*cameraPivotPtr_) = cameraPivotCpy_;

		/** <- x -> rotate around PIVOT PARENT y-axis */
		cameraPivotPtr_->rotate( ry, axisy, TRANSFORM_MODE_PARENT );

		/** <- y -> rotate around PIVOT x-axis */
		cameraPivotPtr_->rotate( rx, axisx, TRANSFORM_MODE_LOCAL );

		//Quatf r = cameraPivotPtr_->getRotation();
		//std::cout << r[0] << ", " << r[1] << ", " << r[2] << ", " << r[3] << '\n';
	}


	// translate in response to alt+middle
	else if ( cameraPivotPtr_ && cameraPtr_ &&
		    ( _mousestate & MOUSE_BUTTON_MIDDLE ) )
	{
		//	     y
		//	     |
		//  x ---'
		float dx = _x - x_;
		float dy = y_ - _y;
		float o = cameraCpy_.getPosition().norm();
		float f = cameraCpy_.getFrustumFOV();
		float h = cameraCpy_.getViewportHeight();
		float tx = -(dx/h) * o * f;
		float ty = -(dy/h) * o * f;
		Vec3f axisx = Vec3f( 1, 0, 0 );
		Vec3f axisy = Vec3f( 0, 1, 0 );

		/** reset to saved state before translating **/
		(*cameraPivotPtr_) = cameraPivotCpy_;

		/** <- x -> translate along center x-axis */
		cameraPivotPtr_->translate( axisx*tx, TRANSFORM_MODE_LOCAL );
		//cameraPivotPtr_->translate( axisx*dx*speedx, Node::BASIS_LOCAL );

		/** <- y -> translate along center y-axis */
		cameraPivotPtr_->translate( axisy*ty, TRANSFORM_MODE_LOCAL );
		//cameraPivotPtr_->translate( axisy*dy*speedy, Node::BASIS_LOCAL );

	}


	// zoom in resonse to alt+right
	else if ( cameraPivotPtr_ && cameraPtr_ &&
		    ( _mousestate & MOUSE_BUTTON_RIGHT ) )
	{
		//	     y
		//	     |
		//  x ---'
		//
		/**
		* n = f(y) = e^k*y describes a function between screen pixels
		* and the zoom level
		*
		* dy = pixel difference between mouse down and motion
		* t = translation
		* k = speed constant
		* n = new position
		* o = old position
		*
		* n = e^k*y
		* o = e^k*oy
		* ny = oy + dy
		* n = e^k(ny) e^k(oy+dy)= e^k*oy * e^k*dy = o * e^k*dy
		* t = n - o = o * e^k*dy - o
		**/
		float dy = y_ - _y;
		float o = cameraCpy_.getPosition().norm();
		float k = zoomSpeed_;
		float t = o * exp( k * dy ) - o;
		Vec3f axisz = Vec3f( 0, 0, 1 );

		/** reset to saved state before zooming **/
		(*cameraPtr_) = cameraCpy_;

		/** <- y -> translate along lookat z-axis */
		cameraPtr_->translate( axisz*t, TRANSFORM_MODE_PARENT );
	}
}

void
OrbitalController::
mouseWheelFunc( const int _delta, const float _x, const float _y,
				const int _mousestate, const int _modifiers )
{
	//this->enableDebug();
	//Controller::mouseWheelFunc( delta, x, y, mousestate, modifiers );


	// Maya style control with Alt
	if ( !(_modifiers & KEYBOARD_MODIFIER_ALT) )
		return;


	// zoom in response to scrool wheel forward or backward motion
	if ( cameraPivotPtr_ && cameraPtr_ )
	{
		//	     z
		//	     |
		//		 '
		//		 |
		//		-z
		//		
		float dz = -_delta/5;
		float o = cameraPtr_->getPosition( ).norm( );
		float k = zoomSpeed_;
		float t = o * exp( k * dz ) - o;
		Vec3f axisz = Vec3f( 0, 0, 1 );

		/** <- y -> translate along lookat z-axis */
		cameraPtr_->translate( axisz*t, TRANSFORM_MODE_PARENT );
	}
}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================