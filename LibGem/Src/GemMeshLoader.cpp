//== INCLUDES ==================================================================

#include "GemMeshLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//=== IMPLEMENTATION ===========================================================

//-- constructors/destructor ---------------------------------------------------

MeshLoader::MeshLoader( )
: path_( "" )
, fileFormat_( FILE_FORMAT_NONE )
, primitivesCount_( 0 )
, primitives_()
, vertexCount_( 0 )
, isLoaded_( false )
{
}


MeshLoader::~MeshLoader( )
{
	clear( );
}

//-- copy and clear ------------------------------------------------------------

void
MeshLoader::clear( )
{
	primitivesCount_ = 0;
	primitives_.clear();
	vertexCount_ = 0;
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		vertexAttributes_[i].clear();
	}
	isLoaded_ = false;
}

void
MeshLoader::copy( const MeshLoader& other )
{
	// no deep copy needed, allocators have deep copy implemented
	(*this) = other;
}

//-- load and create -------------------------------------------------------

void
MeshLoader::load( const std::string& _path, const FILE_FORMAT _fileFormat )
{
	// argument checks
	if ( _path.empty() )
		GEM_ERROR( "Path argument is empty." );


	// always create new
	if ( isLoaded_ )
		clear();


	// local variables
	FILE_FORMAT fileFormat = _fileFormat;
	std::string path = _path;


	// split path
	std::string dir, name, ext;
	splitPath( path, &dir, &name, &ext );


	// Tell the user that we are trying to load the file
	GEM_CONSOLE( "Loading mesh " + name + (ext.empty() ? "" : ".") + ext );


	// Determine file format from extension
	if ( fileFormat == FILE_FORMAT_NONE )
	{
		if ( ext == "obj" )
		{
			fileFormat = FILE_FORMAT_OBJ;
		}
		else if ( ext == "obx" )
		{
			fileFormat = FILE_FORMAT_OBX;
		}
	}


	// activate the right file loader depending on file type
	try
	{
		switch ( fileFormat )
		{
		case FILE_FORMAT_OBJ:
			loadOBX( path );
			break;
		case FILE_FORMAT_OBX:
			loadOBX( path );
			break;
		default:
			GEM_THROW( "Unsupported file type ." + ext );
			break;
		}
	}
	catch( const std::exception& e )
	{
		clear();
		GEM_ERROR( e.what() );
	}


	// we got this far, so we are ok to setup member variables
	path_ = path;
	fileFormat_ = fileFormat;
	isLoaded_ = true;
}

void
MeshLoader::reload()
{
	if ( isLoaded_ )
	{
		// clear before reloading
		clear();

		// and load
		load( path_, fileFormat_ );
	}
}

