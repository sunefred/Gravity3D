//==============================================================================
//
// Description here
//
//==============================================================================


#ifndef GEM_RENDERSTAGE_H
#define GEM_RENDERSTAGE_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class RenderStage
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	RenderStage();

	// default copy constructor is
	~RenderStage();

	// default destructor ok


	//-- copy and clear --------------------------------------------------------

	// implement this if you have pointers
	void copy( ) {}

	// implement this if you have pointers
	void clear( ) {}


	//-- INPUT: mesh data ------------------------------------------------------

	void setMesh( MeshLoader* const _meshLoaderPtr );

	void setVertexIndexData( Allocator* const _allocatorPtr );

	void setVertexAttributeData( Allocator* const _allocatorPtr, 
									unsigned int _attr );


	//-- INPUT: texture data ---------------------------------------------------

	void setTexture( TextureLoader* const _textureLoaderPtr,
					 unsigned int _textureUnit );

	//void setTexture( Allocator* const _allocatorPtr,
	//				 unsigned int _textureUnit );


	//-- INPUT: shader program -------------------------------------------------

	void setVertexShader( ShaderLoader* const _vertexShaderLoaderPtr );

	void setTessCtrlShader( ShaderLoader* const _tessCtrlShaderLoaderPtr );

	void setTessEvalShader( ShaderLoader* const _tessEvalShaderLoaderPtr );

	void setGeometryShader( ShaderLoader* const _geometryShaderLoaderPtr );

	void setFragmentShader( ShaderLoader* const _fragmentShaderLoaderPtr );


	//-- INPUT: uniforms -------------------------------------------------------
	//
	//	glUniform(): http://www.opengl.org/sdk/docs/man4/xhtml/glUniform.xml
	//
	
	// Scalars
	void setUniform( int* const _intPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( unsigned int* const _uintPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( float* const _floatPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	// Vectors
	void setUniform( Vec2i* const _vec2iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec2ui* const _vec2uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec2f* const _vec2fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec3i* const _vec3iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec3ui* const _vec3uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec3f* const _vec3fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec4i* const _vec4iPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec4ui* const _vec4uiPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Vec4f* const _vec4fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	// Matrices
	void setUniform( Mat2f* const _mat42fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Mat3f* const _mat3fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );
	void setUniform( Mat4f* const _mat4fPtr,
					 const std::string& _name,
					 const unsigned int _count = 1 );


	//-- INPUT: texture buffer object ------------------------------------------



	//-- OUTOUT: transform feedback --------------------------------------------

	//-- OUTOUT: frame buffer object -------------------------------------------


	//-- the star function of the entire library -------------------------------

	void draw();


protected:


protected:


private:

private:

	//-- INPUT: mesh data ---------------------------------------------------------

	void declareVertexData();
	void undeclareVertexData();
	void uploadVertexData();
	void downloadVertexData();
	void bindVertexData();
	void unbindVertexData();	


	//-- INPUT: texture data ---------------------------------------------------------

	void declareTextureData();
	void undeclareTextureData();
	void uploadTextureData();
	void downloadTextureData();
	void bindTextureData();
	void unbindTextureData();	


	//-- INPUT: shader program ----------------------------------------------------

	void declareShaderProgram();
	void undeclareShaderProgram();
	void bindShaderProgram();
	void unbindShaderProgram();


	//-- INPUT UNIFORMS --------------------------------------------------------

	void declareUniforms();
	void undeclareUniforms();
	void bindUniforms();
	//void unbindUniform();


	//-- data change evaluators ------------------------------------------------

	bool isVertexDataAllocCount();
	bool isVertexDataWriteCount();
	bool isTextureDataAllocCount();
	bool isTextureDataWriteCount();
	bool isShaderProgramAllocCount();
	void resetVertexDataAllocCount();
	void resetVertexDataWriteCount();
	void resetTextureDataAllocCount();
	void resetTextureDataWriteCount();
	void resetShaderProgramAllocCount();


private:

	// Draw call flags
	GLenum drawMode_;
	bool hasTesselator_;
	GLuint vertexIndexDim_;
	GLuint vertexIndexCount_;
	GLenum vertexIndexType_;
	GLenum vertexAttributeDim_[MAX_VERTEX_ATTRIBUTES];
	GLenum vertexAttributeType_[MAX_VERTEX_ATTRIBUTES];
	

	// INPUT: Vertex Data
	Allocator* vertexIndexDataPtr_;
	Allocator* vertexAttributeDataPtr_[MAX_VERTEX_ATTRIBUTES];
	GLuint vertexArrayObjectID_;
	GLuint vertexIndexBufferID_;
	GLuint vertexAttributeBufferID_[MAX_VERTEX_ATTRIBUTES];
	bool isDeclaredVertexData_;


	// INPUT: TextureData
	Allocator* textureDataPtr_[MAX_TEXTURE_UNITS][MAX_MIP_LEVELS];
	unsigned int textureMipLevelCount_[MAX_TEXTURE_UNITS];
	unsigned int textureUnit_[MAX_TEXTURE_UNITS];
	GLuint	textureID_[MAX_TEXTURE_UNITS];
	GLenum	textureTarget_[MAX_TEXTURE_UNITS];
	GLint	textureInternalFormat_[MAX_TEXTURE_UNITS];
	GLenum	textureFormat_[MAX_TEXTURE_UNITS];
	GLenum	textureType_[MAX_TEXTURE_UNITS];
	GLuint samplerID_[MAX_TEXTURE_UNITS];
	bool isDeclaredTextureData_;


	// INPUT: Shader Program
	Allocator* vertexShaderDataPtr_;
	Allocator* tessCtrlShaderDataPtr_;
	Allocator* tessEvalShaderDataPtr_;
	Allocator* geometryShaderDataPtr_;
	Allocator* fragmentShaderDataPtr_;
	GLuint vertexShaderID_;
	GLuint tessCtrlShaderID_;
	GLuint tessEvalShaderID_;
	GLuint geometryShaderID_;
	GLuint fragmentShaderID_;
	GLuint shaderProgramID_;
	bool isDeclaredShaderProgram_;


	// INPUT: Uniforms
	std::vector<void*> uniformPtr_;
	std::vector<std::string> uniformName_;
	std::vector<ALLOC_FORMAT> uniformFormat_;
	std::vector<unsigned int> uniformCount_;
	std::vector<GLint> unformLocation_;
	bool isDeclaredUniforms_;


	// INPUT: Texture Buffer Object


	// 


};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================