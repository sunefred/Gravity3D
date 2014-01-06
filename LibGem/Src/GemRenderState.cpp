//== INCLUDES ==================================================================

#include "GemRenderState.h"
#include "GemMeshLoader.h"
#include "GemTextureLoader.h"
#include "GemShaderLoader.h"
#include "GemTracker.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================


//-- constructors/destructor -----------------------------------------------

RenderState::RenderState()
// all classes with good constructors, no pointers
{
}

RenderState::~RenderState()
{
	// UNDECLARE
	// ----------
	// What: Release buffer storage, textures storage, free all ID's
	// When: When re-declaring or when destructing RenderState
	undeclareBuffers();
	undeclareVertexData();
	undeclareShaderProgram();
	undeclareUniforms();
	undeclareTextures();
	//undeclareBufferTextures();
	//undeclareTransformFeedback();
	undeclareFramebuffer();
}


//-- the star function of the entire library -----------------------------------

void
RenderState::draw()
{
	// DECLARE
	// -------
	// What: Defines buffers, textures, compiles and links shader program etc
	// When: Allocated data format/size has changed or has just been loaded
	declareBuffers();
	declareVertexData();
	declareShaderProgram();
	declareUniforms();
	declareTextures();
	//declareBufferTextures();
	//declareTransformFeedback();
	declareFramebuffer();


	// UPLOAD
	// -------
	// What: Copy data from Allocator (RAM) ---> OpenGL Buffer
	// When: Allocated data has been written to
	uploadBuffers();


	// UNPACK
	// ------
	// What: "Unpack" OpenGL Buffer to OpenGL VRAM/ON-DIE, concerns pixel data
	// When: Buffer data has been written too
	unpackTextures();
	//unpackBufferTextures();


	// BIND
	// ----
	// What: glBindVertexArray(), glUseProgram(), upload uniforms, ...
	// When: Every frame
	bindVertexData();
	bindShaderProgram();
	bindUniforms();
	bindTextures();
	//bindBufferTextures();
	//bindTransformFeedback();
	bindFramebuffer();


	// DRAW
	// ----
	// What: glDrawElements() flushes all the data through the pipeline
	// When: Every frame
	if ( programState_.hasTesselator() )
	{
		glPatchParameteri( GL_PATCH_VERTICES,
						   vertexState_.getIndexDim() );
		glDrawElements( GL_PATCHES,
						vertexState_.getIndexDim()*
						vertexState_.getIndexCount(),
						vertexState_.getIndexType(),
						0 );
	}
	else
	{
		glDrawElements( vertexState_.getDrawMode(),
						vertexState_.getIndexDim()*
						vertexState_.getIndexCount(),
						vertexState_.getIndexType(),
						0 );
	}


	// UNBIND
	// ------
	// What: Set bindings to 0, avoids accidental changes from outside
	// When: Every frame
	unbindVertexData();
	unbindShaderProgram();
	unbindTextures();
	//unbindBufferTextures();
	//unbindTransformFeedback();
	unbindFramebuffer();


	// PACK
	// ----
	// What: "Packs" OpenGL VRAM/ON-DIE to OpenGL Buffer. Concerns pixel data
	// When: OpenGL governed data changed
	packFramebuffer();


	// DOWNLOAD
	// --------
	// What: Copy data from OpenGL Buffer ---> Allocator obhect (RAM)
	// When: Not on draw. On demand, typically when a user wants to save a file.


	// UNDECLARE
	// ----------
	// What: Release buffer storage, textures storage, free all ID's
	// When: Not on draw. When re-declaring or when destructing RenderState



}


//== SHARED: BufferState =======================================================

//-- constructors/destructor ----------------------------------------------

BufferState::BufferState()
{
	clear();
}

BufferState::BufferState( Allocator* const _allocatorPtr )
{
	clear();
	setAllocator( _allocatorPtr );
}

BufferState::~BufferState()
{
	undeclare();
	clear();
}


//-- copy and cleaar -------------------------------------------------------

void
BufferState::clear()
{
	// OpenGL variables
	bufferID_ = 0;
	initialTarget_ = GL_NONE;
	initialUsage_ = GL_NONE;

	// Allocator and trackers
	allocatorPtr_ = NULL;
	trackerAllocCount_.clear();
	trackerWriteCount_.clear();
	trackerPackCount_.clear();

	// Counters and flags
	declareCount_ = 0;
	uploadCount_ = 0;
	packCount_ = 0;
	isDeclared_ = 0;
}


//-- sets and gets -------------------------------------------------------------

void
BufferState::setAllocator( Allocator* const _allocatorPtr )
{
	// initial error handling
	if ( !_allocatorPtr )
	{
		GEM_ERROR( "Invalid Allocator pointer" );
	}


	// setup pointer and trackers
	allocatorPtr_ = _allocatorPtr;
	trackerAllocCount_.set( allocatorPtr_->getAllocCountPtr(), 0 );
	trackerWriteCount_.set( allocatorPtr_->getWriteCountPtr(), 0 );
	trackerPackCount_.set( getPackCountPtr(), 0 );
}



//-- RAM <--> Buffer functions -------------------------------------------------

void
BufferState::declare()
{
	// initial error handling
	{}


	// We check against the alloc counter in the Allocator to see if FORMAT
	// has changed
	if ( !trackerAllocCount_.greaterPtrCpy() )
	{
		return;
	}
	else
	{
		undeclare();
	}


	// One time messages
	if ( !( allocatorPtr_ && allocatorPtr_->isAlloc() ) )
	{
		GEM_ERROR( "Buffer Object Data is not allocated." );
	}
	if ( !initialTarget_ )
	{
		GEM_ERROR( "Buffer Object has no initial target defined" );
	}
	if ( !initialUsage_ )
	{
		GEM_ERROR( "Buffer Object has no initial usage defined" );
	}


	// The buffer is allocated only. Upload of data is done elsewhere
	glGenBuffers( 1, &bufferID_ );
	glBindBuffer( initialTarget_, bufferID_ );
	glBufferData( initialTarget_,
					allocatorPtr_->getByteCount(),
					NULL,
					initialUsage_ );
	glBindBuffer( initialTarget_, 0 );


	// Increase declared counter to inform others about our actions
	// this buffer is now declared, let everyone know
	declareCount_++;
	isDeclared_ = true;
}

void
BufferState::undeclare()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// Undeclare Buffer Object This deletes storage and frees
	// up ID for later use
	if ( glIsBuffer( bufferID_ ) )
	{
		glDeleteBuffers( 1, &bufferID_ );
	}


	// Set flags for buffer user to know that Buffer Okect has changed
	isDeclared_ = false;
}

void
BufferState::upload()
{
	// initial error handling
	if ( !isDeclared_ )
	{
		return;
	}


	// We check against the write counter in the Allocator to see if DATA
	// has changed
	if ( !trackerWriteCount_.greaterPtrCpy() )
	{
		return;
	}


	// One time messages
	if ( !( allocatorPtr_ && allocatorPtr_->isAlloc() ) )
	{
		GEM_ERROR( "Buffer Object Data is not allocated." );
	}
	if ( !initialTarget_ )
	{
		GEM_ERROR( "Buffer Object has no initial target defined." );
	}
	if ( !initialUsage_ )
	{
		GEM_ERROR( "Buffer Object has no initial usage defined." );
	}
	if ( !glIsBuffer( bufferID_ ) )
	{
		GEM_ERROR( "ID is not a valid Buffer Object." );
	}


	// The buffer is uploaded here
	glBindBuffer( initialTarget_, bufferID_ );
	glBufferData( initialTarget_,
					allocatorPtr_->getByteCount(),
					allocatorPtr_->getReadPtr<GLvoid>(),
					initialUsage_ );
	glBindBuffer( initialTarget_, 0 );


	// this buffer has been uploaded, let everyone know
	uploadCount_++;
}

void
BufferState::download()
{
	// initial error handling
	if ( !isDeclared_ )
	{
		return;
	}
	if ( !( allocatorPtr_ && allocatorPtr_->isAlloc() ) )
	{
		GEM_ERROR( "Buffer Object Data is not allocated." );
	}


	// If BUFFER has changed then download data to Allocator
	if ( trackerPackCount_.greaterPtrCpy() )
	{
		glBindBuffer( initialTarget_, bufferID_ );
		glGetBufferSubData( initialTarget_,
							0,
							allocatorPtr_->getByteCount(),
							allocatorPtr_->getWritePtr<GLvoid>() );
		glBindBuffer( initialTarget_, 0 );
	}
}

void
RenderState::declareBuffers()
{
	// setup iterators before loop
	std::map<const Allocator*,BufferState>::iterator i = bufferStates_.begin();
	std::map<const Allocator*,BufferState>::iterator iend = bufferStates_.end();

	// Declare each Buffer Object
	for ( ; i != iend ; ++i )
	{
		(*i).second.declare();
	}
}