void
MeshLoader::create( const unsigned int _primitivesCount,
				     const unsigned int _vertexCount,
					 const PRIM_TYPE _primType,
					 const VERTEX_FORMAT _vertexFormatAttr0,
					 const VERTEX_FORMAT _vertexFormatAttr1,
					 const VERTEX_FORMAT _vertexFormatAttr2,
					 const VERTEX_FORMAT _vertexFormatAttr3,
					 const VERTEX_FORMAT _vertexFormatAttr4,
					 const VERTEX_FORMAT _vertexFormatAttr5,
					 const VERTEX_FORMAT _vertexFormatAttr6,
					 const VERTEX_FORMAT _vertexFormatAttr7,
					 const VERTEX_FORMAT _vertexFormatAttr8,
					 const VERTEX_FORMAT _vertexFormatAttr9,
					 const VERTEX_FORMAT _vertexFormatAttr10,
					 const VERTEX_FORMAT _vertexFormatAttr11,
					 const VERTEX_FORMAT _vertexFormatAttr12,
					 const VERTEX_FORMAT _vertexFormatAttr13,
					 const VERTEX_FORMAT _vertexFormatAttr14,
					 const VERTEX_FORMAT _vertexFormatAttr15 )
{
	// argument checks
	if ( _primitivesCount > 0 )
		GEM_ERROR( "Invalid number of primitives " + _primitivesCount );
	if ( _vertexCount > 0 )
		GEM_ERROR( "Invalid number of vertices " + _vertexCount );
	if ( _primType == PRIM_TYPE_NONE )
		GEM_ERROR( "Invalid primitive type " + PRIM_TYPE_NONE );


	// always create new
	if ( isLoaded_ )
		clear( );


	// save to member variables
	primitivesCount_ = _primitivesCount;
	vertexCount_ = _vertexCount;


	// allocate memory for primitives
	try
	{
		if ( _primType != PRIM_TYPE_NONE )
			createPrimitives( _primType, _primitivesCount );
	}
	catch( const std::exception &e )
	{
		clear();
		GEM_ERROR( e.what() );
	}


	// allocate memory for vertex attributes
	try
	{
		if ( _vertexFormatAttr0 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 0, _vertexFormatAttr0, _vertexCount );
		if ( _vertexFormatAttr1 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 1, _vertexFormatAttr1, _vertexCount );
		if ( _vertexFormatAttr2 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 2, _vertexFormatAttr2, _vertexCount );
		if ( _vertexFormatAttr3 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 3, _vertexFormatAttr3, _vertexCount );
		if ( _vertexFormatAttr4 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 4, _vertexFormatAttr4, _vertexCount );
		if ( _vertexFormatAttr5 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 5, _vertexFormatAttr5, _vertexCount );
		if ( _vertexFormatAttr6 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 6, _vertexFormatAttr6, _vertexCount );
		if ( _vertexFormatAttr7 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 7, _vertexFormatAttr7, _vertexCount );
		if ( _vertexFormatAttr8 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 8, _vertexFormatAttr8, _vertexCount );
		if ( _vertexFormatAttr9 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 9, _vertexFormatAttr9, _vertexCount );
		if ( _vertexFormatAttr10 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 10, _vertexFormatAttr10, _vertexCount );
		if ( _vertexFormatAttr11 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 11, _vertexFormatAttr11, _vertexCount );
		if ( _vertexFormatAttr12 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 12, _vertexFormatAttr12, _vertexCount );
		if ( _vertexFormatAttr13 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 13, _vertexFormatAttr13, _vertexCount );
		if ( _vertexFormatAttr14 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 14, _vertexFormatAttr14, _vertexCount );
		if ( _vertexFormatAttr15 != VERTEX_FORMAT_NONE )
			createVertexAttribute( 15, _vertexFormatAttr15, _vertexCount );
	}
	catch( const std::exception &e )
	{
		clear();
		GEM_ERROR( e.what() );
	}


	// we got this far, so we are ok to setup member variables
	isLoaded_ = true;
}

void
MeshLoader::save( const std::string& _path, const FILE_FORMAT _fileFormat )
{
	// argument checks
	if ( _path.empty() )
		GEM_ERROR( "Path argument is empty." );


	// cant save empty file
	if ( !isLoaded_ )
		return;


	// local variables
	FILE_FORMAT fileType = _fileFormat;
	std::string path = _path;


	// split path
	std::string dir, name, ext;
	splitPath( path, &dir, &name, &ext );


	// Tell the user that we are trying to save the file
	GEM_CONSOLE( "Saving mesh " + name + (ext.empty() ? "" : ".") + ext );


	// First determine file-type
	if ( fileType == FILE_FORMAT_NONE )
	{
		if ( ext == "obj" )
		{
			fileType = FILE_FORMAT_OBJ;
		}
		else if ( ext == "obx" )
		{
			fileType = FILE_FORMAT_OBX;
		}
	}


	// activate the right file loader depending on file type
	try
	{
		switch ( fileType )
		{
		case FILE_FORMAT_OBJ:
			saveOBJ( path );
			break;
		case FILE_FORMAT_OBX:
			saveOBX( path );
			break;
		default:
			GEM_THROW( "Unsupported file type ." + ext );
		}
	}
	catch( const std::exception& e )
	{
		clear();
		GEM_ERROR( e.what() );
	}
}


