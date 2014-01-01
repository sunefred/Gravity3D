//== INCLUDES ==================================================================

#include "GemRenderStage.h"
#include "GemMeshLoader.h"
#include "GemTextureLoader.h"
#include "GemShaderLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================


//-- constructors/destructor -----------------------------------------------

RenderStage::RenderStage()
// Draw call flags
: drawMode_( GL_NONE )
, hasTesselator_( false )
, vertexIndexDim_( 0 )
, vertexIndexCount_( 0 )
, vertexIndexType_( 0 )
// INPUT: Vertex Data
, vertexIndexDataPtr_( NULL )
, vertexArrayObjectID_( 0 )
, vertexIndexBufferID_( 0 )
, isDeclaredVertexData_( false )
// INPUT: Texture Data
, isDeclaredTextureData_( false )
// INPUT: Shader Program
, vertexShaderDataPtr_( NULL )
, tessCtrlShaderDataPtr_( NULL )
, tessEvalShaderDataPtr_( NULL )
, geometryShaderDataPtr_( NULL )
, fragmentShaderDataPtr_( NULL )
, vertexShaderID_( 0 )
, tessCtrlShaderID_( 0 )
, tessEvalShaderID_( 0 )
, geometryShaderID_( 0 )
, fragmentShaderID_( 0 )
, shaderProgramID_( 0 )
, isDeclaredShaderProgram_( false )
// INPUT: Uniforms
, isDeclaredUniforms_( false )
{
	// INPUT: Vertex Data
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		vertexAttributeDim_[i] = GL_NONE;
		vertexAttributeType_[i] = 0;
		vertexAttributeDataPtr_[i] = NULL;
		vertexAttributeBufferID_[i] = 0;
	}

	// INPUT: Texture Data
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		for ( unsigned int j=0; j<MAX_MIP_LEVELS; ++j )
		{
			textureDataPtr_[i][j] = NULL;
		}
		textureMipLevelCount_[i] = 0;
		textureUnit_[i] = 0;
		textureID_[i] = GL_NONE;
		textureTarget_[i] = GL_NONE;
		textureInternalFormat_[i] = GL_NONE;
		textureFormat_[i] = GL_NONE;
		textureType_[i] = GL_NONE;
		samplerID_[i] = 0;
	}
}

RenderStage::~RenderStage()
{
	// UNDECLARE
	// ---------
	// Do this to release OpenGL resources
	if ( isDeclaredShaderProgram_ )
	{
		undeclareShaderProgram();
	}
	if ( isDeclaredUniforms_ )
	{
		undeclareUniforms();
	}
	if ( isDeclaredVertexData_ )
	{
		undeclareVertexData();
	}
	if ( isDeclaredTextureData_ )
	{
		undeclareVertexData();
	}
}


//-- the star function of the entire library -----------------------------------

void
RenderStage::draw()
{
	// DECLARE
	// -------
	// Defines buffers, textures, compiles and links shader program etc
	// Re-declare is only performed if an underlying data format have changed.
	if ( isShaderProgramAllocCount() )
	{
		declareShaderProgram();
		declareUniforms();	
		resetShaderProgramAllocCount();
	}
	if ( isVertexDataAllocCount() )
	{
		declareVertexData();
		resetVertexDataAllocCount();
	}
	if ( isTextureDataAllocCount() )
	{
		declareTextureData();
		resetTextureDataAllocCount();
	}


	// UPLOAD
	// -------
	// Upload is separated from declare because we dont want to re-delare each
	// time data _content_ is changing (possibly every frame)
	// Data is only uploaded if it has changed
	if ( isVertexDataWriteCount() )
	{
		uploadVertexData();
		resetVertexDataWriteCount();
	}
	if ( isTextureDataWriteCount() )
	{
		uploadTextureData();
		resetTextureDataWriteCount();
	}


	// BIND
	// ----
	// glUseProgram(), upload uniforms, glBindVertexArray() etc
	if ( isDeclaredShaderProgram_ )
	{
		bindShaderProgram();
	}
	if ( isDeclaredUniforms_ )
	{
		bindUniforms();
	}
	if ( isDeclaredVertexData_ )
	{
		bindVertexData();
	}
	if ( isDeclaredTextureData_ )
	{
		bindTextureData();
	}


	// DRAW
	// ----
	// Use different draw mode for a shader program with a tesselator
	if ( hasTesselator_ )
	{
		glPatchParameteri( GL_PATCH_VERTICES, vertexIndexDim_ );
		glDrawElements( GL_PATCHES,
						vertexIndexDim_*vertexIndexCount_,
						vertexIndexType_,
						0 );
	}
	else
	{
		glDrawElements( drawMode_,
						vertexIndexDim_*vertexIndexCount_,
						vertexIndexType_,
						0 );
	}


	// UNBIND
	// ------
	// Go to default drawing mode, only required if there is draw code outside
	// of this class, which there shouldn't be
	if ( isDeclaredShaderProgram_ )
	{
		unbindShaderProgram();
	}
	if ( isDeclaredVertexData_ )
	{
		unbindVertexData();
	}
	if ( isDeclaredTextureData_ )
	{
		unbindTextureData();
	}

}


