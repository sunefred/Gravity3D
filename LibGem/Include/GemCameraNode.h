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
	
	// viewport
	void setViewport( const unsigned int _viewportWidth,
					  const unsigned int _viewportHeight );

	unsigned int getViewportWidth( ) const
	{ return viewportWidth_; }

	const unsigned int* getViewportWidthPtr() const
	{ return &viewportWidth_; }

	unsigned int getViewportHeight( ) const
	{ return viewportHeight_; }

	const unsigned int* getViewportHeightPtr() const
	{ return &viewportHeight_; }

	float getViewportAspect( ) const
	{ return viewportAspect_; }


	// frustum
	void setFrustum( const float _frustumFOV,
					 const float _frustumNearZ,
					 const float _frustumFarZ );
	void setFrustum( const Vec3f& _center, const float _radius );

	float getFrustumFOV() const { return frustumFOV_; }
	float getFrustumNearZ() const { return frustumNearZ_; }
	float getFrustumFarZ() const { return frustumFarZ_; }


	// matrices
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

	void updateViewMatrix();
	void updateProjMatrix();


private:

	// viewport
	unsigned int viewportWidth_;	// width of camera "sensor"
	unsigned int viewportHeight_;	// height of camera "sensor"
	float viewportAspect_;			// width/height

	// frustum
	float frustumFOV_;				// field of view where 0 < fov < PI
	float frustumNearZ_;			// near plane of frustum
	float frustumFarZ_;				// far plane of frustum
	
	// matrices
	Mat4f viewMatrix_;				// updated on update
	Mat4f projMatrix_;				// updated on setSize and setFrustum
	Mat4f textureMatrix_;			// updated on update, setSize and setFrustum
	
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================