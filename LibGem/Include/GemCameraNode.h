//==============================================================================
//
//	The CameraNode class extends the SceneNode class with computations for view
//	projection and texture matrices.
//
//==============================================================================

#ifndef GEM_CAMERANODE_H
#define GEM_CAMERANODE_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemSceneNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================

class CameraNode : public SceneNode
{
public:


	//-- constructors ----------------------------------------------------------

	CameraNode( );
	~CameraNode( );


	//-- sets and gets ---------------------------------------------------------

	void setSize( const float _width, const float _height );

	void setWidth( const float _width );

	void setHeight( const float _height );

	float getWidth( ) const { return width_; }

	float getHeight( ) const { return height_; }

	float getAspect( ) const { return aspect_; }

	void setFrustum( const float _FOV, const float _nearz, const float _farz );

	void setFrustum( const Vec3f& _center, const float _radius );

	float getFOV() const { return FOV_; }

	float getNearZ() const { return nearZ_; }
	
	float getFarZ() const { return farZ_; }

	Mat4f getViewMatrix( ) const { return viewMatrix_; }

	Mat4f* getViewMatrixPtr( ) { return &viewMatrix_; }

	Mat4f getProjMatrix( ) const { return projMatrix_; }

	Mat4f* getProjMatrixPtr( ) { return &projMatrix_; }

	Mat4f getTextureMatrix( ) const { return textureMatrix_; }

	Mat4f* getTextureMatrixPtr( ) { return &textureMatrix_; }


	//-- update related --------------------------------------------------------

	virtual void update( );		// updates transformations


	//-- transformations -------------------------------------------------------

	virtual Vec4f unprojectWindow2World( const Vec2f& _positionWindow );

	virtual float getForeshortening( const Vec3f& _positionWorld );


protected:
private:

	//-- view and proj matrices ------------------------------------------------

	void updateViewMatrix( );

	void updateProjMatrix( );


	//-- member variables ------------------------------------------------------

	float width_;					// width of camera "sensor"
	float height_;					// height of camera "sensor"
	float aspect_;					// width/height
	float FOV_;						// field of view where 0 < fov < PI
	float nearZ_;					// near plane of frustum
	float farZ_;					// far plane of frustum
	
	Mat4f viewMatrix_;				// updated on update
	Mat4f projMatrix_;				// updated on setSize and setFrustum
	Mat4f textureMatrix_;			// updated on update, setSize and setFrustum
	
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================