void
RenderState::undeclareBuffers()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// setup iterators before loop
	std::map<const Allocator*,BufferState>::iterator i = bufferStates_.begin();
	std::map<const Allocator*,BufferState>::iterator iend = bufferStates_.end();


	// Undeclare Buffer Objects. Dont use this unless you really intend
	// to invalidate ALL Buffer Objects
	for ( ; i != iend ; ++i )
	{
		(*i).second.undeclare();
	}
}


void
RenderState::uploadBuffers()
{
	// setup iterators before loop
	std::map<const Allocator*,BufferState>::iterator i = bufferStates_.begin();
	std::map<const Allocator*,BufferState>::iterator iend = bufferStates_.end();


	// Undeclare Buffer Objects. Dont use this unless you really intend
	// to invalidate ALL Buffer Objects
	for ( ; i != iend ; ++i )
	{
		(*i).second.upload();
	}
}

void
RenderState::downloadBuffers()
{
	// setup iterators before loop
	std::map<const Allocator*,BufferState>::iterator i = bufferStates_.begin();
	std::map<const Allocator*,BufferState>::iterator iend = bufferStates_.end();


	// Download buffers if anything has happened to the buffer data
	for ( ; i != iend ; ++i )
	{
		(*i).second.download();
	}
}


//== INPUT: VertexState ========================================================

//-- constructors/destructor ---------------------------------------------------

VertexState::VertexState()
{
	clear();
}

VertexState::~VertexState()
{
	undeclare();
	clear();
}


//-- copy and clear --------------------------------------------------------

void
VertexState::clear()
{
	// OpenGL variables
	arrayObjectID_ = GL_NONE;
	drawMode_ = GL_NONE;
	indexDim_ = GL_NONE;
	indexCount_ = 0;
	indexType_ = GL_NONE;
	for ( unsigned int i = 0; i < MAX_VERTEX_ATTRIBUTES; ++i )
	{
		attributeDim_[i] = GL_NONE;
		attributeType_[i] = GL_NONE;
	}

	// BufferStates and trackers
	indexBufferPtr_ = NULL;
	trackerIndexDeclareCount_.clear();
	for ( unsigned int i = 0; i < MAX_VERTEX_ATTRIBUTES; ++i )
	{
		attributeBufferPtr_[i] = NULL;
		trackerAttributeDeclareCount_[i].clear();
	}

	// Counters and flags
	bool isDeclared_ = false;
}


//-- sets and gets -------------------------------------------------------------

void
VertexState::setIndexBuffer( BufferState* const _bufferStatePtr )
{
	// initial error handling
	if ( !( _bufferStatePtr ) )
	{
		GEM_ERROR( "Buffer Object is not valid." );
	}


	// setup pointer and trackers
	indexBufferPtr_ = _bufferStatePtr;
	trackerIndexDeclareCount_.set( _bufferStatePtr->getDeclareCountPtr(), 0 );
}

void
VertexState::setAttributeBuffer( BufferState* const _bufferStatePtr, 
								 unsigned int _attrID )
{
	// initial error handling
	if ( !( _bufferStatePtr ) )
	{
		GEM_ERROR( "Buffer Object is not valid." );
	}
	if ( _attrID >= MAX_VERTEX_ATTRIBUTES )
	{
		GEM_ERROR( "Vertex Attribute index out of range." );
	}

	// setup pointer and trackers
	attributeBufferPtr_[_attrID] = _bufferStatePtr;
	trackerAttributeDeclareCount_[_attrID].set( 
									_bufferStatePtr->getDeclareCountPtr(), 0 );
}

void
RenderState::setMesh( MeshLoader* const _meshLoaderPtr )
{
	// initial error handling
	if ( !( _meshLoaderPtr && _meshLoaderPtr->isLoaded() ) )
	{
		GEM_ERROR( "Mesh data is not loaded." );
	}


	// set primitives storage pointer
	if ( _meshLoaderPtr->getPrimitivesPtr() &&
		 _meshLoaderPtr->getPrimitivesPtr()->isAlloc() )
	{
		setVertexIndexData( _meshLoaderPtr->getPrimitivesPtr() );
	}


	// for each vertex attribute slot we loop through the storage to see if
	// it has data
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( _meshLoaderPtr->getVertexAttributesPtr( i ) &&
			 _meshLoaderPtr->getVertexAttributesPtr( i )->isAlloc() )
		{ 
			setVertexAttributeData( 
				_meshLoaderPtr->getVertexAttributesPtr( i ), i );
		}
	}
}

void
RenderState::setVertexIndexData( Allocator* const _allocatorPtr )
{
	// initial error handling
	if ( !( _allocatorPtr && _allocatorPtr->isAlloc() ) )
	{
		GEM_ERROR( "Vertex Index Data is not allocated." );
	}


	// Get the related BufferState to this Allocator, create new if needed
	BufferState* bufferStatePtr;
	if ( bufferStates_.count( _allocatorPtr ) == 0 )
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
		bufferStatePtr->setAllocator( _allocatorPtr );
		bufferStatePtr->setInitialTarget( GL_ELEMENT_ARRAY_BUFFER );
		bufferStatePtr->setInitialUsage( GL_STATIC_DRAW );
	}
	else
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
	}


	// set this as the Index Buffer Object
	vertexState_.setIndexBuffer( bufferStatePtr );
}

void
RenderState::setVertexAttributeData( Allocator* const _allocatorPtr, 
										unsigned int _attrID )
{
	// initial error handling
	if ( !( _allocatorPtr && _allocatorPtr->isAlloc() ) )
	{
		GEM_ERROR( "Vertex Attribute Data is not allocated." );
	}
	if ( _attrID >= MAX_VERTEX_ATTRIBUTES )
	{
		GEM_ERROR( "Vertex Attribute index out of range." );
	}


	// Get the related BufferState to this Allocator, create new if needed
	BufferState* bufferStatePtr;
	if ( bufferStates_.count( _allocatorPtr ) == 0 )
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
		bufferStatePtr->setAllocator( _allocatorPtr );
		bufferStatePtr->setInitialTarget( GL_ARRAY_BUFFER );
		bufferStatePtr->setInitialUsage( GL_STATIC_DRAW );
		
	}
	else
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
	}


	// set this as the Attribute Buffer Object
	vertexState_.setAttributeBuffer( bufferStatePtr, _attrID );
}


//-- Buffer <--> OpenGL functions ------------------------------------------