//-- INPUT: mesh data ----------------------------------------------------------

void
RenderStage::setMesh( MeshLoader* const _meshLoaderPtr )
{
	if ( _meshLoaderPtr && _meshLoaderPtr->isLoaded() )
	{
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
	else
	{
		GEM_ERROR( "Mesh data is not loaded." );
	}
}

void
RenderStage::setVertexIndexData( Allocator* const _allocatorPtr )
{
	if ( _allocatorPtr && _allocatorPtr->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareVertexData();

		// save data pointer
		vertexIndexDataPtr_ = _allocatorPtr;
	}
	else
	{
		GEM_ERROR( "Vertex Index Data is not allocated." );
	}
}

void
RenderStage::setVertexAttributeData( Allocator* const _allocatorPtr, 
										unsigned int _attrID )
{
	if ( _allocatorPtr && _allocatorPtr->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareVertexData();

		// save data pointer
		vertexAttributeDataPtr_[_attrID] = _allocatorPtr;
	}
	else
	{
		GEM_ERROR( "Vertex Attribute Data is not allocated." );
	}
}

void
RenderStage::declareVertexData()
{
	// release resources and redeclare
	if ( isDeclaredVertexData_ )
		undeclareVertexData();


	// Setup the OpenGL constants for primitives and vertex attributes.
	//
	// This is the RAM data format AND the internal format. No special
	// representation of the internal data format for vertex data.
	//
	if ( vertexIndexDataPtr_ )
	{
		switch ( vertexIndexDataPtr_->getFormat() )
		{
		case ALLOC_FORMAT_SCALAR_32UI:
			drawMode_ = GL_POINT;
			vertexIndexDim_ = 1;
			vertexIndexType_ = GL_UNSIGNED_INT;
			break;
		case ALLOC_FORMAT_VEC2_32UI:
			drawMode_ = GL_LINE;
			vertexIndexDim_ = 2;
			vertexIndexType_ = GL_UNSIGNED_INT;
			break;
		case ALLOC_FORMAT_VEC3_32UI:
			drawMode_ = GL_TRIANGLES;
			vertexIndexDim_ = 3;
			vertexIndexType_ = GL_UNSIGNED_INT;
			break;
		case ALLOC_FORMAT_VEC4_32UI:
			drawMode_ = GL_QUADS;
			vertexIndexDim_ = 4;
			vertexIndexType_ = GL_UNSIGNED_INT;
			break;
		default:
			drawMode_ = GL_NONE;
			vertexIndexDim_ = 0;
			vertexIndexType_ = GL_NONE;
			GEM_ERROR( "Unsupported primitive format." );
		}
		vertexIndexCount_ = vertexIndexDataPtr_->getNumElements();
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] )
		{
			switch ( vertexAttributeDataPtr_[i]->getFormat() )
			{
			case ALLOC_FORMAT_SCALAR_32UI:
				vertexAttributeDim_[i] = 1;
				vertexAttributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_SCALAR_32F:
				vertexAttributeDim_[i] = 1;
				vertexAttributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC2_32UI:
				vertexAttributeDim_[i] = 2;
				vertexAttributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC2_32F:
				vertexAttributeDim_[i] = 2;
				vertexAttributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC3_32UI:
				vertexAttributeDim_[i] = 3;
				vertexAttributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC3_32F:
				vertexAttributeDim_[i] = 3;
				vertexAttributeType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC4_32UI:
				vertexAttributeDim_[i] = 4;
				vertexAttributeType_[i] = GL_UNSIGNED_INT;
				break;
			case ALLOC_FORMAT_VEC4_32F:
				vertexAttributeDim_[i] = 4;
				vertexAttributeType_[i] = GL_FLOAT;
				break;
			default:
				vertexAttributeDim_[i] = 0;
				vertexAttributeType_[i] = GL_NONE;
				GEM_ERROR( "Unsupported vertex attribute format." );
			}
		}
	}


    // Create the vertex array object.
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
	glGenVertexArrays( 1, &vertexArrayObjectID_ );
    glBindVertexArray( vertexArrayObjectID_ );


	// Create the Index Buffer Object and allocate (dont copy yet)
	if ( vertexIndexDataPtr_ &&	 vertexIndexType_ != GL_NONE )
	{
		glGenBuffers( 1, &vertexIndexBufferID_ );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vertexIndexBufferID_ );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
					  vertexIndexDataPtr_->getByteCount(),
					  NULL,
					  GL_STATIC_DRAW );
	}


    // Create the Vertex Buffer Object (VBO) and allocate (dont copy yet)
	// We use a VBO for each vertex attribute so data is ordered sequentially:
	//
	//	v0 v1 v2 ... n0 n1 n2 ...
	//
	// Alternatives to this is interlieved attributes which might be faster
	// in some cases
	//
	//	v0 n0 ... v1 n1 ... v2 n2 ...
	//
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] && vertexAttributeType_[i] != GL_NONE )
		{
			glGenBuffers( 1, &vertexAttributeBufferID_[i] );
			glBindBuffer( GL_ARRAY_BUFFER, vertexAttributeBufferID_[i] );
			glBufferData( GL_ARRAY_BUFFER,
						  vertexAttributeDataPtr_[i]->getByteCount(),
						  NULL,
						  GL_STATIC_DRAW );
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
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] && vertexAttributeType_[i] != GL_NONE )
		{
			glBindBuffer( GL_ARRAY_BUFFER, vertexAttributeBufferID_[i] );
			glVertexAttribPointer( i, vertexAttributeDim_[i],
								   vertexAttributeType_[i], GL_FALSE, 0, 0);
			glEnableVertexAttribArray( i );
		}
	}


	// we might not want this vertex array object bound in the next glCall
	glBindVertexArray( 0 );


	// vertex data is now declared
	isDeclaredVertexData_ = true;
}

