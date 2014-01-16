//==============================================================================
//
// http://www.opengl.org/sdk/docs/man4/
// http://www.opengl.org/registry/doc/glspec42.core.20120427.withchanges.pdf
// http://www.opengl.org/registry/doc/glspec43.core.20130214.withchanges.pdf
//
// RenderState uses OpenGL buffers extensively, in fact wherever its possible
// resource data exists as a buffer object before being being bound to a 
// specific target. Shader sources and uniform data is the exception.
// There are two main  advantages for OpenGL buffers
//
//		1. The buffer represents a general blob of data that can be bound
//		   and rebound to many different targets through out its lifetime
//		2. Once data is uploaded to a buffer, copying data between buffers
//		   or to textures using glTexImage2D() is done without CPU 
//		   involvment. I.e. theese functions will return immediatly.
//
// However you should be aware that OpenGL makes optimizations based on the
// _initial_ binding point of any buffer. So you should not do an initial bind
// to a GL_PIXEL_PACK_BUFFER buffer if you intend to use the buffer as a 
// GL_ARRAY_BUFFER as that buffer would probably be placed in the texture
// memory instead of on die as vertex data.
//
// RenderState accomplishes this generalisation by creating a buffer object
// for each Allocator* passed to the RenderState through one of the setX()
// functions. The first invoked setX() for any Allocator* will determine it's
// initial target. So if you invoke setVertexIndexData() you will get a buffer
// with an initial binding to GL_ELEMENT_ARRAY_BUFFER. You can invoke another
// setY() involving the same Allocator* which will mean that the OpenGL buffer
// object will be bound to two targets during a single draw.
//
// RenderState does not know the concept of compound objects such as Meshes it
// only knows about Allocator* correlated binding points in the OpenGL state,
// and there are specific functions for each binding point. However  for
// convenience there are compund functions such as setMesh that will call the
// correct Allocator binding functions for you. Once that is done the 
// connection to the MeshLoader is lost.
//
// Not all binding points in the OpenGL can use the buffer objects directly
// as they might require to be translated or unpacked to a specific internal 
// format that is not supported by the client side buffer object. This is true
// for the texture and Framebuffer attachments specifically. There are specific
// unpack and pack functions for each type for this translation.
//
// Bellow is the full list of used gl* commands used by this OpenGL state
// implementation sorted by resource type. The y-axis represents the resource
// lifetime from birt (declare) to it's death (undeclare). It also specifies
// what triggers a specific set of commands, is it client side data change,
// buffer data change or is it run every frame?
//
//
//
//
//
//		BUFFER					VERTEX_DATA					SHADER
//			
// -----------------------------------------------------------------------------
//	|	DECLARE - create id's and allocate storage
//  d
//	a	glGenBuffer()			glGenVertexArrays()
//	t	glBindBuffer()			glBindVertexArray()
//	a	glBufferStorage(..,0)	glBindBuffer(idx)
//	|	glBindBuffer(0)			glBindBuffer(attr...)
//	a							glVertexAttribPointer()	
//	l							glEnableVertexAttribArray()	
//	l							glBindVertexArray(0)	
//	o
//	c
//	|
//------------------------------------------------------------------------------
//  |	UPLOAD - upload RAM ---> Buffer
//  d		
//	a	glBindBuffer()
//	t	glBufferStorage()
//	a	or glBufferData()
//	|	glBindBuffer(0)
//	c
//	h
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	UNPACK - Buffer ---> Pixel Storage
//	b	
//	u	
//	f	
//	|	
//	c	
//	h	
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	BIND
//	e
//	v							glBindVertexArray()
//	e	
//	r	
//	y
//	|	------------------------------------------------------------------------
//	f	DRAW
//	a
//	m							glDrawElements()
//	e
//	|
//	|	------------------------------------------------------------------------
//	|	UNBIND
//	|
//	|							glBindVertexArray()
//	|		
//	|		
//	|
//------------------------------------------------------------------------------
//	|	PACK - Pixel Storage ---> Buffer
//	o	
//	g
//	l
//	|		
//	w		
//  r		
//  t		
//	|
// -----------------------------------------------------------------------------
//	|	DOWNLOAD - Buffer ---> RAM
//	o
//	n			
//	|	
//	d	glBindBuffer(0)
//	e
//	m
//	a
//	n
//	d
//	|
// -----------------------------------------------------------------------------
//	|	UNDECLARE
//	|
//	|	glDetelBuffer(0)
//	|
//	|
//	|
// -----------------------------------------------------------------------------
//
//
//
//
//
//
//		UNIFORM					TEXTURE					BUFFER TEXTURE
//			
// -----------------------------------------------------------------------------
//	|	DECLARE - create id's and allocate storage
//  d
//	a	
//	t	
//	a	
//	|	
//	a	
//	l	
//	l	
//	o
//	c
//	|
//------------------------------------------------------------------------------
//  |	UPLOAD - upload RAM ---> Buffer
//  d		
//	a	
//	t	
//	a	
//	|	
//	c
//	h
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	UNPACK - Buffer ---> Pixel Storage
//	b	
//	u	
//	f	
//	|	
//	c	
//	h	
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	BIND
//	e
//	v	
//	e	
//	r	
//	y
//	|	------------------------------------------------------------------------
//	f	DRAW
//	a
//	m	
//	e
//	|
//	|	------------------------------------------------------------------------
//	|	UNBIND
//	|
//	|	
//	|		
//	|		
//	|
//------------------------------------------------------------------------------
//	|	PACK - Pixel Storage ---> Buffer
//	o	
//	g
//	l
//	|		
//	w		
//  r		
//  t		
//	|
// -----------------------------------------------------------------------------
//	|	DOWNLOAD - Buffer ---> RAM
//	o
//	n
//	|
//	d
//	e
//	m
//	a
//	n
//	d
//	|
// -----------------------------------------------------------------------------
//	|	UNDECLARE
//	|
//	|	
//	|
//	|
//	|
// ------------------------------------------------------------------------------
//
//
//
//
//
//
//		TRANSFORM FEEDBACK								FRAMEBUFFER
//			
// -----------------------------------------------------------------------------
//	|	DECLARE - create id's and allocate storage
//  d
//	a	/* varyings after compile, before link */
//	t	glTransformFeedbackVaryings()
//	a	glGenTransformFeedbacks(,i)
//	|	glBindBufferBase(,loc,buf)
//	a	glBindTransformFeedback(0)
//	l	GL_RASTERIZER_DISCARD
//	l	
//	o	
//	c
//	|
//------------------------------------------------------------------------------
//  |	UPLOAD - upload RAM ---> Buffer
//  d		
//	a	
//	t	
//	a	
//	|	
//	c
//	h
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	UNPACK - Buffer ---> Pixel Storage
//	b	
//	u	
//	f	
//	|	
//	c	
//	h	
//	n
//	g
//	|
//------------------------------------------------------------------------------
//	|	BIND
//	e
//	v	glBeginTransformFeedback()
//	e	
//	r	
//	y
//	|	------------------------------------------------------------------------
//	f	DRAW
//	a
//	m	glDrawTransformFeedback()
//	e	or glDrawArrays()
//	|	
//	|	
//	|
//	|	------------------------------------------------------------------------
//	|	UNBIND
//	|
//	|	glEndTransformFeedback()
//	|		
//	|		
//	|
//------------------------------------------------------------------------------
//	|	PACK - Pixel Storage ---> Buffer
//	o	
//	g
//	l
//	|		
//	w		
//  r		
//  t		
//	|
// -----------------------------------------------------------------------------
//	|	DOWNLOAD - Buffer ---> RAM
//	o
//	n
//	|
//	d
//	e
//	m
//	a
//	n
//	d
//	|
// -----------------------------------------------------------------------------
//	|	UNDECLARE
//	|
//	|	
//	|
//	|
//	|
//
//==============================================================================