void
VertexState::declare()
{
	// TODO: initial error handling
	{}


	// Contrary to some other OpenGL objects (textures, buffers im looking at
	// you) any change in format invalidates the entire vertex array object and
	// an undeclare and re-declare is needed.
	bool requireDeclare = false;
	requireDeclare |= trackerIndexDeclareCount_.greaterPtrCpy();
	for ( unsigned int i = 0; i < MAX_VERTEX_ATTRIBUTES; ++i )
	{
		requireDeclare |= trackerAttributeDeclareCount_[i].greaterPtrCpy();
	}
	if ( requireDeclare )
	{
		undeclare();
	}
	else
	{
		return;
	}


	// One time messages
	if ( !(indexBufferPtr_ && indexBufferPtr_->getAllocatorPtr()) )
	{
		GEM_ERROR( "Index buffer data is not complete." );
	}


	// Setup the OpenGL constants for primitives and vertex attributes.
	//
	// This is the RAM data format AND the internal format. No special
	// representation of the internal data format for vertex data.
	//

	switch ( indexBufferPtr_->getAllocatorPtr()->getFormat() )
	{
	case ALLOC_FORMAT_SCALAR_32UI:
		drawMode_ = GL_POINT;
		indexDim_ = 1;
		indexType_ = GL_UNSIGNED_INT;
		break;
	case ALLOC_FORMAT_VEC2_32UI:
		drawMode_ = GL_LINE;
		indexDim_ = 2;
		indexType_ = GL_UNSIGNED_INT;
		break;
	case ALLOC_FORMAT_VEC3_32UI:
		drawMode_ = GL_TRIANGLES;
		indexDim_ = 3;
		indexType_ = GL_UNSIGNED_INT;
		break;
	case ALLOC_FORMAT_VEC4_32UI:
		drawMode_ = GL_QUADS;
		indexDim_ = 4;
		indexType_ = GL_UNSIGNED_INT;
		break;
	default:
		drawMode_ = GL_NONE;
		indexDim_ = 0;
		indexType_ = GL_NONE;
		GEM_ERROR( "Unsupported primitive format." );
	}
	indexCount_ = indexBufferPtr_->getAllocatorPtr()->getElementCount();


	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( attributeBufferPtr_[i] && 
			 attributeBufferPtr_[i]->getAllocatorPtr() )
		{
			switch ( attributeBufferPtr_[i]->getAllocatorPtr()->getFormat() )
			{
			case ALLOC_FORMAT_SCALAR_32UI:
				attributeDim_[i] = 1;
				attributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_SCALAR_32F:
				attributeDim_[i] = 1;
				attributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC2_32UI:
				attributeDim_[i] = 2;
				attributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC2_32F:
				attributeDim_[i] = 2;
				attributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC3_32UI:
				attributeDim_[i] = 3;
				attributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC3_32F:
				attributeDim_[i] = 3;
				attributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC4_32UI:
				attributeDim_[i] = 4;
				attributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC4_32F:
				attributeDim_[i] = 4;
				attributeType_[i] = GL_FLOAT;
				break;
			default:
				attributeDim_[i] = 0;
				attributeType_[i] = GL_NONE;
				GEM_ERROR( "Unsupported vertex attribute format." );
			}
		}
	}


    // Create the vertex array object and bind index and attributes
    //
    // The buffer objects that are to be used by the vertex stage of the GL
    // are collected together to form a vertex array object. All state related
    // to the definition of data used by the vertex processor is encapsulated
    // in a vertex array object.
    //
    // Vertex array objects maintain state. Any required vertex and index
    // buffer objects are created and bound to the vertex array object. Then
    // any vertex shader generic shader input variables are mapped to the
    // vertex data stored in the vertex buffer objects bound to the vertex
    // array object. All of this only needs to be done once when using vertex
    // array objects.
	//
	glGenVertexArrays( 1, &arrayObjectID_ );
	glBindVertexArray( arrayObjectID_ );
	if ( indexType_ != GL_NONE )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,
					  indexBufferPtr_->getBufferID() );
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( attributeType_[i] != GL_NONE )
		{
			glBindBuffer( GL_ARRAY_BUFFER,
						  attributeBufferPtr_[i]->getBufferID() );
		}
	}


	// Here we bind the buffer to the generic vertex attribute locations.
	// Use the layout specifier to access the attributes in the shader
	//
	//		layout( location = 0 ) vec3 Position;
	//
	// We could also use glGetAttribLocation( shaderProgramID_, "Position" );
	// to get the location of a named vertex attribute and then use this
	// location with glVertexAttribPointer(). I prefer this though as it doesnt
	// require the shader program to be bound yet.
	//
	// The first argument of glVertexAttribPointer() is the layout specifier
	// number and the last argument is the data pointer which is 0 since we 
	// binding it to the buffer object. The vertex array is then enabled
	// with glEnableVertexAttribArray()
	//
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( attributeType_[i] != GL_NONE )
		{
			glBindBuffer( GL_ARRAY_BUFFER, 
						  attributeBufferPtr_[i]->getBufferID() );
			glVertexAttribPointer( i, attributeDim_[i],
								   attributeType_[i], GL_FALSE, 0, 0);
			glEnableVertexAttribArray( i );
		}
	}


	// we might not want this vertex array object bound in the next glCall
	glBindVertexArray( 0 );


	// vertex data is now declared
	isDeclared_ = true;
}

void
VertexState::undeclare()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// Delete vertex array object contex and free up index for re-use.
	if ( glIsVertexArray( arrayObjectID_ ) )
	{
		glDeleteVertexArrays( 1, &arrayObjectID_ );
	}


	// mark is no longer declared
	isDeclared_  = false;
}

void
VertexState::bind()
{
	// initial error handling
	if ( !isDeclared_ )
	{
		return;
	}


	// Because we use vertex array objects all we need to do here is bind
	if ( glIsVertexArray( arrayObjectID_ ) )
	{
		glBindVertexArray( arrayObjectID_ );
	}
}

void
VertexState::unbind()
{
	// initial error handling
	if ( !isDeclared_ )
	{
		return;
	}


	// Because we use vertex array objects all we need to do here is bind
	glBindVertexArray( 0 );
}

void
RenderState::declareVertexData()
{
	vertexState_.declare();
}

void
RenderState::undeclareVertexData()
{
	vertexState_.undeclare();
}

void
RenderState::bindVertexData()
{
	vertexState_.bind();
}

void
RenderState::unbindVertexData()
{
	vertexState_.unbind();
}


//== INPUT: ProgramState =======================================================

//-- constructors/destructor ---------------------------------------------------

ProgramState::ProgramState()
{
	clear();
}

ProgramState::~ProgramState()
{
	undeclare();
	clear();
}


//-- copy and clear ------------------------------------------------------------
	
void
ProgramState::clear()
{
	// OpenGL variables
	vertexShaderID_ = GL_NONE;
	tessCtrlShaderID_ = GL_NONE;
	tessEvalShaderID_ = GL_NONE;
	geometryShaderID_ = GL_NONE;
	fragmentShaderID_ = GL_NONE;
	programID_ = GL_NONE;

	// Allocators and trackers
	vertexShaderDataPtr_ = NULL;
	tessCtrlShaderDataPtr_ = NULL;
	tessEvalShaderDataPtr_ = NULL;
	geometryShaderDataPtr_ = NULL;
	fragmentShaderDataPtr_ = NULL;
	trackerVertexShaderAllocCount_.clear();
	trackerTessCtrlShaderAllocCount_.clear();
	trackerTessEvalShaderAllocCount_.clear();
	trackerGeometryShaderAllocCount_.clear();
	trackerFragmentShaderAllocCount_.clear();

	// Counters and flags
	hasTesselator_ = false;
	declareCount_ = 0;
	isDeclared_ = false;
}


//-- sets and gets -------------------------------------------------------------

void
ProgramState::setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr )
{
	if ( _vertexShaderLoaderPtr &&
		 _vertexShaderLoaderPtr->getSourcePtr() &&
		 _vertexShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclare();

		// save data pointer
		// setup tracker
		vertexShaderDataPtr_ = _vertexShaderLoaderPtr->getSourcePtr();
		trackerVertexShaderAllocCount_.set(
							vertexShaderDataPtr_->getAllocCountPtr(), 0 );
	}
}

void
ProgramState::setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr )
{
	if ( _tessCtrlShaderLoaderPtr &&
		 _tessCtrlShaderLoaderPtr->getSourcePtr() &&
		 _tessCtrlShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclare();

		// save data pointer
		// setup tracker
		tessCtrlShaderDataPtr_ = _tessCtrlShaderLoaderPtr->getSourcePtr();
		trackerTessCtrlShaderAllocCount_.set(
							tessCtrlShaderDataPtr_->getAllocCountPtr(), 0 );
	}
}

void
ProgramState::setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr )
{
	if ( _tessEvalShaderLoaderPtr &&
		 _tessEvalShaderLoaderPtr->getSourcePtr() &&
		 _tessEvalShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclare();

		// save data pointer
		// setup tracker
		tessEvalShaderDataPtr_ = _tessEvalShaderLoaderPtr->getSourcePtr();
		trackerTessEvalShaderAllocCount_.set(
							tessEvalShaderDataPtr_->getAllocCountPtr(), 0 );
	}
}

void
ProgramState::setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr )
{
	if ( _geometryShaderLoaderPtr &&
		 _geometryShaderLoaderPtr->getSourcePtr() &&
		 _geometryShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclare();

		// save data pointer
		// setup tracker
		geometryShaderDataPtr_ = _geometryShaderLoaderPtr->getSourcePtr();
		trackerGeometryShaderAllocCount_.set(
							geometryShaderDataPtr_->getAllocCountPtr(), 0 );
	}
}


void
ProgramState::setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr )
{
	if ( _fragmentShaderLoaderPtr &&
		 _fragmentShaderLoaderPtr->getSourcePtr() &&
		 _fragmentShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclare();

		// save data pointer
		// setup tracker
		fragmentShaderDataPtr_ = _fragmentShaderLoaderPtr->getSourcePtr();
		trackerFragmentShaderAllocCount_.set(
							fragmentShaderDataPtr_->getAllocCountPtr(), 0 );
	}
}

void
RenderState::setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr )
{
	programState_.setVertexShader( _vertexShaderLoaderPtr );
}

void
RenderState::setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr )
{
	programState_.setTessCtrlShader( _tessCtrlShaderLoaderPtr );
}

void
RenderState::setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr )
{
	programState_.setTessEvalShader( _tessEvalShaderLoaderPtr );
}

void
RenderState::setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr )
{
	programState_.setGeometryShader( _geometryShaderLoaderPtr );
}


void
RenderState::setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr )
{
	programState_.setFragmentShader( _fragmentShaderLoaderPtr );
}


//-- Buffer <--> OpenGL functions ---------------------------------------------