void
RenderStage::undeclareVertexData()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// Delete vertex array object contex and free up index for re-use.
	if ( glIsVertexArray( vertexArrayObjectID_ ) )
	{
		glDeleteVertexArrays( 1, &vertexArrayObjectID_ );
	}


	// delete vertex index content and free up index for re-use
	if ( glIsBuffer( vertexIndexBufferID_ ) )
	{
		glDeleteBuffers( 1, &vertexIndexBufferID_ );
	}


	// delete vertex attribute content and free up index for re-use
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( glIsBuffer( vertexAttributeBufferID_[i] ) )
		{
			glDeleteBuffers( 1, &vertexAttributeBufferID_[i] );
		}
	}
}

void
RenderStage::uploadVertexData()
{
	// vertex data should already be declared
	if ( !isDeclaredVertexData_ )
		return;


	//
	// For updating you use the VBO indices and you dont need to have the
	// the Vertex Array Buffer active
	//


	// upload vertex indices and save state to track changes to data
	if ( vertexIndexDataPtr_ &&
		 vertexIndexDataPtr_->getWriteCount() > 0 &&
		 glIsBuffer( vertexIndexBufferID_ ) )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vertexIndexBufferID_ );
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
						 0,
						 vertexIndexDataPtr_->getByteCount(),
						 vertexIndexDataPtr_->getReadPtr<void>() );
	}


	// upload vertex attributes and save state to track changes to data
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] &&
			 vertexAttributeDataPtr_[i]->getWriteCount() > 0 &&
			 glIsBuffer( vertexAttributeBufferID_[i] ) )
		{
			glBindBuffer( GL_ARRAY_BUFFER, vertexAttributeBufferID_[i] );
			glBufferSubData( GL_ARRAY_BUFFER,
							 0,
							 vertexAttributeDataPtr_[i]->getByteCount(),
							 vertexAttributeDataPtr_[i]->getReadPtr<void>() );
		}
	}
}

void
RenderStage::downloadVertexData()
{
	// can't download if GPU data has not been declared
	if ( !isDeclaredVertexData_ )
		return;


	// download vertex indices
	if ( vertexIndexDataPtr_ &&
		 glIsBuffer( vertexIndexBufferID_ ) )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vertexIndexBufferID_ );
		glGetBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
							0,
							vertexIndexDataPtr_->getByteCount(),
							vertexIndexDataPtr_->getWritePtr<void>() );
	}


	// download vertex attributes
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] &&
			 glIsBuffer( vertexAttributeBufferID_[i] ) )
		{
			glBindBuffer( GL_ARRAY_BUFFER, vertexAttributeBufferID_[i] );
			glGetBufferSubData( GL_ARRAY_BUFFER,
								0,
								vertexAttributeDataPtr_[i]->getByteCount(),
								vertexIndexDataPtr_->getWritePtr<void>() );
		}
	}
}

void
RenderStage::bindVertexData()
{
	// vertex data should already be declared
	if ( !isDeclaredVertexData_ )
		return;


	// Because we use vertex array objects all we need to do here is bind
	glBindVertexArray( vertexArrayObjectID_ );
}

void
RenderStage::unbindVertexData()
{
	// unbind vertex array, leave it to default state
	glBindVertexArray( vertexArrayObjectID_ );
}


//-- INPUT: texture data ---------------------------------------------------

