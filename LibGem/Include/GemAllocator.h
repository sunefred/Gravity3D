//==============================================================================
//	
//	The Allocator object is the goto class for allocating dynamic data for
//  vertex data, texture data and more. It supplies templated read and write 
//	functions to access the underlying data as if it was a Vec3f, unsigned int
//	Mat4f or what have you.
//
//	A good reason for centralizing the allocation is to later implement an
//	allocator factory that you have to go through to create new data stores.
//	You could use this to print diagnostics for the application and for 
//	garbage collection.
//
//	Tracking updates to data
//	-----------------------
//	This class allow the access to data in many different ways, through 
//	read/write reference returns, read/write pointer access and dedicated
//	functions read(), write() etc. In addition the Allocator object keeps track
//	of any changes to the data. This is done through alloc, read and write
//	counters. The following functions increase theese counters
//
//	alloc:		alloc
//	read:		at, get, getReadPtr, read
//	write:		at, set, getWritePtr, write
//
//
//	TODO TODO TODO
//	--------------
//	In many cases it useful to have data live _only_ on the GPU, for instance
//	when doing transform feedback. For these cases it would be practical to
//	keep the Allocator---oLoader---oRenderState system intact. In this case the
//	Allocator would simply act a specification sheet for the type of data to 
//	allocate on GPU. Currently this will allocate the data on the CPU aswell
//	because there is not way to specify an allocator without allocating data.
//	In some cases this actually wanted because even in a transform feedback
//	loop you might want to download the geometry for debugging purposes.
//
//	The proposed change is to use on demand allocation. So the Allocator will
//	always start out empty even when defined for a specific type and size.
//	All writers will then have an initial boundary check and allocate memory as
//	required. Data should be allocated in chunks defined by some constant for
//	performance reasons.
//
//	A consequence is that all direct access operators should be disabled
//	because	we cant guarantee the memory state even within the size
//	specification.
//
//==============================================================================

#ifndef GEM_ALLOCATOR_H
#define GEM_ALLOCATOR_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITIONS =========================================================


class Allocator
{
public:


public:

	//-- define, typedef, enum -------------------------------------------------


	//-- constructors ----------------------------------------------------------

	Allocator( );
	
	Allocator( const Allocator& other );

	~Allocator( );


	//-- copy and clear --------------------------------------------------------

	void copy( const Allocator& other );

	void clear();


	//-- assignment operator ---------------------------------------------------

	Allocator& operator=(const Allocator& rhs);


	//-- direct access operators -----------------------------------------------
	
	// access to element through alloc.at<T>(pos), read-write
	template<typename T>
	T& at( const unsigned int pos )
	{
		// boundary check
		assert( (pos+1)*sizeof(T) <= byteCount_ );

		// update activity counters
		readCount_++;
		writeCount_++;

		// return value at pos
		T* ptr = static_cast<T*>(ptr_);
		return ptr[pos];
	}

	// access to element through alloc.at<T>(i,j), read-write
	template<typename T>
	T& at( const unsigned int i, const unsigned int j )
	{
		// return values at [i,j]
		return this->at( i * width_ + j );
	}

	// access to element through alloc.set<T>(pos,T), write-only
	template<typename T>
	void set( const unsigned int pos, const T& value )
	{
		// boundary check
		assert( (pos+1)*sizeof(T) <= byteCount_ );

		// update activity counters
		writeCount_++;

		// set value at pos
		T* ptr = static_cast<T*>(ptr_);
		ptr[pos] = value;
	}

	// access to element through alloc.set<T>(i,j,T), write-only
	template<typename T>
	void set( const unsigned int i, const unsigned int j, const T& value )
	{
		// set values at [i,j]
		this->set( i * width_ + j, value );
	}

	// access to element through alloc.get<T>(pos), read-only
	template<typename T>
	T& get( const unsigned int pos )
	{
		// boundary check
		assert( (pos+1)*sizeof(T) <= byteCount_ );

		// update activity counters
		readCount_++;

		// return read only reference to value at pos
		T* ptr = static_cast<T*>(ptr_);
		return ptr[pos];
	}

	// access to element through alloc.get<T>(i,j), read-only
	template<typename T>
	T& get( const unsigned int i, const unsigned int j )
	{
		// return read only reference to value at [i,j]
		return this->get( i * width_ + j );
	}


	//-- access via pointer ----------------------------------------------------

	// pointer to first element, read/write
	template<typename T>
	T* getWritePtr()
	{
		// update activity counters
		readCount_++;
		writeCount_++;

		// return read/write pointer
		return static_cast<T*>(ptr_);
	}