void
ProgramState::declare()
{
	// initial error handling
	{}


	// Change in ANY of the shader sources will invalidate the entire program
	// and consequently require a re-declare
	bool requireDeclare = false;
	requireDeclare |= trackerVertexShaderAllocCount_.greaterPtrCpy();
	requireDeclare |= trackerTessCtrlShaderAllocCount_.greaterPtrCpy();
	requireDeclare |= trackerTessEvalShaderAllocCount_.greaterPtrCpy();
	requireDeclare |= trackerGeometryShaderAllocCount_.greaterPtrCpy();
	requireDeclare |= trackerFragmentShaderAllocCount_.greaterPtrCpy();
	if ( requireDeclare )
	{
		undeclare();
	}
	else
	{
		return;
	}


	// Compile each shader and then link them into a shader program
	//
	// The process for creating a single program object is
	//
	//		glCreateShader()
	//		glShaderSource()
	//		glCompileShader()
	//		glGetShaderiv( _, GL_COMPILE_STATUS, _ )
	//		...
	//		glCreateProgram()
	//		glAttachShader()
	//		...
	//		glLinkProgram()
	//		glGetProgramiv( _, GL_LINK_STATUS, _ )
	//
	// An alternate way is to create each program individually and bind them 
	// to a pipeline object
	//
	//		glCreateShaderProgramv()
	//		...
	//		glGenProgramPipelines()
	//		glBindProgramPipeline()
	//		...
	//
	// I prefer the first method because you bind uniforms to a program object
	// object and not a pipeline which makes the uniform available for all 
	// shaders for the first case but not the second.


	// load and compile VERTEX shader
	GLint vertexShaderCompileStatus = 0;
	std::string vertexShaderInfoLog;
	const GLchar* ptr;
	if ( vertexShaderDataPtr_ )
	{
		vertexShaderID_ = glCreateShader( GL_VERTEX_SHADER );
		ptr = vertexShaderDataPtr_->getReadPtr<char>();
		glShaderSource( vertexShaderID_, 1, &ptr, NULL );
		glCompileShader( vertexShaderID_ );
	}


	// load and compile TESSELATION CONTROL shader
	GLint tessCtrlShaderCompileStatus = 0;
	std::string tessCtrlShaderInfoLog;
	if ( tessCtrlShaderDataPtr_ )
	{
		tessCtrlShaderID_ = glCreateShader( GL_TESS_CONTROL_SHADER );
		ptr = tessCtrlShaderDataPtr_->getReadPtr<char>();
		glShaderSource( tessCtrlShaderID_, 1, &ptr, NULL );
		glCompileShader( tessCtrlShaderID_ );
	}


	// load and compile TESSELATION EVAULATION shader
	GLint tessEvalShaderCompileStatus = 0;
	std::string tessEvalShaderInfoLog;
	if ( tessEvalShaderDataPtr_ )
	{
		tessEvalShaderID_ = glCreateShader( GL_TESS_EVALUATION_SHADER );
		ptr = tessEvalShaderDataPtr_->getReadPtr<char>();
		glShaderSource( tessEvalShaderID_, 1, &ptr, NULL );
		glCompileShader( tessEvalShaderID_ );
	}
	

	// load and compile GEOMETRY shader
	GLint geometryShaderCompileStatus = 0;
	std::string geometryShaderInfoLog;
	if ( geometryShaderDataPtr_ )
	{
		geometryShaderID_ = glCreateShader( GL_GEOMETRY_SHADER );
		ptr = geometryShaderDataPtr_->getReadPtr<char>();
		glShaderSource( geometryShaderID_, 1, &ptr, NULL );
		glCompileShader( geometryShaderID_ );
	}


	// load and compile FRAGMENT shader
	GLint fragmentShaderCompileStatus = 0;
	std::string fragmentShaderInfoLog;
	if ( fragmentShaderDataPtr_ )
	{
		fragmentShaderID_ = glCreateShader( GL_FRAGMENT_SHADER );
		ptr = fragmentShaderDataPtr_->getReadPtr<char>();
		glShaderSource( fragmentShaderID_, 1, &ptr, NULL );
		glCompileShader( fragmentShaderID_ );
	}


	// create shader program
	programID_ = glCreateProgram();
	if ( vertexShaderDataPtr_ )
		glAttachShader( programID_, vertexShaderID_ );
	if ( tessCtrlShaderDataPtr_ )
		glAttachShader( programID_, tessCtrlShaderID_ );
	if ( tessEvalShaderDataPtr_ )			
		glAttachShader( programID_, tessEvalShaderID_ );
	if ( geometryShaderDataPtr_ )
		glAttachShader( programID_, geometryShaderID_ );
	if ( fragmentShaderDataPtr_ )
		glAttachShader( programID_, fragmentShaderID_ );
	glLinkProgram( programID_ );


	// One stop error handling. If the shader compiles fail this will show up
	// in the the program link infor log, thats why we dont bother handling
	// these individually
	GLint linkStatus = 0;
	GLsizei infoLogSize = 0;
	std::string infoLog;
	glGetProgramiv( programID_, GL_LINK_STATUS, &linkStatus);
	if ( !linkStatus )
	{
		glGetProgramiv( programID_, GL_INFO_LOG_LENGTH, &infoLogSize );
		infoLog.resize( infoLogSize );
		glGetProgramInfoLog( programID_, infoLogSize,
							 &infoLogSize, &infoLog[0] );
		infoLog.resize( infoLog.length()-2 );
		GEM_ERROR( "Shader Program Link Failed\n" 
				   "==========================\n\n" +
				   infoLog );
	}


	// save this flag for simplicity
	if ( tessCtrlShaderDataPtr_ || tessEvalShaderDataPtr_ )
	{
		hasTesselator_ = true;
	}


	// shader data is now declared, let everyone know
	declareCount_++;
	isDeclared_ = true;
}

void
ProgramState::undeclare()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// Delete shaders content and free up index for re-use
	if ( glIsShader( vertexShaderID_ ) )
	{
		glDeleteShader( vertexShaderID_ );
	}
	if ( glIsShader( fragmentShaderID_ ) )
	{
		glDeleteShader( fragmentShaderID_ );
	}
	if ( glIsShader( geometryShaderID_ ) )
	{
		glDeleteShader( geometryShaderID_ );
	}
	if ( glIsShader( tessCtrlShaderID_ ) )
	{
		glDeleteShader( tessCtrlShaderID_ );
	}
	if ( glIsShader( tessEvalShaderID_ ) )
	{
		glDeleteShader( tessEvalShaderID_ );
	}


	// Delete program content  and free up index for re-use
	if ( glIsProgram( programID_ ) )
	{
		glDeleteProgram( programID_ );
	}


	// hader data is no longer declared
	isDeclared_ = false;
}

void
ProgramState::bind()
{
	// shader program should already be declared
	if ( !isDeclared_ )
		return;

	// bind program
	glUseProgram( programID_ );
}

void
ProgramState::unbind()
{
	// shader program should already be declared
	if ( !isDeclared_ )
		return;

	// unbind program, leave it to default state
	glUseProgram( 0 );
}

void
RenderState::declareShaderProgram()
{
	programState_.declare();
}

void
RenderState::undeclareShaderProgram()
{
	programState_.undeclare();
}


void
RenderState::bindShaderProgram()
{
	programState_.bind();
}


void
RenderState::unbindShaderProgram()
{
	programState_.unbind();
}


//== INPUT: UniformState =======================================================

//-- constructors/destructor ---------------------------------------------------

UniformState::UniformState()
{
	clear();
}

UniformState::UniformState( const ProgramState* const _programStatePtr,
							const void* _ptr,
							const std::string& _name,
							const ALLOC_FORMAT& _allocFormat,
							const unsigned int _count )
{
	clear();
	if ( _programStatePtr && _ptr )
	{
		programStatePtr_ = _programStatePtr;
		ptr_ = _ptr;
		name_ = _name;
		allocFormat_ = _allocFormat;
		count_ = _count;
		trackerProgramDeclareCount_.set( 
			_programStatePtr->getDeclareCountPtr(), 0 );
	}
}

UniformState::~UniformState()
{
	undeclare();
	clear();
}


//-- copy and clear ------------------------------------------------------------

void
UniformState::clear()
{
	// OpenGL variables
	location_ = -1;

	// program to request for uniform variables
	programStatePtr_ = NULL;

	// Allocators type variables and trackers
	ptr_ = NULL;
	name_ = "";
	allocFormat_ = ALLOC_FORMAT_NONE;
	count_ = 0;
	trackerProgramDeclareCount_.clear();

	// Counters and flags
	bool isDeclared_ = false;
}


//-- sets and gets ---------------------------------------------------------