#ifndef GEM_RENDERSTATE_H
#define GEM_RENDERSTATE_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemTracker.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== SHARED: BufferState =======================================================

class BufferState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	BufferState();

	// specialized constructors
	BufferState( Allocator* const _allocatorPtr );

	// destructor
	~BufferState();


	//-- copy and cleaar -------------------------------------------------------

	void clear();


	//-- sets and gets ---------------------------------------------------------

	GLuint getBufferID() const { return bufferID_; }
	GLuint getInitialTarget() const { return initialTarget_; }
	GLuint getInitialUsage() const { return initialUsage_; }

	void setAllocator( Allocator* const _allocatorPtr );
	const Allocator* getAllocatorPtr() const { return allocatorPtr_; }

	void setInitialTarget( const GLenum& _initialTarget )
	{ initialTarget_ = _initialTarget; }
	void setInitialUsage( const GLenum& _initialUsage )
	{ initialUsage_ = _initialUsage; }

	unsigned int getDeclareCount() const { return declareCount_; }
	const unsigned int* getDeclareCountPtr() const { return &declareCount_; }

	unsigned int getUploadCount() const { return uploadCount_; }
	const unsigned int* getUploadCountPtr() const { return &uploadCount_; }

	void increasePackCount() { packCount_++; }
	unsigned int getPackCount() const { return packCount_; }
	const unsigned int* getPackCountPtr() const { return &packCount_; }

	void increaseFeedbackCount() { feedbackCount_++; }
	unsigned int getFeedbackCount() const { return feedbackCount_; }
	const unsigned int* getFeedbackCountPtr() const { return &feedbackCount_; }

	bool isDeclare() const { return isDeclared_; };


	//-- RAM <--> Buffer functions ---------------------------------------------

	void declare();
	void undeclare();
	void upload();
	void download();


	//-- stream output ---------------------------------------------------------

	friend std::ostream& operator<< (std::ostream& lhs,
									 const BufferState& rhs)
	{
		// id, target, usage, format, height, width, bytecount
	}