	// pointer to first element, read only
	// disallow caller to change content, changing pointer i fine
	template<typename T>
	const T* getReadPtr()
	{
		// update activity counters
		readCount_++;

		// return read only pointer
		return static_cast<T*>(ptr_);
	}

	// pointer a pointer to the first element, pointer to const pointer
	// disallow caller to change class pointer to content, makes sense yes?
	//template<typename T>
	//const T ** getPtrPtr() const { return static_cast<T**>(&ptr_); }


	//-- file style access -----------------------------------------------------

	template<typename T>
	bool read( T* e )
	{
		// local variables
		T* ptr = static_cast<T*>(ptr_);
		T* cur = static_cast<T*>(cur_);

		// boundary check
		unsigned int pos =  cur - ptr;
		if ( (pos+1)*sizeof(T) > byteCount_ )
			return false;

		// update activity counters
		readCount_++;

		// read element and increase pointer
		*e = *cur;
		cur_ = static_cast<void*>(++cur);
		return true;
	}

	// mass read from pointer. no boundary check on pointer (obviously)
	template<typename T>
	bool read( T* e, const unsigned int count,
			   const unsigned int offset = 0, const unsigned int stride = 1 )
	{
		for ( unsigned int i = offset; i < count; i+=stride )
		{
			if ( !read<T>( &(e[i]) ) )
				return false;
		}
		return true;
	}

	template<typename T>
	bool write( const T& e )
	{
		// local variables
		T* ptr = static_cast<T*>(ptr_);
		T* cur = static_cast<T*>(cur_);

		// boundary check
		unsigned int pos =  cur - ptr;
		if ( (pos+1)*sizeof(T) > byteCount_  )
			return false;

		// update activity counters
		writeCount_++;

		// write element and increase pointer
		*cur = e;
		cur_ = static_cast<void*>(++cur);
		return true;
	}

	// mass write from pointer. no boundary check on pointer (obviously)
	template<typename T>
	bool write( const T* e, const unsigned int count,
				const unsigned int offset = 0, const unsigned int stride = 1 )
	{
		for ( unsigned int i = offset; i < count; i+=stride )
		{
			if ( !write<T>( e[i] ) )
				return false;
		}
		return true;
	}

	// re-position read/write pointer using seek<T>(pos)
	template<typename T>
	bool seek( const unsigned int pos )
	{
		// boundary check
		if ( (pos+1)*sizeof(T) > byteCount_  )
			return false;

		// reposition current element pointer
		T* ptr = static_cast<T*>(ptr_);
		cur_ = static_cast<void*>(&ptr[pos]);
		return true;
	}

	// re-position read/write pointer using seek<T>(i,j)
	template<typename T>
	bool seek( const unsigned int i, const unsigned int j )
	{
		return this->seek<T>( i * width_ + j );
	}


	//-- normal gets and sets --------------------------------------------------

	ALLOC_FORMAT getFormat( ) const { return format_; }
	
	ALLOC_DIM getDim( ) const { return dim_; }

	ALLOC_TYPE getType( ) const { return type_; }

	unsigned int getWidth( ) const { return width_; }

	unsigned int getHeight( ) const { return height_; }

	unsigned int getBitsPerElement( ) const { return bitsPerElement_; }

	unsigned int getElementCount( ) const { return elementCount_; }

	unsigned int getByteCount( ) const { return byteCount_; };

	unsigned int getAllocCount() const { return allocCount_; }

	const unsigned int* getAllocCountPtr() const { return &allocCount_; }

	unsigned int getReadCount() const { return readCount_; }

	const unsigned int* getReadCountPtr() const { return &readCount_; }

	unsigned int getWriteCount() const { return writeCount_; }

	const unsigned int* getWriteCountPtr() const { return &writeCount_; }

	bool isAlloc( ) const { return isAlloc_; }


	//-- alloc/dealloc ---------------------------------------------------------

	void alloc( ALLOC_FORMAT _format,
				const unsigned int _width = 0,
				const unsigned int _height = 1 );

protected:
private:


	//-- local variables -------------------------------------------------------

	// pointer to allocated data
	// pointer to current element
	void* ptr_;
	void* cur_;

	// data settings
	ALLOC_FORMAT format_;
	unsigned int width_;
	unsigned int height_;

	// derived data settins
	ALLOC_DIM dim_;
	ALLOC_TYPE type_;
	unsigned int bitsPerElement_;
	unsigned int elementCount_;
	unsigned int byteCount_;
	
	// Activity counters for classes that want to track changes to the data.
	unsigned int allocCount_;
	unsigned int readCount_;
	unsigned int writeCount_;

	// isAlloc - there is data in the Allocator
	bool isAlloc_;
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================