//-- private load and create ---------------------------------------------------

void
MeshLoader::createPrimitives( const PRIM_TYPE _primType,
							  const unsigned int _primitivesCount )
{
	// pre allocation checks
	if ( !_primitivesCount )
		GEM_THROW( "Invalid number of primitives" + _primitivesCount );
	if ( _primType == PRIM_TYPE_NONE )
		GEM_THROW( "Invalid primType" + _primType );


	// allocate storage for index array
	primitives_.alloc( static_cast<ALLOC_FORMAT>(_primType), _primitivesCount );


	// set member variables
	primitivesCount_ = _primitivesCount;
}

void
MeshLoader::createVertexAttribute( const unsigned int _attr,
								   const VERTEX_FORMAT _vertexFormat,
								   const unsigned int _vertexCount)
{
	// pre allocation checks
	if ( !_vertexCount )
		GEM_THROW( "Invalid number of vertices" + _vertexCount );
	if ( _vertexFormat == VERTEX_FORMAT_NONE )
		GEM_THROW( "Invalid vertexType" + _vertexFormat );
	if ( _attr >= MAX_VERTEX_ATTRIBUTES )
		GEM_THROW( "Attribute index out of range" + _attr );


	// allocate storage for vertex array
	vertexAttributes_[_attr].alloc( static_cast<ALLOC_FORMAT>(_vertexFormat),
									vertexCount_ );


	// set member variables
	vertexCount_ = _vertexCount;
}


//-- private file-io -----------------------------------------------------------