private:

	// OpenGL variables
	GLuint bufferID_;
	GLenum initialTarget_;
	GLuint initialUsage_;

	// Allocator and trackers
	Allocator* allocatorPtr_;
	Trackerui trackerAllocCount_;
	Trackerui trackerWriteCount_;
	Trackerui trackerPackCount_;
	Trackerui trackerFeedbackCount_;
	
	// Counters and flags
	unsigned int declareCount_;
	unsigned int uploadCount_;
	unsigned int packCount_;
	unsigned int feedbackCount_;
	bool isDeclared_;
};


//== INPUT: VertexState ========================================================

class VertexState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	VertexState();

	// destructor
	~VertexState();

	//-- copy and clear --------------------------------------------------------

	void clear();

	
	//-- sets and gets ---------------------------------------------------------

	GLenum getDrawMode() const { return drawMode_; }
	GLuint getIndexDim() const { return indexDim_; }
	GLuint getIndexCount() const { return indexCount_; }
	GLenum getIndexType() const { return indexType_; }

	void setIndexBuffer( BufferState* const _bufferStatePtr );
	void setAttributeBuffer( BufferState* const _bufferStatePtr,
							 unsigned int _attrIndex );

	bool isDeclared() const { return isDeclared_; };


	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLuint arrayObjectID_;
	GLenum drawMode_;
	GLuint indexDim_;
	GLuint indexCount_;
	GLenum indexType_;
	GLenum attributeDim_[MAX_VERTEX_ATTRIBUTES];
	GLenum attributeType_[MAX_VERTEX_ATTRIBUTES];

	// BufferStates and trackers
	BufferState* indexBufferPtr_;
	Trackerui trackerIndexDeclareCount_;
	BufferState* attributeBufferPtr_[MAX_VERTEX_ATTRIBUTES];
	Trackerui trackerAttributeDeclareCount_[MAX_VERTEX_ATTRIBUTES];

	// Counters and flags
	bool isDeclared_;
};


//== INPUT: ProgramState =======================================================

