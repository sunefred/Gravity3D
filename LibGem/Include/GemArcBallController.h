//==============================================================================
//
//	There are two different controllers, the Orbital and ArcBall. The Arcball
//	is a google earth style controller with rotate and zoom suited for rotating
//	around a single object. If the object in the scene has the same radius as 
//	the virtual sphere the mouse cursor will have a feeling of being glued to
//	that object during rotate.
//
//	Rotate: Alt + Left Mouse
//	Translate: Not allowed
//	Zoom: Alt + Right Mouse or Scroll Wheel
//
//==============================================================================

#ifndef GEM_ARCBALL_H
#define GEM_ARCBALL_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemController.h"
#include "GemTransformNode.h"
#include "GemCameraNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================


class ArcBallController : public Controller
{
public:


	//-- constructors ----------------------------------------------------------

	ArcBallController();
	~ArcBallController();


	//-- sets and gets ---------------------------------------------------------
	
	void setCamera( CameraNode* _cameraPtr )
	{ cameraPtr_ = _cameraPtr; }

	CameraNode* getCamera( ) const
	{ return cameraPtr_; }

	void setCameraPivot( TransformNode* _cameraPivotPtr )
	{ cameraPivotPtr_ = _cameraPivotPtr; }

	TransformNode* getCameraPivot( ) const
	{ return cameraPivotPtr_; }

	void setRadius( const float _radius )
	{ radius_ = _radius; }

	float getRadius( ) const
	{ return radius_; }


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
	float radius_;					// radius of ArcBall virtual sphere
	CameraNode* cameraPtr_;			// used for zoom and screenspace->world
	CameraNode cameraCpy_;
	TransformNode* cameraPivotPtr_;	// used for rotation
	TransformNode cameraPivotCpy_;

	float zoomSpeed_;				// Percent of distance to target per pixel
	
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================