void
MeshLoader::loadOBX( const std::string& _path )
{
	// local variables
	std::string line;
	std::istringstream iss;
	std::string cmd;
	unsigned int n = 0;
	unsigned int primCount = 0;
	unsigned int vertexCount = 0;
	PRIM_TYPE primType = PRIM_TYPE_NONE;
	VERTEX_FORMAT vertexFormat = VERTEX_FORMAT_NONE;


	// open file for reading
	std::ifstream ifs( _path.c_str( ) );
	if ( ifs.fail() )
	{
		GEM_THROW( "Could not open ifs " + _path );
	}


	// run through file once to count vertices and faces
	while ( std::getline( ifs, line ) )
	{
		// clear io-state and set new string
		cmd.clear( );
		iss.clear( );
		iss.str( line );
		iss >> cmd;


		// increase vertex and face counters
		if ( cmd == "f" )
		{
			if ( !primCount )
			{
				for ( n = 0; iss >> cmd; ++n );
				switch ( n )
				{
				case 1: primType = PRIM_TYPE_POINT; break;
				case 2: primType = PRIM_TYPE_LINE; break;
				case 3: primType = PRIM_TYPE_TRIANGLE; break;
				case 4: primType = PRIM_TYPE_QUAD; break;
				}
			}
			++primCount;
		}
		else if ( cmd == "v" || cmd == "v0f" )
		{
			if ( !vertexCount )
			{
				for ( n = 0; iss >> cmd; ++n );
				switch ( n )
				{
				case 1: vertexFormat = VERTEX_FORMAT_X_32F; break;
				case 2: vertexFormat = VERTEX_FORMAT_XY_32F; break;
				case 3: vertexFormat = VERTEX_FORMAT_XYZ_32F; break;
				case 4: vertexFormat = VERTEX_FORMAT_XYZW_32F; break;
				}
			}
			++vertexCount;
		}
		else if ( cmd == "v0i" )
		{
			if ( !vertexCount )
			{
				for ( n = 0; iss >> cmd; ++n );
				switch ( n )
				{
				case 1: vertexFormat = VERTEX_FORMAT_X_32UI; break;
				case 2: vertexFormat = VERTEX_FORMAT_XY_32UI; break;
				case 3: vertexFormat = VERTEX_FORMAT_XYZ_32UI; break;
				case 4: vertexFormat = VERTEX_FORMAT_XYZW_32UI; break;
				}
			}
			++vertexCount;
		}
	}


	// we came this far without exception, so we can save member vars
	primitivesCount_ = primCount;
	vertexCount_ = vertexCount;


	// allocate memory for primitives (throws)
	createPrimitives( primType, primCount );


	// clear io-state and reset file pointer
	ifs.clear( );
	ifs.seekg( 0 );


	// run through file again adding attributes
	while ( std::getline( ifs, line ) )
	{
		// clear io-state and get new command
		cmd.clear( );
		iss.clear( );
		iss.str( line );
		iss >> cmd;


		// load primitives
		if ( cmd == "f" )
		{
			// get indices from file
			unsigned int c[4] = { 0, 0, 0, 0 };
			for ( n = 0; iss >> c[n] && n<4; ++n )
				iss.ignore( 100, ' ' );

			// add indices to buffer
			for ( unsigned int i = 0; i<n; ++i )
				primitives_.write<unsigned int>( c[i] - 1 );
		}


		// load vertex attributes
		else
		{
			unsigned int attr;
			VERTEX_FORMAT floatFormat = VERTEX_FORMAT_NONE;
			VERTEX_FORMAT intFormat = VERTEX_FORMAT_NONE;
			VERTEX_FORMAT vertexFormat = VERTEX_FORMAT_NONE;


			// get coordinates from file
			float c[4] = { 0, 0, 0, 0 };
			for ( n = 0; iss >> c[n] && n<4; ++n );


			// set temporary formats based on number of vertex elements
			if ( n == 1 ) { intFormat = VERTEX_FORMAT_X_32UI;
							floatFormat = VERTEX_FORMAT_X_32F; }
			else if ( n == 2 ) { intFormat = VERTEX_FORMAT_XY_32UI;
								 floatFormat = VERTEX_FORMAT_XY_32F; }
			else if ( n == 3 ) { intFormat = VERTEX_FORMAT_XYZ_32UI;
								 floatFormat = VERTEX_FORMAT_XYZ_32F; }
			else if ( n == 4 ) { intFormat = VERTEX_FORMAT_XYZW_32UI;
								 floatFormat = VERTEX_FORMAT_XYZW_32F; }


			// set attribute number and vertex format
			if ( cmd == "v" ) { attr = 0;  vertexFormat = floatFormat; }
			else if ( cmd == "vn" ) { attr = 2;  vertexFormat = floatFormat; }
			else if ( cmd == "vt" ) { attr = 8;  vertexFormat = floatFormat; }
			else if ( cmd == "v0f" ) { attr = 1;  vertexFormat = floatFormat; }
			else if ( cmd == "v1f" ) { attr = 1;  vertexFormat = floatFormat; }
			else if ( cmd == "v2f" ) { attr = 2;  vertexFormat = floatFormat; }
			else if ( cmd == "v3f" ) { attr = 3;  vertexFormat = floatFormat; }
			else if ( cmd == "v4f" ) { attr = 4;  vertexFormat = floatFormat; }
			else if ( cmd == "v5f" ) { attr = 5;  vertexFormat = floatFormat; }
			else if ( cmd == "v6f" ) { attr = 6;  vertexFormat = floatFormat; }
			else if ( cmd == "v7f" ) { attr = 7;  vertexFormat = floatFormat; }
			else if ( cmd == "v8f" ) { attr = 8;  vertexFormat = floatFormat; }
			else if ( cmd == "v9f" ) { attr = 9;  vertexFormat = floatFormat; }
			else if ( cmd == "v10f" ) { attr = 10; vertexFormat = floatFormat; }
			else if ( cmd == "v11f" ) { attr = 11; vertexFormat = floatFormat; }
			else if ( cmd == "v12f" ) { attr = 12; vertexFormat = floatFormat; }
			else if ( cmd == "v13f" ) { attr = 13; vertexFormat = floatFormat; }
			else if ( cmd == "v14f" ) { attr = 14; vertexFormat = floatFormat; }
			else if ( cmd == "v15f" ) { attr = 15; vertexFormat = floatFormat; }
			else if ( cmd == "v0ui" ) { attr = 0;  vertexFormat = intFormat; }
			else if ( cmd == "v1ui" ) { attr = 1;  vertexFormat = intFormat; }
			else if ( cmd == "v2ui" ) { attr = 2;  vertexFormat = intFormat; }
			else if ( cmd == "v3ui" ) { attr = 3;  vertexFormat = intFormat; }
			else if ( cmd == "v4ui" ) { attr = 4;  vertexFormat = intFormat; }
			else if ( cmd == "v5ui" ) { attr = 5;  vertexFormat = intFormat; }
			else if ( cmd == "v6ui" ) { attr = 6;  vertexFormat = intFormat; }
			else if ( cmd == "v7ui" ) { attr = 7;  vertexFormat = intFormat; }
			else if ( cmd == "v8ui" ) { attr = 8;  vertexFormat = intFormat; }
			else if ( cmd == "v9ui" ) { attr = 9;  vertexFormat = intFormat; }
			else if ( cmd == "v10ui" ) { attr = 10; vertexFormat = intFormat; }
			else if ( cmd == "v11ui" ) { attr = 11; vertexFormat = intFormat; }
			else if ( cmd == "v12ui" ) { attr = 12; vertexFormat = intFormat; }
			else if ( cmd == "v13ui" ) { attr = 13; vertexFormat = intFormat; }
			else if ( cmd == "v14ui" ) { attr = 14; vertexFormat = intFormat; }
			else if ( cmd == "v15ui" ) { attr = 15; vertexFormat = intFormat; }
			else { continue; }


			// create attribute buffer if it doesnt exist (throws)
			if ( !vertexAttributes_[attr].isAlloc( ) )
				createVertexAttribute( attr, vertexFormat, vertexCount );


			// add floating point coordinate to buffer
			if ( vertexFormat == VERTEX_FORMAT_X_32F ||
				 vertexFormat == VERTEX_FORMAT_XY_32F ||
				 vertexFormat == VERTEX_FORMAT_XYZ_32F ||
				 vertexFormat == VERTEX_FORMAT_XYZW_32F )
			{
				for ( unsigned int i = 0; i<n; ++i )
					vertexAttributes_[attr].write<float>( c[i] );
			}
			

			// add unsigned int coordinate to buffer
			if ( vertexFormat == VERTEX_FORMAT_X_32UI ||
				 vertexFormat == VERTEX_FORMAT_XY_32UI ||
				 vertexFormat == VERTEX_FORMAT_XYZ_32UI ||
				 vertexFormat == VERTEX_FORMAT_XYZW_32UI )
			{
				for ( unsigned int i = 0; i<n; ++i )
					vertexAttributes_[attr].write<unsigned int>( c[i] );
			}
		}
	}
}