class ProgramState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	ProgramState();

	// destructor
	~ProgramState();


	//-- copy and clear --------------------------------------------------------
	
	void clear();


	//-- sets and gets ---------------------------------------------------------

	void setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr );

	void setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr );

	void setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr );

	void setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr );

	void setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr );

	void setTransformFeedbackName( const std::string& _xbfName )
	{ transformFeedbackNames_.push_back( _xbfName ); }

	unsigned int getTransformFeedbackNameCount() const
	{ return transformFeedbackNames_.size(); }

	void enableTransformFeedbackInterleaved()
	{ isTransformFeedbackInterleaved_ = true; }

	void disableTransformFeedbackInterleaved()
	{ isTransformFeedbackInterleaved_ = false; }

	GLuint getProgramID() const { return programID_; }

	unsigned int getDeclareCount() const { return declareCount_; }

	const unsigned int* getDeclareCountPtr() const { return &declareCount_; }

	bool hasTesselator() const { return hasTesselator_; }


	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLuint vertexShaderID_;
	GLuint tessCtrlShaderID_;
	GLuint tessEvalShaderID_;
	GLuint geometryShaderID_;
	GLuint fragmentShaderID_;
	GLuint programID_;

	// Transform Feedback states
	std::vector<std::string> transformFeedbackNames_;
	bool isTransformFeedbackInterleaved_;

	// Allocators and trackers
	Allocator* vertexShaderDataPtr_;
	Allocator* tessCtrlShaderDataPtr_;
	Allocator* tessEvalShaderDataPtr_;
	Allocator* geometryShaderDataPtr_;
	Allocator* fragmentShaderDataPtr_;
	Trackerui trackerVertexShaderAllocCount_;
	Trackerui trackerTessCtrlShaderAllocCount_;
	Trackerui trackerTessEvalShaderAllocCount_;
	Trackerui trackerGeometryShaderAllocCount_;
	Trackerui trackerFragmentShaderAllocCount_;

	// Counters and flags
	bool hasTesselator_;
	unsigned int declareCount_;
	bool isDeclared_;
};


//== INPUT: UniformState =======================================================

class UniformState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	UniformState();

	// specialized constructors
	UniformState( const ProgramState* const _programStatePtr,
				  const void* const _ptr,
				  const std::string& _name,
				  const ALLOC_FORMAT& _allocFormat,
				  const unsigned int _count );

	// destructor
	~UniformState();


	//-- copy and clear --------------------------------------------------------
	
	void clear();


	//-- sets and gets ---------------------------------------------------------

	void setProgram( const ProgramState* const _programStatePtr );

	void setUniform( const void* const _ptr,
					 const std::string& _name,
					 const ALLOC_FORMAT& _allocFormat,
					 const unsigned int _count );


	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLint location_;

	// program to request for uniform variables
	const ProgramState* programStatePtr_;

	// Allocators type variables and trackers
	const void* ptr_;
	std::string name_;
	ALLOC_FORMAT allocFormat_;
	unsigned int count_;
	Trackerui trackerProgramDeclareCount_;

	// Counters and flags
	bool isDeclared_;
};


//== INPUT: TextureState =======================================================

class TextureState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	TextureState();

	// specialized constructors

	// destructor
	~TextureState();


	//-- copy and clear --------------------------------------------------------
	
	void clear();


	//-- sets and gets ---------------------------------------------------------

	void setMipMap( BufferState* const _bufferStatePtr,
					const unsigned int _mipLevel );

	void setTextureUnit( TEXTURE_UNIT _textureUnit )
	{ textureUnit_ = _textureUnit; }


	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void unpack();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLuint	textureID_;
	GLenum	target_;
	GLint	internalFormat_;
	GLenum	format_;
	GLenum	type_;
	GLuint samplerID_;

	// Allocators and trackers
	BufferState* bufferStatePtr_[MAX_MIP_LEVELS];
	Trackerui trackerBufferDeclareCount_[MAX_MIP_LEVELS];
	Trackerui trackerBufferUploadCount_[MAX_MIP_LEVELS];
	Trackerui trackerBufferPackCount_[MAX_MIP_LEVELS];
	TEXTURE_UNIT textureUnit_;
	unsigned int mipLevelCount_;

	// Counters and flags
	bool isDeclared_;

};


//== INPUT: BufferTextureState =================================================

class BufferTextureState
{
};


//== OUTPUT: TransformFeedbackState ============================================

class TransformFeedbackState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	TransformFeedbackState();

	// specialized constructors

	// destructor
	~TransformFeedbackState();


	//-- copy and clear --------------------------------------------------------
	
	void clear();


	//-- sets and gets ---------------------------------------------------------

	unsigned int getAttachedBufferCount() const;

	void setAttachment( BufferState* const _bufferStatePtr,
						unsigned int xfbIndex_ );


	//unsigned int getAttachmentCount( ) const
	//{ return }

	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLuint transformFeedbackID_;

	// BufferStates and trackers
	BufferState* bufferStatesPtr_[MAX_TRANSFORMFEEDBACK_ATTACHMENTS];
	Trackerui trackerBufferDeclareCount_[MAX_TRANSFORMFEEDBACK_ATTACHMENTS];

	// Counters and flags
	bool hasTransformFeedback_;
	bool isDeclared_;
};


