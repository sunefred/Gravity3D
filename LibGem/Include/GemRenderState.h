//==============================================================================
//
// http://www.opengl.org/sdk/docs/man4/
// http://www.opengl.org/registry/doc/glspec42.core.20120427.withchanges.pdf
// http://www.opengl.org/registry/doc/glspec43.core.20130214.withchanges.pdf
//
// RenderState uses OpenGL buffers extensively, in fact no data set to this
// class exists without being bound to a buffer object. There are two main 
// advantages for OpenGL buffers
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
// with an initial binding to GL_ELEMENT_ARRAY_BUFFER. You can invoke a setX()
// involving the same Allocator* which will mean that the OpenGL buffer object
// will be bound and unbound to two targets during a single draw.
//
// RenderState does not know the concept of compund objects such as Meshes it
// only knows about Allocator* correlated binding points in the OpenGL state,
// and there are specific functions for each binding point. However  for
// conveniece there are compund functions that setMesh that will call the
// correct Allocator binding functions for you. Once that is done the 
// connection to the Loader is lost.
//
// Downsides of this generalisation.
//
// Not all objects in the OpenGL state are buffer objects. Textures and 
// Framebuffer attachments specifically are their own data types. For textures
// it is possible to use glTexImage to "copy" data from a buffer to a texture.
// Does this really make a copy of the data?. For 
//
//
//		TEXTURE					FRAMEBUFFER					VERTEXDATA				TRANSFORM FEEDBACK
//			
// ----------------------------------------------------------------------------------------------
//	|	DECLARE - create id's and allocate storage
//	n		
//	e	glGenBuffer()			same						same
//	w	glBindBuffer()			same						same
//  |	glBufferStorage()		same						same
//	a	glBindBuffer(0)			same						same
//  l
//	l	-----------------------------------------------------------------------------------------
//	o	DECLARE - create id's and allocate storage
//  c
//	|	glGenTextures()			glGenFramebuffers()			glGenVertexArrays()
//	|	glTexImage2D(..., 0)	glBindFramebuffer()			glBindVertexArray()
//	|							glDrawBuffers()				glBindBuffer()
//	|							glGenRenderbuffers()		glBindVertexArray(0)	
//	|							glRenderbufferStorage()	
//	|							glFramebufferRenderbuffer()	
//	|
//----------------------------------------------------------------------------------------------
//  |	UPLOAD - upload RMA ---> buffer
//  a		
//	l	glBindBuffer()			same						same
//	c	glBufferStorage()		same						same
//	|	or glBufferData()
//	c	glBindBuffer(0)			same						same
//	h
//	n
//	g
//	|
//----------------------------------------------------------------------------------------------
//	|	UNPACK - upload buffer ---> texture/etc
//	b	
//	u	glBindBuffer()			glBindBuffer()			
//	f	glBindTexture()			glWritePixels(..., 0)			
//	|	glTexImage2D(..., 0)	glBindBuffer(0)			
//	c	glBindTexture(0)									
//	h	glBindBuffer(0)								
//	n
//	g
//	|
//-----------------------------------------------------------------------------------------------
//	|	BIND
//	e
//	v	glActiveTexture()		glBindFramebuffer()
//	e	glBindTexture()			
//	r	glBindSampler()
//	y
//	|	-----------------------------------------------------------------------------------------
//	f	
//	r	DRAW												glDrawElements()
//	a
//	m	-----------------------------------------------------------------------------------------
//	e	UNBIND
//	|
//	|	glActiveTexture(0)		glBindFramebuffer(0)	
//	|	glBindTexture(0)
//	|	glBindSampler(0)
//	|
//------------------------------------------------------------------------------------------------
//	|	PACK - download framebuffer/feedback/etc ---> buffer
//	o	
//	n	glBindBuffer()			glBindBuffer()
//	|	glBindTexture()			glBindFramebuffer()
//	w	glGetTexImage2D(..., 0)	glReadPixels(..., 0)
//  r	glBindBuffer(0)			glBindFramebuffer(0)
//  t	glBindTexture(0)		glBindBuffer(0)
//	|
// ------------------------------------------------------------------------------------------------
//	|	DOWNLOAD - dowbload buffer ---> RAM
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
// ----------------------------------------------------------------------------------------------
//	|	UNDECLARE
//	|
//	|	glBindBuffer()			same					same
//	|	glGetBufferSubData()	same					same
//	|	glBindBuffer(0)			same					same
//	|
//	|	-----------------------------------------------------------------------------------------
//	|	UNDECLARE:
//	|	
//	|	glDetelBuffer(0)		same					same
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

	bool isDeclare() const { return isDeclared_; };


	//-- RAM <--> Buffer functions ---------------------------------------------

	void declare();
	void undeclare();
	void upload();
	void download();


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
	
	// Counters and flags
	unsigned int declareCount_;
	unsigned int uploadCount_;
	unsigned int packCount_;
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
							 unsigned int _attrID );

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

	void setTextureUnit( unsigned int _textureUnit )
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
	GLenum  textureUnit_;
	GLuint samplerID_;

	// Allocators and trackers
	BufferState* bufferStatePtr_[MAX_MIP_LEVELS];
	Trackerui trackerBufferDeclareCount_[MAX_MIP_LEVELS];
	Trackerui trackerBufferUploadCount_[MAX_MIP_LEVELS];
	Trackerui trackerBufferPackCount_[MAX_MIP_LEVELS];
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
	void clear( ) {}


	//-- DRAW ------------------------------------------------------------------

	void draw();


	//-- INPUT: mesh data ------------------------------------------------------

	void setMesh( MeshLoader* const _meshLoaderPtr );

	void setVertexIndexData( Allocator* const _allocatorPtr );

	void setVertexAttributeData( Allocator* const _allocatorPtr, 
									unsigned int _attr );


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
					 unsigned int _textureUnit );

	void setTexture( Allocator* const _allocatorPtr,
					 unsigned int _textureUnit, unsigned int _mipLevel = 0 );


	//-- INPUT: buffer texture -------------------------------------------------

	void setBufferTexture( )
	{

	}


	//-- OUTOUT: transform feedback --------------------------------------------

	void setTransformFeedback( Allocator* const _allocatorPtr,
							   const std::string& _name )
	{

	}


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

	// SHARED: Buffer Object
	std::map<const Allocator*,BufferState> bufferStates_;


	// INPUT: Vertex Data
	VertexState vertexState_;

	
	// INPUT: Shader Program
	ProgramState programState_;


	// INPUT: Uniforms
	std::vector<UniformState> uniformStates_;

	
	// INPUT: TextureData
	std::map<unsigned int,TextureState> textureStates_;


	//-- INPUT: Texture Buffer Object ------------------------------------------



	//-- OUTOUT: Transform Feedback --------------------------------------------


	//-- OUTOUT: Frame Buffer Object -------------------------------------------
	FramebufferState framebufferState_;

	//Allocator* attachmentDataPtr_[1];
	//GLuint attachmentBufferID_[1];
	//GLuint attachmentTarget_[1];
	//GLuint attachmentInternalFormat_[1];
	//GLuint attachmentFormat_[1];
	//GLuint attachmentType_[1];
	//GLuint frameBufferObjectID_;
	//GLuint attachmentRenderBufferID_[1];



};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================