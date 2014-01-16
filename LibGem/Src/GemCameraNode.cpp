//== INCLUDES ==================================================================

#include "GemCameraNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== IMPLEMENTATION ============================================================


//-- constructors --------------------------------------------------------------

CameraNode::CameraNode( )
	: SceneNode()
	// viewport
	, viewportWidth_( 512 )
	, viewportHeight_( 512 )
	, viewportAspect_( 1.0f )
	// frustum
	, frustumFOV_( Mem::PI/2 )
	, frustumNearZ_( 0.0001 )
	, frustumFarZ_( 10000 )
	// matrices
	, viewMatrix_(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
	, projMatrix_(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
	, textureMatrix_(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
{
}

CameraNode::~CameraNode( )
{}



//-- sets and gets -------------------------------------------------------------

void
CameraNode::setViewport( const unsigned int _viewportWidth,
						 const unsigned int _viewportHeight )
{
	viewportWidth_ = _viewportWidth;
	viewportHeight_ = _viewportHeight;

	if ( viewportHeight_ < 1 )
		 viewportHeight_ = 1;

	viewportAspect_ = static_cast<float>(viewportWidth_) / 
					  static_cast<float>(viewportHeight_);

	this->updateProjMatrix();
}

void
CameraNode::setFrustum( const float _frustumFOV,
						const float _nearFarZ,
						const float _frustumFarZ )
{
	frustumFOV_ = _frustumFOV;
	frustumNearZ_= _nearFarZ;
	frustumFarZ_ = _frustumFarZ;

	this->updateProjMatrix();
}

void
CameraNode::setFrustum( const Vec3f& _center, const float _radius )
{
	/* will set frustum such that it covers an object or objects with a center
	   and a radius

	       nearz  farz				  b
	 l .---.-----.--------		  l .---.----> dir
		\							 \  |
		 \  . - .					c \ | a
		  \| p. |					   \|
		   \. _ .						. p
			\
	*/

	// positions and direction
	Vec3f l( derivedPosition_ );
	Vec3f p( _center );
	Vec3f dir( derivedRotation_ * Vec3f(0,0,-1) );

	Vec3f c = p - l;
	Vec3f b = c.dot( dir ) * dir;
	Vec3f a = c - b;

	float an = a.norm();
	float bn = b.norm();
	float cn = c.norm();

	frustumFOV_ = 2 * atan2( an + _radius, bn );
	frustumNearZ_ = bn - _radius;
	frustumFarZ_ = bn + _radius;
}


//-- update related ------------------------------------------------------------

void
CameraNode::update( )
{
	SceneNode::update();
	this->updateViewMatrix();
}


//-- transformations -----------------------------------------------------------
Vec4f
CameraNode::unprojectWindow2World( const Vec2f& _positionWindow )
{
	/*
		model space
		world space
		view space (aka eye space)
		clip space
		normalized device space (ndc)
		windows coordinates
	*/


	// windows coordinates -> normalized device coordinates
	// x,y is in the [-1,1] range and z is ON the near plane i.e -1
	Vec4f p_ndc = Vec4f( 2.0f * (  _positionWindow[0]/viewportWidth_ ) - 1.0f,
					     2.0f * ( -_positionWindow[1]/viewportHeight_ ) + 1.0f,
					     -1.0f,
					     1.0f );


	// normalized device coordinates -> clip space
	// for points ON the clip plane (z=-nearz) the perspective divide is
	// w = -z = nearz
	Vec4f p_clip = p_ndc * viewportHeight_;


	// clip space -> view space
	Mat4f projinv = this->projMatrix_.inverse();
	Vec4f p_view = projinv * p_clip;


	// view space -> world space
	Mat4f viewinv = this->viewMatrix_.inverse();
	Vec4f p_world = viewinv * p_view;
	return p_world;
}

float
CameraNode::getForeshortening( const Vec3f& _positionWorld )
{
	/* Foreshorterning is the height of an object relative to its length along
	 * line of sight in perspective mode. It is the w component of the
	 * coordinates in homogemous form (x,y,z,w) after projection transform
	 */
	Vec4f p_clip = this->projMatrix_ * this->viewMatrix_
					* Vec4f( _positionWorld, 1 );
	return p_clip[3];
}


//-- view and proj matrices ----------------------------------------------------

void
CameraNode::updateViewMatrix( )
{
	viewMatrix_ = derivedTransform_.inverse();

	// update derived matrices
	Mat4f clip2tex( 0.5, 0.0, 0.0, 0.5,
					0.0, 0.5, 0.0, 0.5,
					0.0, 0.0, 0.5, 0.5,
					0.0, 0.0, 0.0, 1.0 );
	textureMatrix_ = clip2tex * projMatrix_ * viewMatrix_;
}

void
CameraNode::updateProjMatrix( )
{
	// opengl perspective projection matrix, same as gluPerspective()'
	// good reference at http://www.songho.ca/opengl/gl_projectionmatrix.html
	//
	float yScale = 1.0f / tan( 0.5f * frustumFOV_ );
	float xScale = yScale / viewportAspect_;
	float zPlan1 = ( frustumFarZ_ + frustumNearZ_ ) / 
				   ( frustumNearZ_ - frustumFarZ_ );
	float zPlan2 = 2 * frustumFarZ_ * frustumNearZ_ / 
					 ( frustumNearZ_ - frustumFarZ_ );

	projMatrix_ = Mat4f( xScale,	0,		0,		0,
						 0,			yScale,	0,		0,
						 0,			0,		zPlan1,	zPlan2,
						 0,			0,		-1,		0 );
	

	// update derived matrices
	Mat4f clip2tex( 0.5, 0.0, 0.0, 0.5,
					0.0, 0.5, 0.0, 0.5,
					0.0, 0.0, 0.5, 0.5,
					0.0, 0.0, 0.0, 1.0 );
	textureMatrix_ = clip2tex * projMatrix_ * viewMatrix_;
}

//void
//CameraNode::updateOrthoMatrix( )
//{
	// opengl orthographic projection matrix, same as glOrtho()'
	//float xScale = 2.0f / mWidth;
	//float yScale = 2.0f / mHeight;
	//float zPlan1 = 1 / ( mNearZ - mFarZ );
	//float zPlan2 = mNearZ / ( mNearZ - mFarZ );

	//Mat4f proj( xScale,	0,		0,		0,
	//			0,		yScale,	0,		0,
	//			0,		0,		zPlan1,	0,
	//			0,		0,		zPlan2,	1 );

	//return proj;
	//return Mat4f( 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 );
//}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================