//== OUTPUT: FramebufferState ==================================================

class FramebufferState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	FramebufferState();

	// specialized constructors

	// destructor
	~FramebufferState();


	//-- copy and clear --------------------------------------------------------
	
	void clear();


	//-- sets and gets ---------------------------------------------------------

	void setAttachment( BufferState* const _bufferStatePtr,
						FRAMEBUFFER_ATTACHMENT _framebufferAttachment );


	//-- Buffer <--> OpenGL functions ------------------------------------------

	void declare();
	void undeclare();
	void pack();
	void bind();
	void unbind();


private:

	// OpenGL variables
	GLuint framebufferID_;
	GLenum target_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLint  internalFormat_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLenum format_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLenum type_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLenum attachment_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLuint renderbufferID_[MAX_FRAMEBUFFER_ATTACHMENTS];
	GLenum drawBuffers_[MAX_FRAMEBUFFER_ATTACHMENTS];

	// BufferStates and trackers
	BufferState* bufferStatesPtr_[MAX_FRAMEBUFFER_ATTACHMENTS];
	Trackerui trackerBufferDeclareCount_[MAX_FRAMEBUFFER_ATTACHMENTS];

	// Counters and flags
	unsigned int drawBuffersCount_;
	bool isDeclared_;
};


//== MAIN CLASS: RenderState ===================================================