void
RenderStage::setTexture( TextureLoader* const _textureLoaderPtr,
						 unsigned int _textureUnit )
{
	if ( !(_textureLoaderPtr && _textureLoaderPtr->isLoaded()) )
	{
		GEM_ERROR( "Texture data is not loaded." );
	}
	if ( !( 0 <= _textureUnit && _textureUnit < MAX_TEXTURE_UNITS ) )
	{
		GEM_ERROR( "Texture unit specifier out of range." );
	}


	// set mip level count
	textureMipLevelCount_[_textureUnit] = Math::min<unsigned int>( 
										_textureLoaderPtr->getMipLevelCount(),
										MAX_MIP_LEVELS );

	// Loop over texture mip levels until we find one that is not allocated
	for ( unsigned int i = 0; i < textureMipLevelCount_[_textureUnit]; ++i )
	{
		textureDataPtr_[_textureUnit][i] = _textureLoaderPtr->getMipLevePtr(i);
	}

}

void
RenderStage::declareTextureData()
{
	// release resources and redeclare
	if ( isDeclaredTextureData_ )
		undeclareTextureData();


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
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] )
		{
			switch ( textureDataPtr_[i][0]->getFormat() )
			{
			case ALLOC_FORMAT_SCALAR_8UI:
				textureTarget_[i] = GL_TEXTURE_2D;
				textureInternalFormat_[i] = GL_R8;
				textureFormat_[i] = GL_RED;
				textureType_[i] = GL_UNSIGNED_BYTE;
				break;
			case ALLOC_FORMAT_VEC3_8UI:
				textureTarget_[i] = GL_TEXTURE_2D;
				textureInternalFormat_[i] = GL_RGB8;
				textureFormat_[i] = GL_RGB;
				textureType_[i] = GL_UNSIGNED_BYTE;
				break;
			case ALLOC_FORMAT_SCALAR_32F:
				textureTarget_[i] = GL_TEXTURE_2D;
				textureInternalFormat_[i] = GL_R32F;
				textureFormat_[i] = GL_RED;
				textureType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC3_32F:
				textureTarget_[i] = GL_TEXTURE_2D;
				textureInternalFormat_[i] = GL_RGB32F;
				textureFormat_[i] = GL_RGB;
				textureType_[i] = GL_FLOAT;
				break;
			case ALLOC_FORMAT_VEC3_DXT1:
				textureTarget_[i] = GL_TEXTURE_2D;
				textureInternalFormat_[i] = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				textureFormat_[i] = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				textureType_[i] = GL_UNSIGNED_BYTE;
				break;
			default:
				textureTarget_[i] = GL_NONE;
				textureInternalFormat_[i] = GL_NONE;
				textureFormat_[i] = GL_NONE;
				textureType_[i] = GL_NONE;
				GEM_ERROR( "Unsupported texture format." );
			}
		}
	}


	// Create the texture names and specify format.
	//
	// http://www.opengl.org/sdk/docs/man4/xhtml/glGenTextures.xml
	// http://www.opengl.org/sdk/docs/man4/xhtml/glBindTexture.xml
	// http://www.opengl.org/sdk/docs/man4/xhtml/glTexImage2D.xml
	// http://www.opengl.org/sdk/docs/man4/xhtml/glActiveTexture.xml
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
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] && textureType_[i] != GL_NONE )
		{
			glGenTextures( 1, &textureID_[i] );
			glBindTexture( textureTarget_[i], textureID_[i] );
			for ( unsigned int j = 0; j<textureMipLevelCount_[i]; ++j )
			{
				double w = textureDataPtr_[i][0]->getWidth();
				double h = textureDataPtr_[i][0]->getHeight();
				w = std::max( std::floor( w / (1 << j) ), 1.0 );
				h = std::max( std::floor( h / (1 << j) ), 1.0 );

				if ( textureDataPtr_[i][0]->getType() == ALLOC_TYPE_DXT1 )
				{

					glCompressedTexImage2D(
						textureTarget_[i],					// target
						j,									// mipmap level
						textureInternalFormat_[i],			// internal format
						w,									// mipmap width
						h,									// mipmap height
						0,									// border
						textureDataPtr_[i][j]->getByteCount(),	// image size
						NULL );								// data ptr
				}

				else
				{
					glTexImage2D( 
						textureTarget_[i],					// target
						j,									// mipmap level
						textureInternalFormat_[i],			// internal format
						w,									// mipmap width
						h,									// mipmap height
						0,									// border
						textureFormat_[i],					// format
						textureType_[i],					// type
						NULL );								// data ptr
				}
			}
			//glGenerateMipmap( textureTarget_[i] );
			//glTexParameteri( textureTarget_[i], GL_GENERATE_MIPMAP, GL_TRUE );
			glBindTexture(  textureTarget_[i], 0 );
		}
	}
	

	// Create samplers and specifiy sampling properties
	//
	// http://www.opengl.org/sdk/docs/man4/xhtml/glBindSampler.xml
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
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] && textureType_[i] != GL_NONE )
		{
			glGenSamplers( 1, &samplerID_[i] );
			if ( textureMipLevelCount_[i] > 1 )
			{
				glSamplerParameteri( samplerID_[i],
									 GL_TEXTURE_MIN_FILTER,
									 GL_LINEAR_MIPMAP_LINEAR );
				glSamplerParameteri( samplerID_[i],
									 GL_TEXTURE_MAG_FILTER,
									 GL_LINEAR );
				}
			else
			{
				glSamplerParameteri( samplerID_[i],
									 GL_TEXTURE_MIN_FILTER,
									 GL_LINEAR );
				glSamplerParameteri( samplerID_[i],
									 GL_TEXTURE_MAG_FILTER,
									 GL_LINEAR );
			}
			glSamplerParameteri( samplerID_[i],
								 GL_TEXTURE_WRAP_S,
								 GL_REPEAT );
			glSamplerParameteri( samplerID_[i],
								 GL_TEXTURE_WRAP_T,
								 GL_REPEAT );
		}
	}


	// texture data is now declared
	isDeclaredTextureData_ = true;
}

