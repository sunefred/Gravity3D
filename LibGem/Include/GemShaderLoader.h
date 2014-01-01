//==============================================================================
//
//  Creates and holds shader data, i.e. the source code. The functionality of
//	this class is quite small and could possibly be combined with declare, but
//	is provided for symmetry reasons (with Mesh and Texture stores)
//
//==============================================================================

#ifndef GEM_SHADERLOADER_H
#define GEM_SHADERLOADER_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemLoader.h"
#include "GemAllocator.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION ===========================================================

class ShaderLoader : public Loader
{

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	ShaderLoader();

	// default copy constructor is ok

	// destructor
	~ShaderLoader();

	// default assignment operator is ok


	//-- copy and clear --------------------------------------------------------

	// implement this if you have pointers
	void copy( const ShaderLoader& other );

	// implement this if you have pointers
	void clear( );


public:

	//-- gets and sets ---------------------------------------------------------

	Allocator* const getSourcePtr()
	{ return &source_; }

	//std::string* const getSourcePtr()
	//{ return &source_; }

	bool isLoaded() const
	{ return isLoaded_; }

	bool isUpdated() const
	{ return isUpdated_; }

	void enableUpdated()
	{ isUpdated_ = true; }

	void disableUpdated()
	{ isUpdated_ = false; }


public:

	//-- load and create -------------------------------------------------------

	void load( const std::string& _path,
			   const SHADER_TYPE _shaderType = SHADER_TYPE_NONE );

	void reload();

protected:


protected:


private:

	//-- private file-io -------------------------------------------------------

	void loadShader( const std::string& _path );

private:

	// file information
	std::string path_;
	SHADER_TYPE shaderType_;

	// shader data stores
	Allocator source_;
	//std::string source_;

	// flags
	bool isLoaded_;
	bool isUpdated_;
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================