//== INCLUDES ==================================================================

#include "GemTransformNode.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== NODE IMPLEMENTATION =======================================================

//-- constructors/destructors --------------------------------------------------

TransformNode::TransformNode( )
	: name_("")
	, parent_(NULL)
	, position_(0,0,0)
	, scale_(1,1,1)
	, rotation_(1,0,0,0)
	, transform_(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
	, derivedPosition_(0,0,0)
	, derivedScale_(1,1,1)
	, derivedRotation_(1,0,0,0)
	, derivedTransform_(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
	, needUpdate_(true)
{}

TransformNode::~TransformNode( )
{}


//-- sets and gets -------------------------------------------------------------

void
TransformNode::setPosition( const Vec3f& _position )
{
	position_ = _position;
	needUpdate();
}

Vec3f
TransformNode::getPosition( ) const
{
	return position_;
}

void
TransformNode::setScale( const Vec3f& _scale )
{
	scale_ = _scale;
	needUpdate();
}

Vec3f
TransformNode::getScale( ) const
{
	return scale_;
}

void
TransformNode::setRotation( const Quatf& _rotation )
{
	rotation_ = _rotation;
}

Quatf
TransformNode::getRotation( ) const
{
	return rotation_;
}

Mat4f
TransformNode::getTransform( )
{
	if ( needUpdate_ ) update(); 
	return transform_;
}

Mat4f*
TransformNode::getTransformPtr( )
{ 
	if ( needUpdate_ ) update(); 
	return &transform_;
}

Vec3f
TransformNode::getDerivedPosition( )
{
	if ( needUpdate_ ) update(); 
	return derivedPosition_;
}

Vec3f*
TransformNode::getDerivedPositionPtr( )
{
	if ( needUpdate_ ) update(); 
	return &derivedPosition_;
}

Vec3f
TransformNode::getDerivedScale( )
{
	if ( needUpdate_ ) update(); 
	return derivedScale_;
}

Vec3f*
TransformNode::getDerivedScalePtr( )
{
	if ( needUpdate_ ) update(); 
	return &derivedScale_;
}

Quatf
TransformNode::getDerivedRotation( ) 
{
	if ( needUpdate_ ) update(); 
	return derivedRotation_;
}

Quatf*
TransformNode::getDerivedRotationPtr( )
{ 
	if ( needUpdate_ ) update(); 
	return &derivedRotation_;
}

Mat4f
TransformNode::getDerivedTransform( )
{ 
	if ( needUpdate_ ) update(); 
	return derivedTransform_;
}

Mat4f*
TransformNode::getDerivedTransformPtr( )
{ 
	if ( needUpdate_ ) update(); 
	return &derivedTransform_;
}


//-- translate and rotate ------------------------------------------------------

void
TransformNode::translate( const float _x, const float _y, const float _z,
						  const TRANSFORM_MODE _transformMode )
{
	this->translate( Vec3f( _x, _y, _z ), _transformMode );
}

void
TransformNode::translate( const Vec3f& _translation,
						  const TRANSFORM_MODE _transformMode )
{
	switch ( _transformMode )
	{
	case TRANSFORM_MODE_LOCAL:
		position_ += rotation_ * _translation;
		break;
	case TRANSFORM_MODE_PARENT:
		position_ += _translation;
		break;
	case TRANSFORM_MODE_WORLD:
		if ( parent_ )
		{
			position_ += (parent_->getDerivedRotation().inverse()
						 * _translation ) / parent_->getDerivedScale();
		}
		else
		{
			position_ += _translation;
		}
		break;
	default:
		break;
	}
	needUpdate();
}

void
TransformNode::rotate( const float _angle,
					   const float _x, const float _y, const float _z,
					   const TRANSFORM_MODE _transformMode )
{
	this->rotate( Quatf( _angle, Vec3f( _x, _y, _z ) ), _transformMode );
}

void
TransformNode::rotate( const float _angle, const Vec3f& _axis,
					   const TRANSFORM_MODE _transformMode )
{
	this->rotate( Quatf( _angle, _axis ), _transformMode );
}

void
TransformNode::rotate( Quatf& _rotation,
					   const TRANSFORM_MODE _transformMode )
{
	// Normalize quaternion to avoid drivt
	Quatf rotationNormalized = _rotation;
	rotationNormalized.normalize();
		
	switch ( _transformMode )
	{
	case TRANSFORM_MODE_LOCAL:
		rotation_ = rotation_ * rotationNormalized;
		break;
	case TRANSFORM_MODE_PARENT:
		/*
		 * p_world  = R_parent * R * R^-1 * r * R * p = 
		 *			= R_parent * r * R * p
		 *						 '-.-'
		 *						    R_new
		 */
		rotation_ = rotationNormalized * rotation_;
		break;
	case TRANSFORM_MODE_WORLD:
		/*
		 * p_world  = R_parent * R * R_tot^-1 * r * R_tot * p = 
		 *						 '-----------.-----------'
		 *						             R_new
		 */
		rotation_ = rotation_ * this->getDerivedRotation().inverse() *
					rotationNormalized * this->getDerivedRotation();
		break;
	default:
		break;
	}
	needUpdate();
}



//-- TransformNode related --------------------------------------------------------------

void
TransformNode::addChild( TransformNode* _transformNodePtr,
						 const bool _preserveWorldPosition )
{
	if ( _transformNodePtr )
	{
		_transformNodePtr->setParent( this, _preserveWorldPosition );
		children_.push_back( _transformNodePtr );
	}
}

void
TransformNode::setParent( TransformNode* _transformNodePtr,
						  const bool _preserveWorldPosition )
{
	if ( _transformNodePtr )
	{
		parent_ = _transformNodePtr;

		if ( _preserveWorldPosition )
		{
			Vec3f parentPosition = parent_->getDerivedPosition();
			Quatf parentRotation = parent_->getDerivedRotation();

			rotation_ = parentRotation.inverse() * rotation_;
			position_ = parentRotation.inverse() * (position_ - parentPosition);
		}
	}
	needUpdate();
}

TransformNode*
TransformNode::getChildPtr() const
{
	std::vector<TransformNode*>::const_iterator it = children_.begin();
	return (*it);
}

TransformNode*
TransformNode::getParentPtr() const
{
	return parent_;
}

//-- update related ------------------------------------------------------------

void
TransformNode::update( )
{
	// update transform
	if ( needUpdate_ )
	{
		if ( parent_ )
		{
			Vec3f parentPosition = parent_->getDerivedPosition();
			Vec3f parentScale = parent_->getDerivedScale();
			Quatf parentRotation = parent_->getDerivedRotation();


			// update rotation. this backwards rotation is the same as rotating in 
			// the parent coordinate system, that is first rotate using parent
			// rotation and then rotate in the base spanned by the rotated xyz-axis
			derivedRotation_ =  parentRotation * rotation_;


			// update scale
			derivedScale_ = parentScale * scale_;


			// update position
			derivedPosition_ = parentPosition + 
							   parentRotation *
							   ( parentScale * position_ );
							   
		}
		else
		{
			derivedRotation_ = rotation_;
			derivedScale_ = scale_;
			derivedPosition_ = position_;
		}
		transform_.fromPosScaleRot( position_, scale_, rotation_ );
		derivedTransform_.fromPosScaleRot( derivedPosition_ ,
										   derivedScale_,
										   derivedRotation_);


		// reset update BEFORE calling children or we will deadlock
		this->resetUpdate();
	}


	// update children
	for ( std::vector<TransformNode*>::iterator
		  it=children_.begin(), iend=children_.end(); it!=iend; ++it )
	{
		(*it)->update();
	}
}

void
TransformNode::needUpdate( )
{
	// this node has changed so set flag for it AND its children
	needUpdate_ = true;
	for ( std::vector<TransformNode*>::iterator
		  it=children_.begin(), iend=children_.end(); it!=iend; ++it )
	{
		(*it)->needUpdate();
	}
}

void
TransformNode::resetUpdate()
{
	// we dont allow a reset bubble down, reset is called when this node is 
	// updated which will reset chilren as well.
	needUpdate_ = false;
}

//==============================================================================
}		// end of namespace Gem
//==============================================================================