void
UniformState::setProgram( const ProgramState* const _programStatePtr )
{
	if ( _programStatePtr )
	{
		trackerProgramDeclareCount_.set( 
			_programStatePtr->getDeclareCountPtr(), 0 );
	}
}

void
UniformState::setUniform( const void* const _ptr,
							const std::string& _name,
							const ALLOC_FORMAT& _allocFormat,
							const unsigned int _count )
{
	if ( _ptr )
	{
		ptr_ = _ptr;
		name_ = _name;
		allocFormat_ = _allocFormat;
		count_ = _count;
	}
}

void
RenderState::setUniform( const Allocator* const _allocatorPtr,
						 const std::string& _name )
{
	if ( _allocatorPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _allocatorPtr ),
						  _name,
						  _allocatorPtr->getFormat(),
						  _allocatorPtr->getElementCount() ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const int* const _intPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _intPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _intPtr ),
						  _name,
						  ALLOC_FORMAT_SCALAR_32I,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const unsigned int* const _uintPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _uintPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _uintPtr ),
						  _name,
						  ALLOC_FORMAT_SCALAR_32UI,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const float* const _floatPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _floatPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _floatPtr ),
						  _name,
						  ALLOC_FORMAT_SCALAR_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec2i* const _vec2iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2iPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec2iPtr ),
						  _name,
						  ALLOC_FORMAT_VEC2_32I,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec2ui* const _vec2uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2uiPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec2uiPtr ),
						  _name,
						  ALLOC_FORMAT_VEC2_32UI,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec2f* const _vec2fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec2fPtr ),
						  _name,
						  ALLOC_FORMAT_VEC2_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec3i* const _vec3iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3iPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec3iPtr ),
						  _name,
						  ALLOC_FORMAT_VEC3_32I,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec3ui* const _vec3uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3uiPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec3uiPtr ),
						  _name,
						  ALLOC_FORMAT_VEC3_32UI,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec3f* const _vec3fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec3fPtr ),
						  _name,
						  ALLOC_FORMAT_VEC3_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec4i* const _vec4iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4iPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec4iPtr ),
						  _name,
						  ALLOC_FORMAT_VEC4_32I,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec4ui* const _vec4uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4uiPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec4uiPtr ),
						  _name,
						  ALLOC_FORMAT_VEC4_32UI,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Vec4f* const _vec4fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _vec4fPtr ),
						  _name,
						  ALLOC_FORMAT_VEC4_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Mat2f* const _mat2fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat2fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _mat2fPtr ),
						  _name,
						  ALLOC_FORMAT_MAT2_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Mat3f* const _mat3fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat3fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _mat3fPtr ),
						  _name,
						  ALLOC_FORMAT_MAT3_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderState::setUniform( const Mat4f* const _mat4fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat4fPtr )
	{
		uniformStates_.push_back( 
			UniformState( &programState_,
						  static_cast<const void*>( _mat4fPtr ),
						  _name,
						  ALLOC_FORMAT_MAT4_32F,
						  _count ) );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}


//-- Buffer <--> OpenGL functions ------------------------------------------

void
UniformState::declare()
{
	// TODO initial error handling
	{}


	// re-declare is needed if the program has changed or possibly if the 
	// uniform name has changed (we dont track changes to name at current)
	if ( trackerProgramDeclareCount_.greaterPtrCpy() )
	{
		undeclare();
	}
	else
	{
		return;
	}


	// Look for a uniform named name_ in Shader Program
	location_ = glGetUniformLocation( programStatePtr_->getProgramID(),
										 name_.c_str() );
	if ( location_ == -1 )
	{
		GEM_ERROR( "Uniform not found in shader program: " + name_ );
	}


	// if we got this far the location has been found and we are ok!
	isDeclared_ = true;
}

void
UniformState::undeclare()
{
	// uniform should be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// -1 location means "not found" in OpenGL speak. 0 is a valid location
	location_ = -1;


	// mark as no longer declared
	isDeclared_ = false;
}