void
RenderStage::undeclareTextureData()
{
	// because the declare might fail mid-function, we run this code
	// regardless of the state of isDeclared


	// delete texture content and free up iondex for re-use
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( glIsTexture( textureID_[i] ) )
		{
			glDeleteTextures( 1, &textureID_[i] );
		}
	}


	// delete sampler content and free up iondex for re-use
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( glIsSampler( samplerID_[i] ) )
		{
			glDeleteSamplers( 1, &samplerID_[i] );
		}
	}
}

void
RenderStage::uploadTextureData()
{
	// Upload data is separated from the texture declaration so you can upload
	// changes to the allocated without having to redeclare the texture.
	//
	// http://www.opengl.org/sdk/docs/man4/xhtml/glCompressedTexSubImage2D.xml
	// https://www.opengl.org/sdk/docs/man4/xhtml/glTexSubImage2D.xml
	//
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] && textureType_[i] != GL_NONE )
		{
			glBindTexture( textureTarget_[i], textureID_[i] );
			for ( unsigned int j = 0; j<textureMipLevelCount_[i]; ++j )
			{
				double w = textureDataPtr_[i][0]->getWidth();
				double h = textureDataPtr_[i][0]->getHeight();
				w = std::max( std::floor( w / (1 << j) ), 1.0 );
				h = std::max( std::floor( h / (1 << j) ), 1.0 );
				if ( w > textureDataPtr_[i][j]->getWidth() ||
					 h > textureDataPtr_[i][j]->getHeight() )
				{
					GEM_ERROR( "The allocator is smaller than what is"
							   "required for this mip level " + j );
				}

				if ( textureDataPtr_[i][0]->getType() == ALLOC_TYPE_DXT1 )
				{
					glCompressedTexSubImage2D(
						textureTarget_[i],					// target
						j,									// mipmap level
						0,									// xoffset
						0,									// yoffset
						w,									// mipmap width
						h,									// mipmap height
						textureFormat_[i],					// format
						textureDataPtr_[i][j]->getByteCount(),	// image size
						textureDataPtr_[i][j]->getReadPtr<GLvoid>() ); // *data
				}

				else
				{
					glTexSubImage2D(
						textureTarget_[i],					// target
						j,									// mipmap level
						0,									// xoffset
						0,									// yoffset
						w,									// texture width
						h,									// texture height
						textureFormat_[i],					// format
						textureType_[i],					// type
						textureDataPtr_[i][j]->getReadPtr<GLvoid>() ); // *data
				}
			}
			glBindTexture( textureTarget_[i], 0 );
		}
	}
}

void
RenderStage::downloadTextureData()
{
	

}

void
RenderStage::bindTextureData()
{
	// texture data should already be declared
	if ( !isDeclaredVertexData_ )
		return;

	// Bind the texture to the correct texture unit
	// Bind a sampler to the correct texture unit
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i] && textureType_[i] != GL_NONE )
		{
			glActiveTexture( GL_TEXTURE0 + textureUnit_[i] );
			glBindTexture( textureTarget_[i], textureID_[i] );
			glBindSampler( textureUnit_[i], samplerID_[i] );
		}
	}
}
	
void
RenderStage::unbindTextureData()
{
	// Unbind the texture from the texture unit
	// Unbind the sampler from the texture unit
	for ( unsigned int i = 0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i] && textureType_[i] != GL_NONE )
		{
			glActiveTexture( GL_TEXTURE0 + textureUnit_[i] );
			glBindTexture( textureTarget_[i], 0 );
			glBindSampler( textureUnit_[i], 0 );
		}
	}
}


//--INPUT: shader program -----------------------------------------------------

