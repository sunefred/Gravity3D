//==============================================================================
//
//	The Tracker class is a simple class intended to answer the simple question
//	"Did this variable change since last time?". This is useful for tracking 
//	state changes to the same variable from different places without ever
//	resetting the tracked variable. This is done by creating multiple trackers
//	for the same variable. Each tracker keeps its own copy of the tracked
//	variable to compare against.
//
//==============================================================================


#ifndef GEM_TRACKER_H
#define GEM_TRACKER_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATIONS ========================================================

template<typename T>
class Tracker
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor ok
	Tracker()
	: ptr_( NULL )
	, cpy_() {}

	// specialized constructors
	Tracker( const T* _ptr )
	: ptr_( _ptr )
	, cpy_()
	{
		if ( _ptr )
		{
			cpy_ = *_ptr;
		}
	}

	Tracker( const T* _ptr, T& _init )
	: ptr_( _ptr )
	, cpy_( _init )
	{
	}

	// default destructor ok
	~Tracker() {}


	//-- copy and clear --------------------------------------------------------

	void clear()
	{
		ptr_ = NULL;
		cpy_ = T();
	}


	//-- sets and gets ---------------------------------------------------------

	void set( const T* _ptr )
	{
		if ( _ptr )
		{
			ptr_ =  _ptr;
			cpt_ = *_ptr;
		}
	}
	
	void set( const T* _ptr, const T& _init )
	{
		if ( _ptr )
		{
			ptr_ = _ptr;
			cpy_ = _init;
		}
	}


public:

	//-- special comparators with update ---------------------------------------

	bool equalsPtrCpy( bool _update = true )
	{
		bool retval = false;
		if ( ptr_ )
		{
			retval = ( cpy_ == *ptr_ );
			if ( _update )
			{
				cpy_ = *ptr_;
			}
		}
		return retval;
	}

	bool lessPtrCpy( bool _update = true )
	{
		bool retval = false;
		if ( ptr_ )
		{
			retval = ( *ptr_ < cpy_ );
			if ( _update )
			{
				cpy_ = *ptr_;
			}
		}
		return retval;
	}

	bool greaterPtrCpy( bool _update = true )
	{
		bool retval = false;
		if ( ptr_ )
		{
			retval = ( *ptr_ > cpy_ );
			if ( _update )
			{
				cpy_ = *ptr_;
			}
		}
		return retval;
	}

	bool nequalsPtrCpy( bool _update = true )
	{
		return !equalsPtrCpy( _update );
	}

	bool lequalPtrCpy( bool _update = true )
	{
		return !greaterPtrCpy( _update );
	}

	bool gequalPtrCpy( bool _update = true )
	{
		return !lessPtrCpy( _update );
	}


private:

	const T* ptr_;
	T  cpy_;

};


//== TYPEDEFS ==================================================================

/** scalars */
typedef Tracker<signed int> Trackeri;
typedef Tracker<unsigned int> Trackerui;
typedef Tracker<float> Trackerf;
typedef Tracker<double> Trackerd;


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================