//==============================================================================
//
//	This is a TRANSFORM node and nothing else. You can not connect meshes to it
//	or other things for rendering. It holds the transforms and a list of
//	children. If you want to use it to affect rendering you could load its
//	transform into the OpenGL stack before drawing.
//
//	There is an update policy that flags a node and its children for update
//	whenever its translation, scale or rotation is changed. Say you are
//	rotating a node and then request the derived transform for one of its
//	children. Both the node and the child will be updated before returning the
//	derived transform.
//
//	You will never be able to get an out of date transform for any node in the 
//	tree, but it is still recommended to run root.update() at the beginning of
//	the draw loop to update all nodes in one go. A node will not run the update
//	code if it is not needed however.
//
//==============================================================================

#ifndef GEM_TRANSFORMNODE_H
#define GEM_TRANSFORMNODE_H


//== INCLUDES ==================================================================


#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== NODE DEFINITION ===========================================================


class TransformNode
{
public:

	//-- constructors/destructors ----------------------------------------------

	TransformNode( );
	~TransformNode( );

	
	//-- comparison operators --------------------------------------------------

	bool operator== ( const TransformNode& _rhs ) const
	{
		return ( name_ == _rhs.name_ &&
				 parent_ == _rhs.parent_ &&
				 children_ == _rhs.children_ &&
				 position_ == _rhs.position_ &&
				 scale_ == _rhs.scale_ &&
				 rotation_ == _rhs.rotation_ &&
				 transform_ == _rhs.transform_ &&
				 derivedPosition_ == _rhs.derivedPosition_ &&
				 derivedScale_ == _rhs.derivedScale_ &&
				 derivedRotation_ == _rhs.derivedRotation_ &&
				 derivedTransform_ == _rhs.derivedTransform_ );
	}

	// component-wise non-equality
	bool operator!= ( const TransformNode& _rhs ) const
	{
		return !( *this == _rhs );
	}

	//-- sets and gets ---------------------------------------------------------

	void setName( const std::string& _name )
	{ name_ = _name; }

	std::string getName( void )
	{ return name_; }

	void setPosition( const Vec3f& _position );

	Vec3f getPosition( ) const;

	void setScale( const Vec3f& _scale );

	Vec3f getScale( ) const;

	void setRotation( const Quatf& _rotation );

	Quatf getRotation( ) const;

	Mat4f getTransform( );

	Mat4f* getTransformPtr( );

	Vec3f getDerivedPosition( );

	Vec3f* getDerivedPositionPtr( );

	Vec3f getDerivedScale( );

	Vec3f* getDerivedScalePtr( );

	Quatf getDerivedRotation( );

	Quatf* getDerivedRotationPtr( );

	Mat4f getDerivedTransform( );

	Mat4f* getDerivedTransformPtr( );



	//-- translate and rotate --------------------------------------------------

	void translate( const float _x, const float _y, const float _z,
				 const TRANSFORM_MODE _transformMode = TRANSFORM_MODE_PARENT );

	void translate( const Vec3f& _translation,
			     const TRANSFORM_MODE _transformMode = TRANSFORM_MODE_PARENT );

	void rotate( const float _angle,
				 const float _x, const float _y, const float _z,
				 const TRANSFORM_MODE _transformMode = TRANSFORM_MODE_LOCAL );

	void rotate( const float _angle, const Vec3f& _axis,
				 const TRANSFORM_MODE _transformMode = TRANSFORM_MODE_LOCAL );

	void rotate( Quatf& _rotation,
				 const TRANSFORM_MODE _transformMode = TRANSFORM_MODE_LOCAL );


	//-- node related ----------------------------------------------------------

	void addChild( TransformNode* _transformNodePtr,
				   const bool preserveWorldPosition=false );

	void setParent( TransformNode* _transformNodePtr,
					const bool preserveWorldPosition=false );

	TransformNode* getChildPtr( ) const;

	TransformNode* getParentPtr( ) const;


	//-- update related --------------------------------------------------------

	virtual void update( );			// updates transformations

	virtual void needUpdate( );		// flag that node needs update
	
	virtual void resetUpdate( );	// un-flag that node needs update


protected:

	std::string name_;
	TransformNode* parent_;
	std::vector<TransformNode*> children_;

	Vec3f position_;			// local position
	Vec3f scale_;				// local scale
	Quatf rotation_;			// local rotation
	Mat4f transform_;			// local transform

	Vec3f derivedPosition_;		// global position
	Vec3f derivedScale_;		// global scale
	Quatf derivedRotation_;		// global rotation
	Mat4f derivedTransform_;	// global transform

	bool needUpdate_;			// flag weather node transform is out of date

private:

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================