class RenderState
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	RenderState();

	// default copy constructor is
	~RenderState();

	// default destructor ok


	//-- copy and clear --------------------------------------------------------

	// implement this if you have pointers
	void copy( ) {}

	// implement this if you have pointers
	void clear();


	//-- DRAW ------------------------------------------------------------------

	void draw();


	//-- OPENGL FLAGS & VARS ---------------------------------------------------

	// clear color, depth, stencil
	void setClear( const bool _isClearColor = false,
				   const Vec4f& _clearColor = Vec4f::ZERO,
				   const bool _isClearDepth = false,
				   const float _clearDepth = 1.0f,
				   const bool _isClearStencil = false,
				   const int _clearStencil = 0 );

	// culling
	void setCulling( const bool _isCullFace = false,
					 const CULL_FACE& _cullFace = CULL_FACE_BACK );

	// depth test
	void setDepthTest( const bool _isDepthTest = false,
					   const bool _isDepthWrite = false,
					   const DEPTH_FUNC& _depthFunc = DEPTH_FUNC_LESS );
	
	//polygon mode
	void setPolygonMode( const POLYGON_MODE& _polygonMode = POLYGON_MODE_FILL );

	// viewport size
	void setViewport( const unsigned int _viewportWidth, 
					  const unsigned int _viewportHeight );
	void setViewportPtr( const unsigned int* _viewportWidthPtr, 
						 const unsigned int* _viewportHeightPtr );


	//-- INPUT: mesh data ------------------------------------------------------

	void setMesh( MeshLoader* const _meshLoaderPtr );

	void setVertexIndexData( Allocator* const _allocatorPtr );

	void setVertexAttributeData( Allocator* const _allocatorPtr, 
									unsigned int _attrIndex );


	//-- INPUT: shader program -------------------------------------------------

	void setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr );

	void setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr );

	void setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr );

	void setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr );

	void setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr );


	//-- INPUT: uniforms -------------------------------------------------------
	//
	//	glUniform(): http://www.openorg/sdk/docs/man4/xhtml/glUniform.xml
	//
	
	// Allocator
	void setUniform( const Allocator* const _allocatorPtr,
					 const std::string& _name );

	// Scalars
	void setUniform( const int* const _intPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const unsigned int* const _uintPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const float* const _floatPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	// Vectors
	void setUniform( const Vec2i* const _vec2iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec2ui* const _vec2uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec2f* const _vec2fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec3i* const _vec3iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec3ui* const _vec3uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec3f* const _vec3fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec4i* const _vec4iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec4ui* const _vec4uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Vec4f* const _vec4fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	// Matrices
	void setUniform( const Mat2f* const _mat42fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Mat3f* const _mat3fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( const Mat4f* const _mat4fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );


	//-- INPUT: texture data ---------------------------------------------------

	void setTexture( TextureLoader* const _textureLoaderPtr,
					 TEXTURE_UNIT _textureUnit );

	void setTexture( Allocator* const _allocatorPtr,
					 TEXTURE_UNIT _textureUnit, unsigned int _mipLevel = 0 );


	//-- INPUT: buffer texture -------------------------------------------------

	void setBufferTexture( TextureLoader* const _textureLoaderPtr,
						   TEXTURE_UNIT _textureUnit );

	//void setBufferTexture( Allocator* const _allocatorPtr,
	//					   unsigned int _textureUnit, unsigned int _mipLevel = 0 );


	//-- OUTOUT: transform feedback --------------------------------------------

	void setTransformFeedback( MeshLoader* const _meshLoaderPtr );

	void setTransformFeedback( Allocator* const _allocatorPtr,
							   const std::string& _xbfName0,
							   const std::string& _xbfName1 = "",
							   const std::string& _xbfName2 = "",
							   const std::string& _xbfName3 = "",
							   const std::string& _xbfName4 = "",
							   const std::string& _xbfName5 = "",
							   const std::string& _xbfName6 = "",
							   const std::string& _xbfName7 = "" );

	//-- OUTOUT: frame buffer object -------------------------------------------

	void setFramebuffer( TextureLoader* const _textureLoaderPtr,
						 FRAMEBUFFER_ATTACHMENT _framebufferAttachment );

	void setFramebuffer( Allocator* const _allocatorPtr,
						 FRAMEBUFFER_ATTACHMENT _framebufferAttachment );


protected:


protected:


private:


private:


	//-- SHARED: buffer objects ------------------------------------------------

	void declareBuffers();
	void undeclareBuffers();
	void uploadBuffers();
public:
	void downloadBuffers();
private:


	//-- INPUT: mesh data ------------------------------------------------------

	void declareVertexData();
	void undeclareVertexData();
	void uploadVertexData();
	void downloadVertexData();
	void bindVertexData();
	void unbindVertexData();	


	//-- INPUT: shader program -------------------------------------------------

	void declareShaderProgram();
	void undeclareShaderProgram();
	void bindShaderProgram();
	void unbindShaderProgram();


	//-- INPUT: uniforms --------------------------------------------------------

	void declareUniforms();
	void undeclareUniforms();
	void bindUniforms();


	//-- INPUT: texture data ---------------------------------------------------

	void declareTextures();
	void undeclareTextures();
	void unpackTextures();
	void bindTextures();
	void unbindTextures();


	//-- INPUT: buffer texture -------------------------------------------------

	void declareBufferTexture();
	void undeclareBufferTexture();
	void bindBufferTexture();
	void unbindBufferTexture();


	//-- OUTOUT: transform feedback --------------------------------------------

	void declareTransformFeedback();
	void undeclareTransformFeedback();
	void bindTransformFeedback();
	void unbindTransformFeedback();


	//-- OUTOUT: framebuffer object --------------------------------------------

	void declareFramebuffer();
	void undeclareFramebuffer();
	void packFramebuffer();
	void bindFramebuffer();
	void unbindFramebuffer();

private:

	// OPENGL FLAGS & VARS
	// -------------------
	// clear color, depth, stencil
	bool isClearDepth_;
	bool isClearColor_;
	bool isClearStencil_;
	Vec4f clearColor_;
	float clearDepth_;
	int clearStencil_;
	// culling
	bool isCullFace_;
	CULL_FACE cullFace_;
	// depth test
	bool isDepthTest_;
	bool isDepthWrite_;
	DEPTH_FUNC depthFunc_;
	// polygone mode
	POLYGON_MODE polygonMode_;
	// viewport size
	unsigned int viewportWidth_;
	unsigned int viewportHeight_;
	const unsigned int* viewportWidthPtr_;
	const unsigned int* viewportHeightPtr_;


	// SHARED: Buffer Object
	static std::map<const Allocator*,BufferState> bufferStates_;


	// INPUT: Vertex Data
	VertexState vertexState_;

	
	// INPUT: Shader Program
	ProgramState programState_;


	// INPUT: Uniforms
	std::vector<UniformState> uniformStates_;

	
	// INPUT: TextureData
	std::map<TEXTURE_UNIT,TextureState> textureStates_;


	// INPUT: Texture Buffer Object



	// OUTOUT: Transform Feedback
	TransformFeedbackState transformFeedbackState_;



	// OUTOUT: Frame Buffer Object
	FramebufferState framebufferState_;


};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================