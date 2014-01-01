//== INCLUDES ==================================================================

#include "GemShaderLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================

//-- constructors/destructor -----------------------------------------------

// default constructor
ShaderLoader::ShaderLoader()
: path_( "" )
, shaderType_( SHADER_TYPE_NONE )
, source_( )
, isLoaded_( false )
{

}

// default copy constructor is
ShaderLoader::~ShaderLoader()
{
	clear();
}

//-- copy and clear --------------------------------------------------------

void
ShaderLoader::copy( const ShaderLoader& other )
{
	// no deep copy needed
	(*this) = other;
}

void
ShaderLoader::clear()
{
	source_.clear();
	isLoaded_ = false;
}


//-- load and create -------------------------------------------------------

void
ShaderLoader::load( const std::string& _path,
					const SHADER_TYPE _shaderType )
{
	// argument checks
	if ( _path.empty() )
		GEM_ERROR( "Path argument is empty." );


	// always create new
	if ( isLoaded_ )
		clear();


	// local variables
	SHADER_TYPE shaderType = _shaderType;
	std::string path = _path;


	// split path
	std::string dir, name, ext;
	splitPath( path, &dir, &name, &ext );


	// Tell the user that we are trying to load the file
	GEM_CONSOLE( "Loading shader " + name + (ext.empty() ? "" : ".") + ext );


	// Determine file format from extension
	if ( shaderType == SHADER_TYPE_NONE )
	{
		if ( ext == "vert" )
		{
			shaderType = SHADER_TYPE_VERTEX;
		}
		else if ( ext == "frag" )
		{
			shaderType = SHADER_TYPE_FRAGMENT;
		}
		else if ( ext == "geom" )
		{
			shaderType = SHADER_TYPE_GEOMETRY;
		}
		else if ( ext == "tesc" )
		{
			shaderType = SHADER_TYPE_TESSELATION_CONTROL;
		}
		else if ( ext == "tese" )
		{
			shaderType = SHADER_TYPE_TESSELATION_EVALUATION;
		}
	}


	// activate the right file loader depending on file type
	try
	{
		switch( shaderType )
		{
		case SHADER_TYPE_VERTEX:
		case SHADER_TYPE_FRAGMENT:
		case SHADER_TYPE_GEOMETRY:
		case SHADER_TYPE_TESSELATION_CONTROL:
		case SHADER_TYPE_TESSELATION_EVALUATION:
			loadShader( path );
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
	shaderType_ = shaderType;
	isLoaded_ = true;
}

void
ShaderLoader::reload()
{
	if ( isLoaded_ )
	{
		// clear before reloading
		clear();

		// and load
		load( path_, shaderType_ );
	}
}


//-- private file-io -------------------------------------------------------

void
ShaderLoader::loadShader( const std::string& _path )
{
	// open file for reading
	std::ifstream file( _path.c_str(), std::ios::binary );
	if ( file.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}


	// get file size
	file.seekg( 0, std::ios::end );
	unsigned int fileSize = static_cast<unsigned int>(file.tellg( ));
	

	// allocate data store with extra nullterminator
	//source_.resize( fileSize );
	source_.alloc( ALLOC_FORMAT_SCALAR_8UI, fileSize + 1 );
		

	// read file
	file.seekg( 0, std::ios::beg );
	file.read( source_.getWritePtr<char>(), fileSize );
	if ( file.fail() )
	{
		source_.clear();
		GEM_THROW( "Failed to read file " + _path );
	}
}


//==============================================================================
GEM_END_NAMESPACE
//==============================================================================