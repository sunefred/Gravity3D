//== INCLUDES ==================================================================

#include "GemAllocator.h"


//== NAMESPACES ================================================================


namespace Gem {


//== IMPLEMENTATION ============================================================


//-- constructors --------------------------------------------------------------

Allocator::Allocator( )
: ptr_(NULL)
, cur_(NULL)
, format_(ALLOC_FORMAT_NONE)
, width_(0)
, height_( 0 )
, dim_(ALLOC_DIM_NONE)
, type_(ALLOC_TYPE_NONE)
, bitsPerElement_(0)
, numElements_(0)
, byteCount_(0)
, allocCount_(0)
, readCount_(0)
, writeCount_(0)
, isAlloc_(false)
{}

Allocator::Allocator( const Allocator& other )
{
	this->copy( other );
}

Allocator::~Allocator( )
{
	this->clear();
}

//-- copy and clear ------------------------------------------------------------

void
Allocator::copy( const Allocator& other )
{
	if ( isAlloc_ )
		this->clear();


	// allocate data
	unsigned char* newPtr;
	newPtr = new unsigned char[other.byteCount_];
	cur_ = ptr_ = static_cast<void*>(newPtr);


	// copy data
	unsigned char* otherPtr = static_cast<unsigned char*>(other.ptr_);
	for ( unsigned int i=0; i<byteCount_; ++i )
	{
		newPtr[i] = otherPtr[i];
	}


	// copy stack member variables
	format_ = other.format_;
	width_ = other.width_;
	height_ = other.height_;
	dim_ = other.dim_;
	type_ = other.type_;
	bitsPerElement_ = other.bitsPerElement_;
	numElements_ = other.numElements_;
	byteCount_ = other.byteCount_;
	allocCount_ = other.allocCount_;
	readCount_ = other.readCount_;
	writeCount_ = other.writeCount_;
	isAlloc_ = other.isAlloc_;
}

void
Allocator::clear( )
{	
	// deallocate data
	delete [] ptr_;

	// reset member variables
	ptr_				= NULL;
	cur_				= NULL;
	format_				= ALLOC_FORMAT_NONE;
	width_				= 0;
	height_				= 0;
	dim_				= ALLOC_DIM_NONE;
	type_				= ALLOC_TYPE_NONE;
	bitsPerElement_		= 0;
	numElements_		= 0;
	byteCount_			= 0;
	allocCount_			= allocCount_; // dont reset, lifetime counter
	readCount_			= readCount_; // dont reset, lifetime counter
	writeCount_			= writeCount_; // dont reset, lifetime counter
	isAlloc_			= false;
}

//-- assignment operator -------------------------------------------------------

Allocator&
Allocator::operator=( const Allocator& rhs )
{
	if ( this != &rhs )
	{
		this->clear();
		this->copy( rhs );
	}
	return *this;
}

//-- alloc ---------------------------------------------------------------------

void
Allocator::alloc( ALLOC_FORMAT _format,
				  const unsigned int _width, 
				  const unsigned int _height )
{
	if ( isAlloc_ )
		this->clear();

	
	// local variables
	ALLOC_FORMAT format = _format;
	unsigned int width = _width;
	unsigned int height = _height;
	ALLOC_DIM dim;
	ALLOC_TYPE type;
	unsigned int bitsPerElement;
	unsigned int numElements;
	unsigned int byteCount;


	// setup data format variables
	switch ( format )
	{
	// scalar formats
	case ALLOC_FORMAT_SCALAR_8I:
		dim = ALLOC_DIM_SCALAR;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 8;
		break;
	case ALLOC_FORMAT_SCALAR_8UI:
		dim = ALLOC_DIM_SCALAR;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 8;
		break;
	case ALLOC_FORMAT_SCALAR_32I:
		dim = ALLOC_DIM_SCALAR;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_SCALAR_32UI:
		dim = ALLOC_DIM_SCALAR;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_SCALAR_32F:
		dim = ALLOC_DIM_SCALAR;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 32;
		break;

	// vector formats
	case ALLOC_FORMAT_VEC2_8I:
		dim = ALLOC_DIM_VEC2;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 16;
		break;
	case ALLOC_FORMAT_VEC2_8UI:
		dim = ALLOC_DIM_VEC2;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 16;
		break;
	case ALLOC_FORMAT_VEC2_32I:
		dim = ALLOC_DIM_VEC2;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 64;
		break;
	case ALLOC_FORMAT_VEC2_32UI:
		dim = ALLOC_DIM_VEC2;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 64;
		break;
	case ALLOC_FORMAT_VEC2_32F:
		dim = ALLOC_DIM_VEC2;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 64;
		break;
	case ALLOC_FORMAT_VEC3_8I:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 24;
		break;
	case ALLOC_FORMAT_VEC3_8UI:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 24;
		break;
	case ALLOC_FORMAT_VEC3_32I:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 96;
		break;
	case ALLOC_FORMAT_VEC3_32UI:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 96;
		break;
	case ALLOC_FORMAT_VEC3_32F:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 96;
		break;
	case ALLOC_FORMAT_VEC4_8I:
		dim = ALLOC_DIM_VEC4;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_VEC4_8UI:
		dim = ALLOC_DIM_VEC4;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_VEC4_32I:
		dim = ALLOC_DIM_VEC4;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_VEC4_32UI:
		dim = ALLOC_DIM_VEC4;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_VEC4_32F:
		dim = ALLOC_DIM_VEC4;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 128;
		break;

	// matrix formats
	case ALLOC_FORMAT_MAT2_8I:
		dim = ALLOC_DIM_MAT2;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_MAT2_8UI:
		dim = ALLOC_DIM_MAT2;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 32;
		break;
	case ALLOC_FORMAT_MAT2_32I:
		dim = ALLOC_DIM_MAT2;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_MAT2_32UI:
		dim = ALLOC_DIM_MAT2;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_MAT2_32F:
		dim = ALLOC_DIM_MAT2;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_MAT3_8I:
		dim = ALLOC_DIM_MAT3;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 72;
		break;
	case ALLOC_FORMAT_MAT3_8UI:
		dim = ALLOC_DIM_MAT3;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 72;
		break;
	case ALLOC_FORMAT_MAT3_32I:
		dim = ALLOC_DIM_MAT3;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 288;
		break;
	case ALLOC_FORMAT_MAT3_32UI:
		dim = ALLOC_DIM_MAT3;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 288;
		break;
	case ALLOC_FORMAT_MAT3_32F:
		dim = ALLOC_DIM_MAT3;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 288;
		break;
	case ALLOC_FORMAT_MAT4_8I:
		dim = ALLOC_DIM_MAT4;
		type = ALLOC_TYPE_8I;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_MAT4_8UI:
		dim = ALLOC_DIM_MAT4;
		type = ALLOC_TYPE_8UI;
		bitsPerElement = 128;
		break;
	case ALLOC_FORMAT_MAT4_32I:
		dim = ALLOC_DIM_MAT4;
		type = ALLOC_TYPE_32I;
		bitsPerElement = 512;
		break;
	case ALLOC_FORMAT_MAT4_32UI:
		dim = ALLOC_DIM_MAT4;
		type = ALLOC_TYPE_32UI;
		bitsPerElement = 512;
		break;
	case ALLOC_FORMAT_MAT4_32F:
		dim = ALLOC_DIM_MAT4;
		type = ALLOC_TYPE_32F;
		bitsPerElement = 512;
		break;

	// compressed texture formats
	case ALLOC_FORMAT_VEC3_DXT1:
		dim = ALLOC_DIM_VEC3;
		type = ALLOC_TYPE_DXT1;
		bitsPerElement = 4;
		break;

	default:
		GEM_THROW( "Unsupported data format." );
		break;
	};


	// calculate number of elements and byte count
	numElements = width * height;
	byteCount = std::ceil( numElements * (bitsPerElement / 8.0f) );


	// allocate data
	unsigned char* ptr;
	ptr = new unsigned char[byteCount];
	cur_ = ptr_ = static_cast<void*>(ptr);


	// set to zero
	for ( unsigned int i=0; i<byteCount; ++i )
	{
		ptr[i] = 0;
	}


	// set member variables
	format_ = format;
	width_ = width;
	height_ = height;
	dim_ = dim;
	type_ = type;
	bitsPerElement_ = bitsPerElement;
	numElements_ = numElements;
	byteCount_ = byteCount;
	allocCount_ += 1; // dont reset, lifetime counter
	readCount_ = readCount_; // dont reset, lifetime counter
	writeCount_ = writeCount_; // dont reset, lifetime counter
	isAlloc_ = true;
}

//==============================================================================
}		// end of namespace Gem
//==============================================================================