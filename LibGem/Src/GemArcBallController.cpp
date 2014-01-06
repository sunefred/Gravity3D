//== INCLUDES ==================================================================

#include "GemArcBallController.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== IMPLEMENTATION ============================================================


//-- constructors --------------------------------------------------------------

ArcBallController::ArcBallController( )
	: Controller()
	, x_(0)
	, y_(0)
	, cameraPtr_(NULL)
	, cameraPivotPtr_(NULL)
	, radius_(1)
	, zoomSpeed_(2.0f/1000.0f)
{}


ArcBallController::~ArcBallController( )
{}


//-- keyboard callbacks --------------------------------------------------------

void
ArcBallController::keyboardPressFunc( const int _key,
									  const float _x,
									  const float _y,
									  const int _mousmstate,
									  const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mousePressFunc( key, x, y, mousestate, modifiers );
}

void
ArcBallController::keyboardReleaseFunc( const int _key,
										const float _x,
										const float _y,
										const int _mousestate,
										const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mousePressFunc( key, x, y, mousestate, modifiers );
}

//-- mouse callbacks -------------------------------------------------------

void
ArcBallController::mousePressFunc( const int _button,
								   const float _x,
								   const float _y,
								   const int _mousestate,
								   const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mousePressFunc( button, x, y, mousestate, modifiers );


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
ArcBallController::mouseReleaseFunc( const int _button,
									 const float _x,
									 const float _y,
									 const int _mousestate,
									 const int _modifiers )

{
	//this->enableDebug( );
	//Controller::mouseReleaseFunc( button, x, y, mousestate, modifiers );
}

void
ArcBallController::mouseDoubleClickFunc( const int _button,
										 const float _x,
										 const float _y,
										 const int _mousestate,
										 const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mouseDoubleClickFunc( button, x, y, mousestate, modifiers );
}

void
ArcBallController::mouseMoveFunc( const float _x,
								  const float _y,
							      const int _mousestate,
								  const int _modifiers )
{
	//this->enableDebug( );
	//Controller::mouseMoveFunc( x, y, mousestate, modifiers );


	// Maya style control with Alt
	if ( !(_modifiers & KEYBOARD_MODIFIER_ALT) )
		return;


	// rotate in response to alt+left
	if ( cameraPivotPtr_ && cameraPtr_ &&
	   ( _mousestate & MOUSE_BUTTON_LEFT ) )
	{
		//    		 ____
		//    	\	/    \
		//       \ |      |   <-- arcball
		//    	  \	\____/
		// motion  \  |
		//   ray	\ |  <--- mouse down ray
		//    		 \|
		//    	   camera
		//

		// geat world space coordinates for old and new mouse XY
		Vec3f mouse_old = cameraCpy_.unprojectWindow2World( Vec2f( x_, x_ ) );
		Vec3f mouse_new = cameraCpy_.unprojectWindow2World( Vec2f( _x, _y ) );


		// return ray from camera center through view plane: p0 + t * dir
		Vec3f p0 = cameraCpy_.getDerivedPosition( );
		Vec3f dir_old = (mouse_old - p0).normalize( );
		Vec3f dir_new = (mouse_new - p0).normalize( );


		// find intersection point with arcball sphere, if non is found then
		// return without updating rotation
		float t_old, t_new;
		//if ( !Primitives::intersectRaySphere( p0, dir_old, mRadius, &t_old ) ||
		//	 !Primitives::intersectRaySphere( p0, dir_new, mRadius, &t_new ) )
		//{
		//	return;
		//}
		return;


		// calculate directions from arcball center to intersection points
		Vec3f center = cameraPivotCpy_.getDerivedPosition( );
		Vec3f p_old = p0 + t_old*dir_old - center;
		Vec3f p_new = p0 + t_new*dir_new - center;


		// error metric for zero comparisons
		float eps = std::numeric_limits<float>::epsilon( );


		/* no constraints, default but confusing rotation */
		if ( false &&
			 !Mem::iszero<float>( 1.0f - p_old.dot( p_new ), 4 * eps ) )
		{
			/** reset to saved state before rotating **/
			(*cameraPivotPtr_) = cameraPivotCpy_;

			/** rotate around a radians around axis **/
			float a = -acos( p_old.dot( p_new ) );
			Vec3f axis = p_old.cross( p_new );
			cameraPivotPtr_->rotate( a, axis, TRANSFORM_MODE_WORLD );
		}


		/* <- x -> constrain to PIVOT x-axis */
		/* <- y -> constrain to PIVOT PARENT y-axis */
		if ( true &&
			 !Mem::iszero<float>( 1.0f - p_old.dot( p_new ), 4 * eps ) )
		{
			/** reset to saved state before rotating **/
			(*cameraPivotPtr_) = cameraPivotCpy_;

			/** project onto PIVOT PARENT y-axis (in world coordinates) **/
			Vec3f axisy = Vec3f( 0, 1, 0 );
			Vec3f axisyw = cameraPivotCpy_.getDerivedTransform( )
				* cameraPivotCpy_.getTransform( ).inverse( ) * axisy;
			Vec3f py_old = (p_old - p_old.dot( axisyw ) * axisyw).normalize( );
			Vec3f py_new = (p_new - p_new.dot( axisyw ) * axisyw).normalize( );

			/** <- x -> rotate around PIVOT PARENT y-axis */
			if ( !Mem::iszero<float>( 1.0f - py_old.dot( py_new ), 2 * eps ) )
			{
				float s = Mem::signum( axisyw.dot( py_old.cross( py_new ) ) );
				float a = -s * acos( py_old.dot( py_new ) );
				Vec3f axis = py_old.cross( py_new );
				cameraPivotPtr_->rotate( a, axisy, TRANSFORM_MODE_PARENT );
			}

			/** project onto PIVOT x-axis (in world coordinates) **/
			Vec3f axisx = Vec3f( 1, 0, 0 );
			Vec3f axisxw = cameraPivotCpy_.getDerivedTransform( ) * axisx;
			Vec3f px_old = (p_old - p_old.dot( axisxw ) * axisxw).normalize( );
			Vec3f px_new = (p_new - p_new.dot( axisxw ) * axisxw).normalize( );

			/** <- y -> rotate around PIVOT x-axis */
			if ( !Mem::iszero<float>( 1.0f - px_old.dot( px_new ), 2 * eps ) )
			{
				float s = Mem::signum( axisxw.dot( px_old.cross( px_new ) ) );
				float a = -s * acos( px_old.dot( px_new ) );
				Vec3f axis = px_old.cross( px_new );
				cameraPivotPtr_->rotate( a, axisx, TRANSFORM_MODE_LOCAL );
			}
		}
	}


	// translate in response to alt+middle
	else if ( cameraPivotPtr_ && cameraPtr_ && 
			( _mousestate & MOUSE_BUTTON_MIDDLE ) )
	{
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
		float dy = -_y + y_;
		float o = cameraCpy_.getPosition( ).norm( );
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
ArcBallController::mouseWheelFunc( const int _delta,
								   const float _x,
								   const float _y,
								   const int _mousestate,
								   const int _modifiers )
{
	//this->enableDebug( );
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
		float dz = -_delta / 5;
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