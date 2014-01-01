//==============================================================================
//
//	There are two different controllers for controlling the camera, the Orbital
//	and ArcBall. The Orbital is a Maya style controller with rotate, translate
//	and zoom, suited for scenes ofarbitrary objects. The rotation is a constant
//	deg/pixels which makes it possible to turn turn turn forever.
//
//	Rotate: Alt + Left Mouse
//	Translate: Alt + Middle Mouse
//	Zoom: Alt + Right Mouse or Scroll Wheel
//
//==============================================================================

#ifndef GEM_ORBITALCONTROLLER_H
#define GEM_ORBITALCONTROLLER_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemController.h"
#include "GemTransformNode.h"
#include "GemCameraNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================


class OrbitalController : public Controller
{
public:


	//-- constructors ----------------------------------------------------------

	OrbitalController();
	~OrbitalController();


	//-- sets and gets ---------------------------------------------------------
	
	void setCamera( CameraNode* _cameraPtr )
	{ cameraPtr_ = _cameraPtr; }

	CameraNode* getCamera( ) const
	{ return cameraPtr_; }

	void setCameraPivot( TransformNode* _cameraPivotPtr )
	{ cameraPivotPtr_ = _cameraPivotPtr; }

	TransformNode* getCameraPivot( ) const
	{ return cameraPivotPtr_; }
	
	void setRotSpeed( const float _rotSpeed ) { rotSpeed_ = _rotSpeed; }

	float getRotSpeed( ) const { return rotSpeed_; }

	void setZoomSpeed( const float _zoomSpeed ) { zoomSpeed_ = _zoomSpeed; }

	float getZoomSpeed( ) const { return zoomSpeed_; }


	//-- keyboard callbacks ----------------------------------------------------

	virtual void keyboardPressFunc( const int _key,
									const float _x,
									const float _y,
									const int _mousestate,
									const int _modifiers );
	virtual void keyboardReleaseFunc( const int _key,
									  const float _x,
									  const float _y,
									  const int _mousestate,
									  const int _modifiers );


	//-- mouse callbacks -------------------------------------------------------

	virtual void mousePressFunc( const int _button,
								 const float _x,
								 const float _y,
								 const int _mousestate,
								 const int _modifiers );
	virtual void mouseReleaseFunc( const int _button,
								   const float _x,
								   const float _y,
								   const int _mousestate,
								   const int _modifiers );
	virtual void mouseDoubleClickFunc( const int _button,
									   const float _x,
									   const float _y,
									   const int _mousestate,
									   const int _modifiers );
	virtual void mouseMoveFunc( const float _x,
								const float _y,
								const int _mousestate,
								const int _modifiers );
	virtual void mouseWheelFunc( const int _delta,
								 const float _x,
								 const float _y,
								 const int _mousestate,
								 const int _modifiers );


protected:
private:

	//-- local keyboard states -------------------------------------------------


	//-- local mouse states ----------------------------------------------------
	float x_;
	float y_;


	//-- local variables -------------------------------------------------------
	CameraNode* cameraPtr_;
	CameraNode cameraCpy_;
	TransformNode* cameraPivotPtr_;
	TransformNode cameraPivotCpy_;

	float rotSpeed_;			// radians per pixel
	float zoomSpeed_;			// f(y) = e^(k*y) is distance from pivot
								// y screen pixels
								// k zoom speed

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================