void
UniformState::bind()
{
	// uniform should be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// call correct OpenGL function
	switch ( allocFormat_ )
	{
		
	// scalars
	case ALLOC_FORMAT_SCALAR_32I:
		glUniform1iv( location_, count_, 
						reinterpret_cast<const GLint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_SCALAR_32UI:
		glUniform1uiv( location_, count_, 
						reinterpret_cast<const GLuint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_SCALAR_32F:
		glUniform1fv( location_, count_, 
						reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;

	// vectors
	case ALLOC_FORMAT_VEC2_32I:
		glUniform2iv( location_, count_, 
						reinterpret_cast<const GLint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC2_32UI:
		glUniform2uiv( location_, count_, 
						reinterpret_cast<const GLuint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC2_32F:
		glUniform2fv( location_, count_, 
						reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC3_32I:
		glUniform3iv( location_, count_, 
						reinterpret_cast<const GLint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC3_32UI:
		glUniform3uiv( location_, count_, 
						reinterpret_cast<const GLuint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC3_32F:
		glUniform3fv( location_, count_, 
						reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC4_32I:
		glUniform4iv( location_, count_, 
						reinterpret_cast<const GLint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC4_32UI:
		glUniform4uiv( location_, count_, 
						reinterpret_cast<const GLuint*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_VEC4_32F:
		glUniform4fv( location_, count_, 
						reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;

	// matrices
	case ALLOC_FORMAT_MAT2_32F:
		glUniformMatrix4fv( location_, count_, GL_TRUE,
							reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_MAT3_32F:
		glUniformMatrix4fv( location_, count_, GL_TRUE,
							reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;
	case ALLOC_FORMAT_MAT4_32F:
		glUniformMatrix4fv( location_, count_, GL_TRUE,
							reinterpret_cast<const GLfloat*>( ptr_ ) );
		break;

	default:
		break;

	}
}

void
RenderState::declareUniforms()
{
	// setup iterators before loop
	std::vector<UniformState>::iterator i = uniformStates_.begin();
	std::vector<UniformState>::iterator iend = uniformStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).declare();
	}
}

void
RenderState::undeclareUniforms()
{
	// setup iterators before loop
	std::vector<UniformState>::iterator i = uniformStates_.begin();
	std::vector<UniformState>::iterator iend = uniformStates_.end();


	// Undeclare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).undeclare();
	}
}

void
RenderState::bindUniforms()
{
	// setup iterators before loop
	std::vector<UniformState>::iterator i = uniformStates_.begin();
	std::vector<UniformState>::iterator iend = uniformStates_.end();


	// Bind each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).bind();
	}
}


//== INPUT: Texture State ======================================================


//-- constructors/destructor -----------------------------------------------

TextureState::TextureState()
{
	clear();
}

TextureState::~TextureState()
{
	undeclare();
	clear();
}


//-- copy and clear --------------------------------------------------------
	
void
TextureState::clear()
{
	// OpenGL variables
	textureID_ = GL_NONE;
	target_ = GL_NONE;
	internalFormat_ = GL_NONE;
	format_ = GL_NONE;
	type_ = GL_NONE;
	textureUnit_ = 0;
	samplerID_ = 0;

	// Allocators and trackers
	for ( unsigned int i = 0; i < MAX_MIP_LEVELS; ++i )
	{
		bufferStatePtr_[i] = NULL;
		trackerBufferDeclareCount_[i].clear();
		trackerBufferUploadCount_[i].clear();
		trackerBufferPackCount_[i].clear();
	}
	mipLevelCount_ = 0;

	// Counters and flags
	isDeclared_ = false;
}


//-- sets and gets ---------------------------------------------------------

void
TextureState::setMipMap( BufferState* const _bufferStatePtr,
						 const unsigned int _mipLevel )
{
	// initial error handling
	if ( !( _bufferStatePtr ) )
	{
		GEM_ERROR( "Buffer Object is not valid." );
	}
	if ( _mipLevel >= MAX_MIP_LEVELS )
	{
		GEM_ERROR( "Mip level specifier out of range" );
	}


	// mip level count for this texture
	mipLevelCount_ = Mem::max<unsigned int>( mipLevelCount_, _mipLevel + 1);


	// setup pointer and trackers
	bufferStatePtr_[_mipLevel] = _bufferStatePtr;
	trackerBufferDeclareCount_[_mipLevel].set(
		_bufferStatePtr->getDeclareCountPtr(), 0 );
	trackerBufferUploadCount_[_mipLevel].set(
		_bufferStatePtr->getUploadCountPtr(), 0 );
	trackerBufferPackCount_[_mipLevel].set(
		_bufferStatePtr->getPackCountPtr(), 0 );
}

void
RenderState::setTexture( TextureLoader* const _textureLoaderPtr,
						 unsigned int _textureUnit )
{
	// initial error handling
	if ( !( _textureLoaderPtr && _textureLoaderPtr->isLoaded() ) )
	{
		GEM_ERROR( "Texture data is not loaded." );
	}
	if ( MAX_TEXTURE_UNITS <= _textureUnit )
	{
		GEM_ERROR( "Texture unit specifier out of range." );
	}


	// Loop over texture mip levels until we find one that is not allocated
	for ( unsigned int i = 0; i < _textureLoaderPtr->getMipLevelCount(); ++i )
	{
		setTexture( _textureLoaderPtr->getMipLevePtr( i ), _textureUnit, i );
	}
}

void
RenderState::setTexture( Allocator* const _allocatorPtr,
						 unsigned int _textureUnit, unsigned int _mipLevel )
{
	if ( !( _allocatorPtr && _allocatorPtr->isAlloc() ) )
	{
		GEM_ERROR( "Texture data is not allocated." );
	}
	if ( MAX_TEXTURE_UNITS <= _textureUnit )
	{
		GEM_ERROR( "Texture unit specifier out of range." );
	}
	if ( MAX_MIP_LEVELS <= _mipLevel )
	{
		GEM_ERROR( "Mip level specifier out of range." );
	}


	// Get the related BufferState to this Allocator, create new if needed
	BufferState* bufferStatePtr;
	if ( bufferStates_.count( _allocatorPtr ) == 0 )
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
		bufferStatePtr->setAllocator( _allocatorPtr );
		bufferStatePtr->setInitialTarget( GL_PIXEL_UNPACK_BUFFER );
		bufferStatePtr->setInitialUsage( GL_STATIC_DRAW );
		
	}
	else
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
	}


	// Get the related TextureState to this Texture Unit, create new if needed
	// Set BufferState to the correct mip level.
	TextureState* textureStatePtr;
	if ( textureStates_.count( _textureUnit ) == 0 )
	{
		textureStatePtr = &(textureStates_[_textureUnit]);
		textureStatePtr->setTextureUnit( _textureUnit );
	}
	else
	{
		textureStatePtr = &(textureStates_[_textureUnit]);
	}
	textureStatePtr->setMipMap( bufferStatePtr, _mipLevel );
}

//-- Buffer <--> OpenGL functions ------------------------------------------

void
TextureState::declare()
{
	// initial error handling
	{}


	// release resources if any of the mip level allocators changed
	bool requireDeclare = false;
	for ( unsigned int i = 0; i < MAX_MIP_LEVELS; ++i )
	{
		requireDeclare |= trackerBufferDeclareCount_[i].greaterPtrCpy();
	}
	if ( requireDeclare )
	{
		undeclare();
	}
	else
	{
		return;
	}


	// One time messages
	if ( !(bufferStatePtr_[0] && bufferStatePtr_[0]->getAllocatorPtr()) )
	{
		GEM_ERROR( "First mip level is not complete." );
	}


	// Setup the OpenGL constants for the allocated textures. 
	//
	// foramt and type
	// ---------------
	// The format and type constants tells OGL the dimensionality and type of
	// the allocated data to be uploaded.
	//
	// internalFormat
	// ---------------
	// It is possible to have a different internal representation of this data
	// specified using the internalFormat constant. This is useful if you wish
	// to save space or don't need the full precision of the current format.
	// OGL will _try_ to give you the specified format and will do the
	// conversion for you. Some examples constants.
	//
	// GL_RGB8, GL_RGBA_5_5_5_5, GL_DEPTH
	//
	// target
	// -------
	// It is also possible to specify a specific target. This will affect
	// such as filtering indexing mode etc. Here are some possibilities
	//
	// GL_TEXTURE_2D: normalized indexing, normal filtering
	// GL_TEXTURE_RECTANGLE: not normalized indexing, normal filtering,
	//						 no mip levels allowed
	// GL_TEXTURE_ARRAY: "3D" textures but ssed to pack many textures into one.
	//					 No filtering between layers.
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	//
	// filtering and clamping
	// ----------------------
	// filtering: GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, etc
	// clamping: GL_CLAMP_TO_EDGE, etc
	//
	switch ( bufferStatePtr_[0]->getAllocatorPtr()->getFormat() )
	{
	case ALLOC_FORMAT_SCALAR_8UI:
		target_ = GL_TEXTURE_2D;
		internalFormat_ = GL_R8;
		format_ = GL_RED;
		type_ = GL_UNSIGNED_BYTE;
		break;
	case ALLOC_FORMAT_VEC3_8UI:
		target_ = GL_TEXTURE_2D;
		internalFormat_ = GL_RGB8;
		format_ = GL_RGB;
		type_ = GL_UNSIGNED_BYTE;
		break;
	case ALLOC_FORMAT_SCALAR_32F:
		target_ = GL_TEXTURE_2D;
		internalFormat_ = GL_R32F;
		format_ = GL_RED;
		type_ = GL_FLOAT;
		break;
	case ALLOC_FORMAT_VEC3_32F:
		target_ = GL_TEXTURE_2D;
		internalFormat_ = GL_RGB32F;
		format_ = GL_RGB;
		type_ = GL_FLOAT;
		break;
	case ALLOC_FORMAT_VEC3_DXT1:
		target_ = GL_TEXTURE_2D;
		internalFormat_ = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		format_ = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		type_ = GL_UNSIGNED_BYTE;
		break;
	default:
		target_ = GL_NONE;
		internalFormat_ = GL_NONE;
		format_ = GL_NONE;
		type_ = GL_NONE;
		GEM_ERROR( "Unsupported texture format." );
	}


	// Create the texture names and specify format.
	//
	// http://www.openorg/sdk/docs/man4/xhtml/glGenTextures.xml
	// http://www.openorg/sdk/docs/man4/xhtml/glBindTexture.xml
	// http://www.openorg/sdk/docs/man4/xhtml/glTexImage2D.xml
	// http://www.openorg/sdk/docs/man4/xhtml/glActiveTexture.xml
	//
	// Texture names are generated per texture and then assigegned to a 
	// texture unit. The format both allocated and internal is specified here
	// but the data is uploaded elsewhere.
	//
	// OpenGL treats an incomplete mip-chain as NO TEXTURE
	//
	//		"If texturing is enabled (and TEXTURE_MIN_FILTER is one that
	//		 requires a mipmap) at the time a primitive is rasterized and if
	//		 the set of arrays 0 through n is incomplete, based on the
	//		 dimensions of array 0, then it is as if texture mapping were
	//		 disabled."
	// 
	// For compressed textures this is bad news because the width of the lower
	// mip map levels are a minimum of 4x4, but a complete texture should
	// end at 1x1. To make sure the mip maps are always correct we use the 
	// following correct formula rather than using the size of the 
	// allocated data:
	//
	//		w_i = floor( w_0 / 2^i )
	//
	glGenTextures( 1, &textureID_ );
	glBindTexture( target_, textureID_ );


	for ( unsigned int i = 0; i<mipLevelCount_; ++i )
	{
		if ( bufferStatePtr_[i] && bufferStatePtr_[i]->getAllocatorPtr() )
		{

			double w = bufferStatePtr_[0]->getAllocatorPtr()->getWidth();
			double h = bufferStatePtr_[0]->getAllocatorPtr()->getHeight();
			w = std::max( std::floor( w / (1 << i) ), 1.0 );
			h = std::max( std::floor( h / (1 << i) ), 1.0 );


			ALLOC_TYPE allocType = 
				bufferStatePtr_[i]->getAllocatorPtr()->getType();
			unsigned int byteCount = 
				bufferStatePtr_[i]->getAllocatorPtr()->getByteCount();


			if ( allocType == ALLOC_TYPE_DXT1 )
			{
				glCompressedTexImage2D(
					target_,							// target
					i,									// mipmap level
					internalFormat_,					// internal format
					w,									// mipmap width
					h,									// mipmap height
					0,									// border
					byteCount,							// image size
					NULL );								// data ptr
			}


			else
			{
				glTexImage2D( 
					target_,							// target
					i,									// mipmap level
					internalFormat_,					// internal format
					w,									// mipmap width
					h,									// mipmap height
					0,									// border
					format_,							// format
					type_,								// type
					NULL );								// data ptr
			}
		}
	}


	//glGenerateMipmap( textureTarget_ );
	//glTexParameteri( textureTarget_, GL_GENERATE_MIPMAP, GL_TRUE );
	glBindTexture( target_, 0 );

	

	// Create samplers and specifiy sampling properties
	//
	// http://www.openorg/sdk/docs/man4/xhtml/glBindSampler.xml
	//
	// OpenGL 3.3 introduced the sampler object wich separates the sampling
	// properties from the texture object (created with glGenTextures). The 
	// benefit is that a sampler definition can be reused on multiple texture
	// units. In this case we just have one sampler per unit as usual but still
	// use the sampler object.
	//
	// The sampler object is bound per texture unit and it overrides the 
	// default sampler for the associated texture. Unbinding the sampler with
	// with glBindSampler( , 0) will go back to the default sampler.
	//
	// Samplers are one of the first "direct access" objects in OpenGL rather
	// than the usual "bind-first"
	//
	glGenSamplers( 1, &samplerID_ );
	if ( mipLevelCount_ > 1 )
	{
		glSamplerParameteri( samplerID_, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR_MIPMAP_LINEAR );
		glSamplerParameteri( samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glSamplerParameteri( samplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glSamplerParameteri( samplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	else
	{
		glSamplerParameteri( samplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glSamplerParameteri( samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glSamplerParameteri( samplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glSamplerParameteri( samplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	

	// texture data is now declared
	isDeclared_ = true;
}

void
TextureState::undeclare()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// delete texture content and free up iondex for re-use
	if ( glIsTexture( textureID_ ) )
	{
		glDeleteTextures( 1, &textureID_ );
	}


	// delete sampler content and free up iondex for re-use
	if ( glIsSampler( samplerID_ ) )
	{
		glDeleteSamplers( 1, &samplerID_ );
	}


	// texture data is no longer declared
	isDeclared_ = true;
}

void
TextureState::unpack()
{
	// initial error handling
	if ( !isDeclared_ )
	{
		return;
	}


	// only unpack if buffer data has been modified since last call
	bool requireUnpack = false;
	for ( unsigned int i = 0; i < MAX_MIP_LEVELS; ++i )
	{
		requireUnpack |= trackerBufferUploadCount_[i].greaterPtrCpy();
		requireUnpack |= trackerBufferPackCount_[i].greaterPtrCpy();
	}
	if ( !requireUnpack )
	{
		return;
	}


	// One time messages
	if ( !(bufferStatePtr_[0] && bufferStatePtr_[0]->getAllocatorPtr()) )
	{
		GEM_ERROR( "First mip level is not complete." );
	}


	// Data in client memory or buffers are considered packed, so unpack us 
	// uploading to GPU. glTexImage2D is pack operations and loads data
	// from the currently bound buffer if one is bound and pointer argument
	// is NULL.
	//
	// Unpack data is separated from the texture declaration so you can upload
	// changes to the allocated without having to redeclare the texture.
	//
	// http://www.openorg/sdk/docs/man4/xhtml/glCompressedTexSubImage2D.xml
	// https://www.openorg/sdk/docs/man4/xhtml/glTexSubImage2D.xml
	//
	glBindTexture( target_, textureID_ );

	for ( unsigned int i = 0; i<mipLevelCount_; ++i )
	{
		if ( bufferStatePtr_[i] && bufferStatePtr_[i]->getAllocatorPtr() )
		{

			double w = bufferStatePtr_[0]->getAllocatorPtr()->getWidth();
			double h = bufferStatePtr_[0]->getAllocatorPtr()->getHeight();
			w = std::max( std::floor( w / (1 << i) ), 1.0 );
			h = std::max( std::floor( h / (1 << i) ), 1.0 );
			if ( w > bufferStatePtr_[i]->getAllocatorPtr()->getWidth() ||
				 h > bufferStatePtr_[i]->getAllocatorPtr()->getHeight() )
			{
				GEM_ERROR( "The allocator is smaller than what is"
							"required for this mip level " + i );
			}


			ALLOC_TYPE allocType = 
				bufferStatePtr_[i]->getAllocatorPtr()->getType();
			unsigned int byteCount = 
				bufferStatePtr_[i]->getAllocatorPtr()->getByteCount();


			glBindBuffer( GL_PIXEL_UNPACK_BUFFER,
						  bufferStatePtr_[i]->getBufferID() );


			if ( allocType == ALLOC_TYPE_DXT1 )
			{
				glCompressedTexSubImage2D(
					target_,							// target
					i,									// mipmap level
					0,									// xoffset
					0,									// yoffset
					w,									// mipmap width
					h,									// mipmap height
					format_,							// format
					byteCount,							// image size
					NULL );								// *data
			}


			else
			{
				glTexSubImage2D(
					target_,							// target
					i,									// mipmap level
					0,									// xoffset
					0,									// yoffset
					w,									// texture width
					h,									// texture height
					format_,							// format
					type_,							// type
					NULL );								// *data
			}


			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		}
	}

	glBindTexture( target_, 0 );
}

void
TextureState::bind()
{
	// texture data should already be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// Bind the texture to the correct texture unit
	// Bind a sampler to the correct texture unit
	glActiveTexture( GL_TEXTURE0 + textureUnit_ );
	glBindTexture( target_, textureID_ );
	glBindSampler( textureUnit_, samplerID_ );
}

void
TextureState::unbind()
{
	// texture data should already be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// Unbind the texture from the texture unit
	// Unbind the sampler from the texture unit
	glActiveTexture( GL_TEXTURE0 + textureUnit_ );
	glBindTexture( target_, 0 );
	glBindSampler( textureUnit_, 0 );
}

void
RenderState::declareTextures()
{
	// setup iterators before loop
	std::map<unsigned int,TextureState>::iterator i = textureStates_.begin();
	std::map<unsigned int,TextureState>::iterator iend = textureStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).second.declare();
	}
}

void
RenderState::undeclareTextures()
{
	// setup iterators before loop
	std::map<unsigned int,TextureState>::iterator i = textureStates_.begin();
	std::map<unsigned int,TextureState>::iterator iend = textureStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).second.undeclare();
	}
}

void
RenderState::unpackTextures()
{
	// setup iterators before loop
	std::map<unsigned int,TextureState>::iterator i = textureStates_.begin();
	std::map<unsigned int,TextureState>::iterator iend = textureStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).second.unpack();
	}
}

void
RenderState::bindTextures()
{
	// setup iterators before loop
	std::map<unsigned int,TextureState>::iterator i = textureStates_.begin();
	std::map<unsigned int,TextureState>::iterator iend = textureStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).second.bind();
	}
}
	
void
RenderState::unbindTextures()
{
	// setup iterators before loop
	std::map<unsigned int,TextureState>::iterator i = textureStates_.begin();
	std::map<unsigned int,TextureState>::iterator iend = textureStates_.end();


	// Declare each uniform
	for ( ; i != iend ; ++i )
	{
		(*i).second.unbind();
	}
}


//== INPUT: BufferTextureState =================================================



//== OUTPUT: TransformFeedbackState ============================================



//== OUTPUT: FramebufferState ==================================================


//-- constructors/destructor ---------------------------------------------------

FramebufferState::FramebufferState()
{
	clear();
}

FramebufferState::~FramebufferState()
{
	undeclare();
	clear();
}


//-- copy and clear ------------------------------------------------------------

void	
FramebufferState::clear()
{
	// OpenGL variables
	framebufferID_ = GL_NONE;
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		target_[i] = GL_NONE;
		internalFormat_[i] = GL_NONE;
		format_[i] = GL_NONE;
		type_[i] = GL_NONE;
		attachment_[i] = GL_NONE;
		renderbufferID_[i] = GL_NONE;
		drawBuffers_[i] = GL_NONE;
	}

	// BufferStates and trackers
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		bufferStatesPtr_[i] = NULL;
		trackerBufferDeclareCount_[i].clear();
	}

	// Counters and flags
	drawBuffersCount_ = 0;
	isDeclared_ = false;
}

//-- sets and gets -------------------------------------------------------------

void
FramebufferState::setAttachment( BufferState* const _bufferStatePtr,
								 FRAMEBUFFER_ATTACHMENT _framebufferAttachment )
{
	// initial error handling
	if ( !( _bufferStatePtr ) )
	{
		GEM_ERROR( "Buffer Object is not valid." );
	}


	// setup pointer and trackers
	bufferStatesPtr_[_framebufferAttachment] = _bufferStatePtr;
	trackerBufferDeclareCount_[_framebufferAttachment].set(
		_bufferStatePtr->getDeclareCountPtr(), 0 );
}

void
RenderState::setFramebuffer( TextureLoader* const _textureLoaderPtr,
							 FRAMEBUFFER_ATTACHMENT _framebufferAttachment )
{
	// initial error handling
	if ( !( _textureLoaderPtr && _textureLoaderPtr->isLoaded() ) )
	{
		GEM_ERROR( "Texture data is not loaded." );
	}
	if ( !( _textureLoaderPtr->getMipLevePtr( 0 ) ) )
	{
		GEM_ERROR( "Texture data is not loaded." );
	}


	// call Allocator version
	setFramebuffer( _textureLoaderPtr->getMipLevePtr( 0 ),
					_framebufferAttachment  );
}

void
RenderState::setFramebuffer( Allocator* const _allocatorPtr,
							 FRAMEBUFFER_ATTACHMENT _framebufferAttachment )
{
	// initial error handling
	if ( !( _allocatorPtr && _allocatorPtr->isAlloc() ) )
	{
		GEM_ERROR( "Texture data is not loaded." );
	}


	// Get the related BufferState to this Allocator, create new if needed
	// It is important to let the map create the instance of BufferState and
	// modify it via a pointer. BufferStates have self references and these
	// get out of date if we copy a buffer state by assignment
	BufferState* bufferStatePtr;
	if ( bufferStates_.count( _allocatorPtr ) == 0 )
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
		bufferStatePtr->setAllocator( _allocatorPtr );
		bufferStatePtr->setInitialTarget( GL_PIXEL_PACK_BUFFER );
		bufferStatePtr->setInitialUsage( GL_DYNAMIC_READ );
		
	}
	else
	{
		bufferStatePtr = &(bufferStates_[_allocatorPtr]);
	}


	// set this as an attachment on the framebuffer
	framebufferState_.setAttachment( bufferStatePtr, _framebufferAttachment );
}

//-- Buffer <--> OpenGL functions ----------------------------------------------

void
FramebufferState::declare()
{
	// TODO: initial error handling
	{}


	// The framebuffer state is invalidated if any of the underlying data
	// formats have changed.
	bool requireDeclare = false;
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		requireDeclare |= trackerBufferDeclareCount_[i].greaterPtrCpy();
	}
	if ( requireDeclare )
	{
		undeclare();
	}
	else
	{
		return;
	}


	// One time messages
	if ( !( bufferStatesPtr_[0] &&  bufferStatesPtr_[0]->getAllocatorPtr() ) )
	{
		GEM_ERROR( "No attachment to color0 found. This is required." );
	}


	// Setup the OpenGL variables for each attachment.
	//
	// Similar to textures, framebuffers have internal format that may differ
	// from the client side memory format and OpenGL will convert this in a 
	// best effort manner.
	//
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		if ( bufferStatesPtr_[i] &&  bufferStatesPtr_[i]->getAllocatorPtr() )
		{
			switch ( bufferStatesPtr_[i]->getAllocatorPtr()->getFormat() )
			{
			case ALLOC_FORMAT_VEC3_32F:
				target_[i] = GL_RENDERBUFFER;
				internalFormat_[i] = GL_RGB32F;
				format_[i] = GL_RGB;
				type_[i] = GL_FLOAT;
				break;
			default:
				target_[i] = GL_NONE;
				internalFormat_[i] = GL_NONE;
				format_[i] = GL_NONE;
				type_[i] = GL_NONE;
				GEM_ERROR( "Unsupported render buffer format." );
			}
			switch ( i )
			{
			case FRAMEBUFFER_ATTACHMENT_COLOR0:
				attachment_[i] = GL_COLOR_ATTACHMENT0;
				break;
			case FRAMEBUFFER_ATTACHMENT_COLOR1:
				attachment_[i] = GL_COLOR_ATTACHMENT1;
				break;
			case FRAMEBUFFER_ATTACHMENT_COLOR2:
				attachment_[i] = GL_COLOR_ATTACHMENT2;
				break;
			case FRAMEBUFFER_ATTACHMENT_COLOR3:
				attachment_[i] = GL_COLOR_ATTACHMENT3;
				break;
			case FRAMEBUFFER_ATTACHMENT_DEPTH:
				attachment_[i] = GL_DEPTH_ATTACHMENT;
				break;
			case FRAMEBUFFER_ATTACHMENT_STENCIL:
				attachment_[i] = GL_STENCIL_ATTACHMENT;
				break;
			default:
				attachment_[i] = GL_NONE;
				GEM_ERROR( "Unsupported render buffer format." );
			}
		}
	}


	// Create framebuffer and renderbuffer ID's and declare storage
	//
	// Similar to the vertex array object, the framebuffer object contains
	// the state for the entire framebuffer including bindings to individual
	// attachments. So you need to generate one ID for the framebuffer and 
	// then one for each renderbuffer and then bind them all together.
	// 
	// Storage is just reserved to match the size and format of the associated
	// buffer object. The unpack function will download this and should be
	// called after each draw to this buffer so other users of the buffer get
	// the up to date data
	//
	// You can also bind textures to the framebuffer which should be faster
	// as data probably never leaves OpenGL memory, but it is less flexible
	// because with the buffer approach you can bind the framebuffer output
	// anywhere, texture data, vertex data, buffer texture etc. Only the
	// buffered approach is currently supported.
	//

	glGenFramebuffers( 1, &framebufferID_ );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebufferID_ );
	drawBuffersCount_ = 0;
	
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		if ( bufferStatesPtr_[i] &&  bufferStatesPtr_[i]->getAllocatorPtr() )
		{

			double w = bufferStatesPtr_[i]->getAllocatorPtr()->getWidth();
			double h = bufferStatesPtr_[i]->getAllocatorPtr()->getHeight();


			// 1. generate render buffer id
			// 2. bind for succeeding commands
			// 3. declare storage (target, internal format), dont fill it
			// 4. unbind
			glGenRenderbuffers( 1, &(renderbufferID_[i]) );
			glBindRenderbuffer( target_[i],
								renderbufferID_[i] );
			glRenderbufferStorage( target_[i],
								   internalFormat_[i],
								   w,
								   h );
			glBindRenderbuffer( target_[i], 0 );


			// associate our new renderbuffer with an attachment point
			// on papa framebuffer
			glFramebufferRenderbuffer( GL_FRAMEBUFFER,
									   attachment_[i],
									   target_[i],
									   renderbufferID_[i] );

			
			// collect another attachment point for later use.
			drawBuffers_[drawBuffersCount_] = attachment_[i];
			drawBuffersCount_++;
		}
	}


	// specify what draw buffers to draw to based on our setup
	glDrawBuffers( drawBuffersCount_, drawBuffers_ );


	// we dont want to accidentaly change this object do we?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );


	// texture data is now declared
	isDeclared_ = true;
}

void
FramebufferState::undeclare()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// delete frambufer content and free up iondex for re-use
	if ( glIsFramebuffer( framebufferID_ ) )
	{
		glDeleteFramebuffers( 1, &framebufferID_ );
	}


	// delete renderbuffer content and free up iondex for re-use
	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		if ( glIsRenderbuffer( renderbufferID_[i] ) )
		{
			glDeleteRenderbuffers( 1, &renderbufferID_[i] );
		}
	}


	// texture data is no longer declared
	isDeclared_ = false;
}