void
RenderStage::setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr )
{
	if ( _vertexShaderLoaderPtr &&
		 _vertexShaderLoaderPtr->getSourcePtr() &&
		 _vertexShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareShaderProgram();

		// save data pointer
		vertexShaderDataPtr_ = _vertexShaderLoaderPtr->getSourcePtr();
	}
}

void
RenderStage::setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr )
{
	if ( _fragmentShaderLoaderPtr &&
		 _fragmentShaderLoaderPtr->getSourcePtr() &&
		 _fragmentShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareShaderProgram();

		// save data pointer
		fragmentShaderDataPtr_ = _fragmentShaderLoaderPtr->getSourcePtr();
	}
}

void
RenderStage::setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr )
{
	if ( _geometryShaderLoaderPtr &&
		 _geometryShaderLoaderPtr->getSourcePtr() &&
		 _geometryShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareShaderProgram();

		// save data pointer
		geometryShaderDataPtr_ = _geometryShaderLoaderPtr->getSourcePtr();
	}
}

void
RenderStage::setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr )
{
	if ( _tessCtrlShaderLoaderPtr &&
		 _tessCtrlShaderLoaderPtr->getSourcePtr() &&
		 _tessCtrlShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareShaderProgram();

		// save data pointer
		tessCtrlShaderDataPtr_ = _tessCtrlShaderLoaderPtr->getSourcePtr();
	}
}

void
RenderStage::setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr )
{
	if ( _tessEvalShaderLoaderPtr &&
		 _tessEvalShaderLoaderPtr->getSourcePtr() &&
		 _tessEvalShaderLoaderPtr->getSourcePtr()->isAlloc() )
	{
		// undeclare before referencing new data
		undeclareShaderProgram();

		// save data pointer
		tessEvalShaderDataPtr_ = _tessEvalShaderLoaderPtr->getSourcePtr();
	}
}

void
RenderStage::declareShaderProgram()
{
	// release resources and redeclare
	if ( isDeclaredShaderProgram_ )
		undeclareShaderProgram();


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
	shaderProgramID_ = glCreateProgram();
	if ( vertexShaderDataPtr_ )
		glAttachShader( shaderProgramID_, vertexShaderID_ );
	if ( tessCtrlShaderDataPtr_ )
		glAttachShader( shaderProgramID_, tessCtrlShaderID_ );
	if ( tessEvalShaderDataPtr_ )			
		glAttachShader( shaderProgramID_, tessEvalShaderID_ );
	if ( geometryShaderDataPtr_ )
		glAttachShader( shaderProgramID_, geometryShaderID_ );
	if ( fragmentShaderDataPtr_ )
		glAttachShader( shaderProgramID_, fragmentShaderID_ );
	glLinkProgram( shaderProgramID_ );


	// One stop error handling. If the shader compiles fail this will show up
	// in the the program link infor log, thats why we dont bother handling
	// these individually
	GLint linkStatus = 0;
	GLsizei infoLogSize = 0;
	std::string infoLog;
	glGetProgramiv( shaderProgramID_, GL_LINK_STATUS, &linkStatus);
	if ( !linkStatus )
	{
		glGetProgramiv( shaderProgramID_, GL_INFO_LOG_LENGTH, &infoLogSize );
		infoLog.resize( infoLogSize );
		glGetProgramInfoLog( shaderProgramID_, infoLogSize,
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


	// hader data is now declared
	isDeclaredShaderProgram_ = true;
}

void
RenderStage::undeclareShaderProgram()
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
	if ( glIsProgram( shaderProgramID_ ) )
	{
		glDeleteProgram( shaderProgramID_ );
	}
}


void
RenderStage::bindShaderProgram()
{
	// shader program should already be declared
	if ( !isDeclaredShaderProgram_ )
		return;

	// bind program
	glUseProgram( shaderProgramID_ );
}


void
RenderStage::unbindShaderProgram()
{
	// unbind program, leave it to default state
	glUseProgram( 0 );
}


//-- INPUT UNIFORMS ------------------------------------------------------------

void
RenderStage::setUniform( int* const _intPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _intPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _intPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_SCALAR_32I );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( unsigned int* const _uintPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _uintPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _uintPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_SCALAR_32UI );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( float* const _floatPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _floatPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _floatPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_SCALAR_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec2i* const _vec2iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2iPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec2iPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC2_32I );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec2ui* const _vec2uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2uiPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec2uiPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC2_32UI );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec2f* const _vec2fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec2fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec2fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC2_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec3i* const _vec3iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3iPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec3iPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC3_32I );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec3ui* const _vec3uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3uiPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec3uiPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC3_32UI );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec3f* const _vec3fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec3fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec3fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC3_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec4i* const _vec4iPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4iPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec4iPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC4_32I );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec4ui* const _vec4uiPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4uiPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec4uiPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC4_32UI );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Vec4f* const _vec4fPtr, 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _vec4fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _vec4fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_VEC4_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Mat2f* const _mat2fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat2fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _mat2fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_MAT2_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Mat3f* const _mat3fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat3fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _mat3fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_MAT3_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}

