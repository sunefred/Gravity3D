//==============================================================================
//
//	Creates and holds mesh data, i.e index arrays and vertex attribute arrays.
//	You can create a mesh with a given index and attribute composition or load
//	it from file and let the file specify the composition.
//
//==============================================================================


#ifndef GEM_MESHLOADER_H
#define GEM_MESHLOADER_H


//== INCLUDES ==================================================================


#include "GemPrerequisites.h"
#include "GemAllocator.h"
#include "GemLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== MESHDATA CLASS DEFINITION =================================================

class MeshLoader : public Loader
{
public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor
	MeshLoader( );

	// default copy constructor is ok

	// default destructor
	~MeshLoader( );

	
	//-- copy and clear --------------------------------------------------------
	
	void clear( );

	void copy( const MeshLoader& other );


public:

	//-- sets and gets ---------------------------------------------------------

	unsigned int getPrimitivesCount() const
	{ return primitivesCount_; }

	Allocator* getPrimitivesPtr()
	{ return &primitives_; }

	unsigned int getVertexCount( ) const
	{ return vertexCount_; }

	Allocator* getVertexAttributesPtr( const unsigned int _attrID )
	{ return &vertexAttributes_[_attrID]; }

	bool isLoaded() const
	{ return isLoaded_; }


public:

	//-- load and create -------------------------------------------------------

	void load( const std::string& _path,
			   const FILE_FORMAT _fileFormat = FILE_FORMAT_NONE );

	void reload();

	void create( const unsigned int _primitivesCount,
				 const unsigned int _vertexCount,
				 const PRIM_TYPE _primType = PRIM_TYPE_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr0 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr1 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr2 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr3 = VERTEX_FORMAT_NONE, 
				 const VERTEX_FORMAT _vertexFormatAttr4 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr5 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr6 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr7 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr8 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr9 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr10 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr11 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr12 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr13 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr14 = VERTEX_FORMAT_NONE,
				 const VERTEX_FORMAT _vertexFormatAttr15 = VERTEX_FORMAT_NONE );

	void save( const std::string& _path,
			   const FILE_FORMAT _fileFormat = FILE_FORMAT_NONE );

protected:


private:


	//-- load and create ------------------------------------------------------

	void createPrimitives( const PRIM_TYPE _primType,
						   const unsigned int _primitivesCount );

	void createVertexAttribute( const unsigned int _attr,
								const VERTEX_FORMAT _vertexFormat,
								const unsigned int _vertexCount );


	//-- private file-io -------------------------------------------------------

	void loadOBX( const std::string& _path );

	void saveOBJ(  const std::string& _path );

	void saveOBX(  const std::string& _path );

private:

	// file information
	std::string	 path_;
	FILE_FORMAT fileFormat_;

	// Primitives data store
	unsigned int primitivesCount_;
	Allocator primitives_;

	// Vertex attributes data store
	unsigned int vertexCount_;
	Allocator vertexAttributes_[MAX_VERTEX_ATTRIBUTES];

	// status flags
	bool isLoaded_;

};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================