void
MeshLoader::saveOBJ( const std::string& _path )
{
	// local vars
	std::ofstream ofs;
	unsigned int order[3] = {0,2,8};


	// open file for writing
	ofs.open( _path.c_str(), std::ofstream::out | std::ofstream::binary );
	if ( ofs.fail() )
		std::cout << "TODO ERROR: MeshData::saveOBJ() file not found\n";


	// save vertex attributes
	for ( unsigned int i=0; i<3; ++i )
	{
		// useful vars
		unsigned int attr	= order[i];


		// dont continue if attribute is not allocated
		if ( !vertexAttributes_[attr].isAlloc() )
			continue;


		// get vertex data type and dimension
		ALLOC_TYPE type = vertexAttributes_[attr].getType();
		ALLOC_DIM dim = vertexAttributes_[attr].getDim();


		// only supports floating point vertices
		if ( type == ALLOC_TYPE_32F )
		{
			// set command string
			std::stringstream	cmd;
			if ( attr==0 ) cmd << "v";
			else if ( attr==2 ) cmd << "vn";
			else if ( attr==8 ) cmd << "vt";


			// set 
			float e;
			vertexAttributes_[attr].seek<float>( 0 );
			ofs  << std::setprecision(6) << std::fixed;

			for ( int j=0; vertexAttributes_[attr].read<float>( &e ); ++j )
			{
				if ( j%dim==0 ) ofs << cmd.str();
				ofs << ' ' << e;
				if ( j%dim==dim-1 ) ofs << '\n';
			}
		}
	}


	// save primitives
	if ( true )
	{
		// get primitive data type and dimension
		ALLOC_TYPE type = primitives_.getType();
		ALLOC_DIM dim = primitives_.getDim();


		// only supports unsigned int indices
		if ( type == ALLOC_TYPE_32UI )
		{
			// set command string
			std::string cmd( "f" );

			unsigned int e;
			primitives_.seek<unsigned int>( 0 );
			//ofs << std::resetiosflags;

			for ( int j=0; primitives_.read<unsigned int>( &e ); ++j )
			{
				if ( j%dim==0 ) ofs << cmd;
				ofs << ' ' << e+1 << '/' << e+1 << '/' << e+1;
				if ( j%dim==dim-1 ) ofs << '\n';
			}
		}
	}


	// close file and report sucess
	ofs.close();
}