void
RenderStage::setUniform( Mat4f* const _mat4fPtr,			 
						 const std::string& _name,
						 const unsigned int _count )
{
	if ( _mat4fPtr )
	{
		uniformPtr_.push_back( static_cast<void*>( _mat4fPtr ) );
		uniformName_.push_back( _name );
		uniformFormat_.push_back( ALLOC_FORMAT_MAT4_32F );
		uniformCount_.push_back( _count );
		unformLocation_.push_back( -1 );
	}
	else
	{
		GEM_ERROR( "Uniform Data is not allocated." );
	}
}


void
RenderStage::declareUniforms()
{
	if ( isDeclaredUniforms_ )
		undeclareUniforms();


	// setup iterators before loop
	std::vector<std::string>::const_iterator i = uniformName_.begin();
	std::vector<std::string>::const_iterator iend = uniformName_.end();
	std::vector<GLint>::iterator j = unformLocation_.begin();
	std::vector<GLint>::iterator jend = unformLocation_.end();


	// loop over all uniform names and get id from shader program
	for ( ; i!=iend, j!=jend; ++i, ++j )
	{
		(*j) = glGetUniformLocation( shaderProgramID_, (*i).c_str() );
		if ( (*j) == -1 )
		{
			GEM_ERROR( "Uniform not found in shader program: " + (*i) );
		}
	}


	// mark as declared to avoid reruns of location retrieval
	isDeclaredUniforms_  = true;
}


void
RenderStage::undeclareUniforms()
{
	// uniforms should already be declared
	if ( !isDeclaredUniforms_ )
		return;


	// setup iterators before loop
	std::vector<GLint>::iterator i = unformLocation_.begin();
	std::vector<GLint>::iterator iend = unformLocation_.end();


	// loop over all uniform and set location to -1 (error value)
	for ( ; i!=iend; ++i )
	{
		(*i) = -1;
	}


	// mark as undeclared
	isDeclaredUniforms_  = false;
}


void
RenderStage::bindUniforms()
{
	// declare before uploading
	if ( !isDeclaredUniforms_ )
		declareUniforms();


	// setup iterators before loop
	std::vector<void*>::const_iterator i = uniformPtr_.begin();
	std::vector<void*>::const_iterator iend = uniformPtr_.end();
	std::vector<ALLOC_FORMAT>::const_iterator j = uniformFormat_.begin();
	std::vector<ALLOC_FORMAT>::const_iterator jend = uniformFormat_.end();
	std::vector<unsigned int>::const_iterator k = uniformCount_.begin();
	std::vector<unsigned int>::const_iterator kend = uniformCount_.end();
	std::vector<GLint>::const_iterator l = unformLocation_.begin();
	std::vector<GLint>::const_iterator lend = unformLocation_.end();


	// loop over all the registered uniforms and call the correct OpenGL func
	for ( ; i!=iend && j!=jend && k!=kend && l!=lend; ++i, ++j, ++k, ++l )
	{

		void* ptr = (*i);
		ALLOC_FORMAT format = (*j);
		unsigned int count = (*k);
		GLint location = (*l);
		

		// skip unfound uniforms
		if ( location == -1 )
		{
			continue;
		}


		// call correct OpenGL function
		switch ( format )
		{
		
		// scalars
		case ALLOC_FORMAT_SCALAR_32I:
			glUniform1iv( location, count, 
						  reinterpret_cast<GLint*>( ptr ) );
			break;
		case ALLOC_FORMAT_SCALAR_32UI:
			glUniform1uiv( location, count, 
						   reinterpret_cast<GLuint*>( ptr ) );
			break;
		case ALLOC_FORMAT_SCALAR_32F:
			glUniform1fv( location, count, 
						  reinterpret_cast<GLfloat*>( ptr ) );
			break;

		// vectors
		case ALLOC_FORMAT_VEC2_32I:
			glUniform1iv( location, count, 
						  reinterpret_cast<GLint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC2_32UI:
			glUniform1uiv( location, count, 
						   reinterpret_cast<GLuint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC2_32F:
			glUniform1fv( location, count, 
						  reinterpret_cast<GLfloat*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC3_32I:
			glUniform1iv( location, count, 
						  reinterpret_cast<GLint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC3_32UI:
			glUniform1uiv( location, count, 
						   reinterpret_cast<GLuint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC3_32F:
			glUniform1fv( location, count, 
						  reinterpret_cast<GLfloat*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC4_32I:
			glUniform1iv( location, count, 
						  reinterpret_cast<GLint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC4_32UI:
			glUniform1uiv( location, count, 
						   reinterpret_cast<GLuint*>( ptr ) );
			break;
		case ALLOC_FORMAT_VEC4_32F:
			glUniform1fv( location, count, 
						  reinterpret_cast<GLfloat*>( ptr ) );
			break;

		// matrices
		case ALLOC_FORMAT_MAT2_32F:
			glUniformMatrix4fv( location, count, GL_TRUE,
								reinterpret_cast<GLfloat*>( ptr ) );
			break;
		case ALLOC_FORMAT_MAT3_32F:
			glUniformMatrix4fv( location, count, GL_TRUE,
								 reinterpret_cast<GLfloat*>( ptr ) );
			break;
		case ALLOC_FORMAT_MAT4_32F:
			glUniformMatrix4fv( location, count, GL_TRUE,
								reinterpret_cast<GLfloat*>( ptr ) );
			break;

		default:
			break;

		}
	}
}


//-- data change evaluators ----------------------------------------------------

bool
RenderStage::isVertexDataAllocCount( )
{
	unsigned int allocCount = 0;
	if ( vertexIndexDataPtr_ )
	{
		allocCount += vertexIndexDataPtr_->getAllocCount();
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] )
		{
			allocCount += vertexAttributeDataPtr_[i]->getAllocCount();
		}
	}
	return (allocCount > 0);
}


bool
RenderStage::isVertexDataWriteCount( )
{
	unsigned int writeCount = 0;
	if ( vertexIndexDataPtr_ )
	{
		writeCount += vertexIndexDataPtr_->getWriteCount();
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] )
		{
			writeCount += vertexAttributeDataPtr_[i]->getWriteCount();
		}
	}
	return (writeCount > 0);
}

bool
RenderStage::isTextureDataAllocCount( )
{
	unsigned int allocCount = 0;
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] )
		{
			allocCount += textureDataPtr_[i][0]->getAllocCount();
		}
	}
	return (allocCount > 0);
}