void
FramebufferState::pack()
{
	if ( !isDeclared_ )
	{
		return;
	}

	// Pack data to the buffer object
	//
	// Buffer data (in client memoryy) is considered packed by OpenGL. The 
	// pack operations such as glReadPixels and glGetTexSubImage2D() downloads
	// data from OpenGL to the buffers
	//
	glBindFramebuffer( GL_FRAMEBUFFER, framebufferID_ );

	for ( unsigned int i = 0; i < MAX_FRAMEBUFFER_ATTACHMENTS; ++i )
	{
		if ( bufferStatesPtr_[i] &&  bufferStatesPtr_[i]->getAllocatorPtr() )
		{

			double w = bufferStatesPtr_[i]->getAllocatorPtr()->getWidth();
			double h = bufferStatesPtr_[i]->getAllocatorPtr()->getHeight();


			// 1. bind the buffer that we want to write TO
			// 2. define the attachment point on the framebuffer to read FROM
			// 3. do it
			// 4. unbind
			glBindBuffer( GL_PIXEL_PACK_BUFFER,
						  bufferStatesPtr_[i]->getBufferID() );
			glReadBuffer( attachment_[i] );
			glReadPixels( 0,					// x
						  0,					// y
						  w,					// width
						  h,					// height
						  format_[i],			// format
						  type_[i],				// type
						  NULL );				// NULL write to bound buffer
			glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

			
			// inform the buffer that its data has changed
			bufferStatesPtr_[i]->increasePackCount();
		}
	}


	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void
FramebufferState::bind()
{
	// texture data should already be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// Bind the framebuffer object
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebufferID_ );
}

void
FramebufferState::unbind()
{
	// texture data should already be declared
	if ( !isDeclared_ )
	{
		return;
	}


	// Unbind the framebuffer object
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}

void
RenderState::declareFramebuffer()
{
	framebufferState_.declare();
}

void
RenderState::undeclareFramebuffer()
{
	framebufferState_.undeclare();
}

void
RenderState::packFramebuffer()
{
	framebufferState_.pack();
}

void
RenderState::bindFramebuffer()
{
	framebufferState_.bind();
}

void
RenderState::unbindFramebuffer()
{
	framebufferState_.unbind();
}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================