void
MeshLoader::saveOBX( const std::string& _path )
{
	// local vars
	std::ofstream ofs;
	unsigned int order[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


	// open file for writing
	ofs.open( _path.c_str(), std::ofstream::out | std::ofstream::binary );
	if ( ofs.fail() )
		std::cout << "TODO ERROR: MeshData::saveOBJ() file not found\n";


	// save vertex attributes
	for ( unsigned int i=0; i<MAX_VERTEX_ATTRIBUTES; ++i )
	{
		// useful vars
		unsigned int attr	= order[i];


		// dont continue if attibrute is not allocated
		if ( !vertexAttributes_[attr].isAlloc() )
			continue;


		// get vertex data type and dimension
		ALLOC_TYPE type = vertexAttributes_[attr].getType();
		ALLOC_DIM dim = vertexAttributes_[attr].getDim();


		// only supports floating point vertices
		if ( type == ALLOC_TYPE_32F )
		{
			// set command string
			std::stringstream cmd;
			cmd << "v" << attr << "f";

			// set 
			float e;
			vertexAttributes_[attr].seek<unsigned int>( 0 );
			ofs  << std::setprecision(6) << std::fixed;

			for ( int j=0; vertexAttributes_[attr].read<float>( &e ); ++j )
			{
				if ( j%dim==0 ) ofs << cmd.str();
				ofs << ' ' << e;
				if ( j%dim==dim-1 ) ofs << '\n';
			}
		}


		else if ( type == ALLOC_TYPE_32UI )
		{
			// set command string
			std::stringstream cmd;
			cmd << "v" << attr << "i";

			// set
			unsigned int e;
			vertexAttributes_[attr].seek<unsigned int>( 0 );

			for ( int j=0;
				  vertexAttributes_[attr].read<unsigned int>( &e ); ++j )
			{
				if ( j%dim==0 ) ofs << cmd.str();
				ofs << ' ' << e;
				if ( j%dim==dim-1 ) ofs << '\n';
			}
		}
	}


	// save primitives
	if ( true )
	{
		// get primitive data type and dimension
		ALLOC_TYPE type = primitives_.getType();
		ALLOC_DIM dim = primitives_.getDim();


		// only supports unsigned int indices
		if ( type == ALLOC_TYPE_32UI )
		{
			// set command string
			std::string cmd( "f" );

			unsigned int e;
			primitives_.seek<unsigned int>( 0 );
			//ofs << std::resetiosflags;

			for ( int j=0; primitives_.read<unsigned int>( &e ); ++j )
			{
				if ( j%dim==0 ) ofs << cmd;
				ofs << ' ' << e+1 << '/' << e+1 << '/' << e+1;
				if ( j%dim==dim-1 ) ofs << '\n';
			}
		}
	}


	// close file and report sucess
	ofs.close();
}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================