bool
RenderStage::isTextureDataWriteCount( )
{
	unsigned int writeCount = 0;
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] )
		{
			writeCount += textureDataPtr_[i][0]->getWriteCount();
		}
	}
	return (writeCount > 0);
}

bool
RenderStage::isShaderProgramAllocCount( )
{
	unsigned int allocCount = 0;
	if ( vertexShaderDataPtr_ )
	{
		allocCount += vertexShaderDataPtr_->getAllocCount();
	}
	if ( tessCtrlShaderDataPtr_ )
	{
		allocCount += tessCtrlShaderDataPtr_->getAllocCount();
	}
	if ( tessEvalShaderDataPtr_ )
	{
		allocCount += tessEvalShaderDataPtr_->getAllocCount();
	}
	if ( geometryShaderDataPtr_ )
	{
		allocCount += geometryShaderDataPtr_->getAllocCount();
	}
	if ( fragmentShaderDataPtr_ )
	{
		allocCount += fragmentShaderDataPtr_->getAllocCount();
	}
	return (allocCount > 0);
}

void
RenderStage::resetVertexDataAllocCount( )
{
	if ( vertexIndexDataPtr_ )
	{
		vertexIndexDataPtr_->resetAllocCount();
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] )
		{
			vertexAttributeDataPtr_[i]->resetAllocCount();
		}
	}
}

void
RenderStage::resetVertexDataWriteCount( )
{
	if ( vertexIndexDataPtr_ )
	{
		vertexIndexDataPtr_->resetWriteCount();
	}
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		if ( vertexAttributeDataPtr_[i] )
		{
			vertexAttributeDataPtr_[i]->resetWriteCount();
		}
	}
}

void
RenderStage::resetTextureDataAllocCount( )
{
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] )
		{
			textureDataPtr_[i][0]->resetAllocCount();
		}
	}
}


void
RenderStage::resetTextureDataWriteCount( )
{
	for ( unsigned int i=0; i<MAX_TEXTURE_UNITS; ++i )
	{
		if ( textureDataPtr_[i][0] )
		{
			textureDataPtr_[i][0]->resetWriteCount();
		}
	}
}

void
RenderStage::resetShaderProgramAllocCount( )
{
	if ( vertexShaderDataPtr_ )
	{
		vertexShaderDataPtr_->resetAllocCount();
	}
	if ( tessCtrlShaderDataPtr_ )
	{
		tessCtrlShaderDataPtr_->resetAllocCount();
	}
	if ( tessEvalShaderDataPtr_ )
	{
		tessEvalShaderDataPtr_->resetAllocCount();
	}
	if ( geometryShaderDataPtr_ )
	{
		geometryShaderDataPtr_->resetAllocCount();
	}
	if ( fragmentShaderDataPtr_ )
	{
		fragmentShaderDataPtr_->resetAllocCount();
	}
}

//==============================================================================
GEM_END_NAMESPACE
//==============================================================================