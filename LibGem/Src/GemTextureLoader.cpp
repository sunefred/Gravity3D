//== INCLUDES ==================================================================

#include "GemTextureLoader.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DEFINITION ==========================================================

//-- constructors/destructor ---------------------------------------------------

TextureLoader::TextureLoader()
: path_()
, fileFormat_( FILE_FORMAT_NONE )
, width_( 0 )
, height_( 0 )
, textureFormat_( TEXTURE_FORMAT_NONE )
, mipLevelCount_( 0 )
, isLoaded_( false )
{
}

TextureLoader::~TextureLoader( )
{
	clear( );
}


//-- copy and clear ------------------------------------------------------------

void
TextureLoader::clear( )
{
	// file information
	path_.clear();
	fileFormat_ = FILE_FORMAT_NONE;

	// Mip level data store
	width_ = 0;
	height_ = 0;
	textureFormat_ = TEXTURE_FORMAT_NONE;
	mipLevelCount_ = 0;
	for ( unsigned int i = 0; i < MAX_MIP_LEVELS; ++i )
	{
		mipLevels_[i].clear();
	}

	// status flags
	bool isLoaded_ = false;
}

void
TextureLoader::copy( const TextureLoader& other )
{
	// no deep copy needed
	(*this) = other;
}


//-- load and create -----------------------------------------------------------

void
TextureLoader::load( const std::string& _path,
					 const FILE_FORMAT _fileFormat )
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
	GEM_CONSOLE( "Loading texture " + name + (ext.empty() ? "" : ".") + ext );


	// Determine file format from extension
	if ( fileFormat == FILE_FORMAT_NONE )
	{
		if ( ext == "bmp" )
		{
			fileFormat = FILE_FORMAT_BMP;
		}
		else if ( ext == "pfm" )
		{
			fileFormat = FILE_FORMAT_PFM;
		}
		else if ( ext == "dds" )
		{
			fileFormat = FILE_FORMAT_DDS;
		}
	}


	// activate the right file loader depending on file type
	try
	{
		switch ( fileFormat )
		{
		case FILE_FORMAT_BMP:
			loadBMP( path );
			break;
		case FILE_FORMAT_PFM:
			loadPFM( path );
			break;
		case FILE_FORMAT_DDS:
			loadDDS( path );
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
TextureLoader::reload()
{
	if ( isLoaded_ )
	{
		// temporary variables that is lost on clear
		std::string path = path_;
		FILE_FORMAT fileFormat = fileFormat_;
		unsigned int width = width_;
		unsigned int height = height_;
		TEXTURE_FORMAT textureFormat = textureFormat_;
		unsigned int mipLevelCount = mipLevelCount_;


		// clear before reloading
		clear();


		// load or create?
		if ( path.empty() )
		{
			create( width, height, textureFormat, ( mipLevelCount_ > 1 ) );
		}
		else
		{
			load( path_, fileFormat_ );
		}
	}
}

void
TextureLoader::create( const unsigned int _width, const unsigned int _height, 
					   const TEXTURE_FORMAT _textureFormat,
					   const bool _createMipLevels )
{
	// argument checks
	if ( _width < 1 )
		GEM_ERROR( "Invalid texture width " + _width );
	if ( _height < 1 )
		GEM_ERROR( "Invalid texture height " + _height );
	if ( _textureFormat == TEXTURE_FORMAT_NONE )
		GEM_ERROR( "Invalid texture format " + _textureFormat );


	// always create new
	if ( isLoaded_ )
		clear( );


	// allocate memory for first mip level
	try
	{
		if ( _createMipLevels )
		{
			createMipLevels( _width, _height, _textureFormat, MAX_MIP_LEVELS );
		}
		else
		{
			createMipLevels( _width, _height, _textureFormat, 1 );
		}
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
TextureLoader::save( const std::string& _path,
					 const FILE_FORMAT _fileFormat )
{
	// argument checks
	if ( _path.empty() )
		GEM_ERROR( "Path argument is empty." );


	// cant save empty file
	if ( !isLoaded_ )
		return;


	// local variables
	FILE_FORMAT fileFormat = _fileFormat;
	std::string path = _path;


	// split path
	std::string dir, name, ext;
	splitPath( path, &dir, &name, &ext );


	// Tell the user that we are trying to save the file
	GEM_CONSOLE( "Saving texture " + name + (ext.empty() ? "" : ".") + ext );


	// First determine file-type
	if ( fileFormat == FILE_FORMAT_NONE )
	{
		if ( ext == "bmp" )
		{
			fileFormat = FILE_FORMAT_BMP;
		}
		else if ( ext == "pfm" )
		{
			fileFormat = FILE_FORMAT_PFM;
		}
	}


	// activate the right file loader depending on file type
	try
	{
		switch ( fileFormat )
		{
		case FILE_FORMAT_BMP:
			saveBMP( path );
			break;
		case FILE_FORMAT_PFM:
			savePFM( path );
			break;
		default:
			GEM_THROW( "Unsupported file type ." + ext );
		}
	}
	catch( const std::exception& e )
	{
		GEM_ERROR( e.what() );
	}
}

//-- private load and create -------------------------------------------------------

void
TextureLoader::createMipLevels( const unsigned int _width,
							    const unsigned int _height,
							    const TEXTURE_FORMAT _textureFormat,
								unsigned int _maxMipLevelCount )
{
	// Calculate maximum miplevel
	//
	// The mip level will be calculated such that the largest dimension will
	// be one pixel wide. So for instance a 3000x512 texture will be 8x1 at
	// mip level 9. Formula used is:
	//
	//		ceil( log_2( max( w, h ) ) ) + 1
	//
	// The extra "1" is because we want to end with a pixel width of 1 (not 2)
	//
	double width = _width;
	double height = _height;
	double maxSize = std::max( width, height );
	unsigned int mipLevelCount;
	mipLevelCount = std::ceil( std::log( maxSize ) / std::log( 2.0 ) ) + 1;
	mipLevelCount = std::min( mipLevelCount, _maxMipLevelCount );


	// Allocate data for each mip level
	// 
	// The formula for calculating width w_i of mip level i for a normal
	// texture is
	//
	//		w_i = floor( w_0 / 2^i )
	//
	// Because we use floor here we might get a size of 0 for the smallest 
	// dimensions, so make sure to clamp it.
	//
	// Notice that DXT textures have to have a width and hight divisible by 4
	// (the block size) for ALL mip map levels. The common strategy seems to be
	// to round up to the closest number divisible by 4. If you get read errors
	// in LoadDDS, make sure your file writer respects this.
	//
	for ( unsigned int w, h, i = 0; i < mipLevelCount; ++i )
	{
		w = std::max( std::floor( width / (1 << i) ), 1.0 );
		h = std::max( std::floor( height / (1 << i) ), 1.0 );
		if ( _textureFormat == TEXTURE_FORMAT_RGB_DXT1 )
		{
			w = std::ceil( w / 4.0 ) * 4u;
			h = std::ceil( h / 4.0 ) * 4u;
		}
		mipLevels_[i].alloc( static_cast<ALLOC_FORMAT>(_textureFormat), w, h );
	}


	// set member variables
	width_ = _width;
	height_ = _height;
	textureFormat_ = _textureFormat;
	mipLevelCount_ = mipLevelCount;
}


//-- private file-io -----------------------------------------------------------

void
TextureLoader::loadBMP( const std::string& _path )
{
	// local vars
	std::ifstream ifs;
	BITMAPFILEHEADER filehdr;
	BITMAPINFOHEADER infohdr;


	// open file for reading
	ifs.open( _path.c_str(), std::ifstream::in | std::ifstream::binary );
	if ( ifs.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}


	// read file header.
	// values are read one by one rather then the whole struct at once because
	// compilers will not always do two-padding of structs. This way is
	// preferred here over getting into compiler specific directives.
	ifs.read( reinterpret_cast<char*>(&filehdr.type), 2 );
	ifs.read( reinterpret_cast<char*>(&filehdr.size), 4 );
	ifs.read( reinterpret_cast<char*>(&filehdr.reserved1), 2 );
	ifs.read( reinterpret_cast<char*>(&filehdr.reserved2), 2 );
	ifs.read( reinterpret_cast<char*>(&filehdr.offBits), 4 );
	if ( ifs.fail( ) )
	{
		ifs.close();
		GEM_THROW( "Failed to read image file header." );
	}

	
	// make sure its a bitmap
	if ( filehdr.type == "BM" )
	{
		ifs.close();
		GEM_THROW( "File is not a bitmap." );
	}


	// read info header
	ifs.read( reinterpret_cast<char*>(&infohdr), sizeof( BITMAPINFOHEADER ) );
	if ( ifs.fail( ) )
	{
		ifs.close();
		GEM_THROW( "Failed to read image info header." );
	}


	// read 24-bit (rgb)
	if ( infohdr.bitCount==24 )
	{
		// allocate data
		createMipLevels( infohdr.width, infohdr.height,
						 TEXTURE_FORMAT_RGB_8UI, 1 );

		
		// get character ptr and total bytecount
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();


		// jump to data section
		ifs.seekg( filehdr.offBits, std::ios::beg );

		
		// read image data
		ifs.read( ptr, bytecount );
		if ( ifs.fail() )
		{
			ifs.close();
			GEM_THROW( "Failed to read image data." );
		}


		// data is in BGR format, so swap R and B
		for ( int i=0; i<bytecount; i+=3 )
		{
			ptr[i] ^= ptr[i+2];
			ptr[i+2] ^= ptr[i];
			ptr[i] ^= ptr[i+2];
		}
	}


	// read 8-bit (monochrome)
	else if ( infohdr.bitCount==8 )
	{
		// set variables and allocate data
		createMipLevels( infohdr.width, infohdr.height,
						 TEXTURE_FORMAT_R_8UI, 1 );
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();

		
		// jump to data section
		ifs.seekg( filehdr.offBits, std::ios::beg );

		
		// read image data
		ifs.read( ptr, bytecount );
		if ( ifs.fail() )
		{
			ifs.close();
			GEM_THROW( "Failed to read image data." );
		}
	}


	// bitcount is not supported, close and report failiure
	else
	{
		ifs.close();
		GEM_THROW( "Bitmap format not supported." );
	}


	// close file and return
	ifs.close();
}

void
TextureLoader::saveBMP( const std::string& _path )
{
	// local vars
	std::ofstream ofs;
	BITMAPFILEHEADER filehdr;
	BITMAPINFOHEADER infohdr;


	// Stupidity check
	if ( !mipLevels_[0].isAlloc() )
	{
		GEM_THROW( "Texture has not been allocated." );
	}


	// open file for writing
	ofs.open( _path.c_str(), std::ofstream::out | std::ofstream::binary );
	if ( ofs.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}
		

	// 8-bit/channel integer RGB
	if ( textureFormat_ == TEXTURE_FORMAT_RGB_8UI )
	{
		// fill in file header
		filehdr.type[0] = 'B';
		filehdr.type[1] = 'M';
		filehdr.size = mipLevels_[0].getByteCount();
		filehdr.reserved1 = 0;
		filehdr.reserved2 = 0;
		filehdr.offBits = 54;


		// write file header to file
		// values are written one by one rather then the whole struct at once
		// because compilers will not always do two-padding of structs. This
		// way is preferred here over getting into compiler specific directives.
		ofs.write( reinterpret_cast<char*>(&filehdr.type), 2 );
		ofs.write( reinterpret_cast<char*>(&filehdr.size), 4 );
		ofs.write( reinterpret_cast<char*>(&filehdr.reserved1), 2 );
		ofs.write( reinterpret_cast<char*>(&filehdr.reserved2), 2 );
		ofs.write( reinterpret_cast<char*>(&filehdr.offBits), 4 );
		if ( ofs.fail() )
		{
			ofs.close();
			GEM_THROW( "Could not write header to file" );
		}


		// fill in info header
		infohdr.size = 40;
		infohdr.width = mipLevels_[0].getWidth();
		infohdr.height = mipLevels_[0].getHeight();
		infohdr.planes = 1;
		infohdr.bitCount = mipLevels_[0].getBitsPerElement();
		infohdr.compression = 0;
		infohdr.sizeImage = mipLevels_[0].getByteCount();
		infohdr.xPelsPerMeter = 0;
		infohdr.yPelsPerMeter = 0;
		infohdr.clrUsed = 0;
		infohdr.clrImportant = 0;


		// write info header to file
		ofs.write( reinterpret_cast<char*>(&infohdr), 40 );
		if ( ofs.fail() )
		{
			ofs.close();
			GEM_THROW( "Could not write info header to file" );
		}


		// get data pointer
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();


		// file should be in BGR, so swap B and R
		for ( int i=0; i<bytecount; i+=3 )
		{
			ptr[i] ^= ptr[i+2];
			ptr[i+2] ^= ptr[i];
			ptr[i] ^= ptr[i+2];
		}


		// write data to file
		ofs.write( ptr, bytecount );
		if ( ofs.fail() )
		{
			ofs.close();
			GEM_THROW( "Could not write data to file" );
		}


		// data is in RGB format, so swap R and B
		for ( int i=0; i<bytecount; i+=3 )
		{
			ptr[i] ^= ptr[i+2];
			ptr[i+2] ^= ptr[i];
			ptr[i] ^= ptr[i+2];
		}

	}
	
	
	// format not supported
	else
	{
		ofs.close();
		GEM_THROW( "Bitmap format not supported" );
	}


	// close file and report sucess
	ofs.close();
}

void
TextureLoader::loadPFM( const std::string& _path )
{
	// local vars
	PFMHEADER hdr;
	std::ifstream ifs;
	char ignore;


	// open file for reading
	ifs.open( _path.c_str(), std::ifstream::in | std::ifstream::binary );
	if ( ifs.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}


	// get header info
	ifs >> hdr.p >> hdr.f				// identifier line
		>> hdr.width >> hdr.height		// dimension line
		>> hdr.scalefactor;				// scale/endianess line
		ifs.getline( &ignore, 1 );		// chew last linefeed
	if ( hdr.p!='P' || ( hdr.f!='F' && hdr .f!='f' ) )
	{
		ifs.close();
		GEM_THROW( "Failed to read image header." );
	}


	// 32-bit/channel floating point RGB
	if ( hdr.f=='F' )
	{
		// allocate data
		createMipLevels( hdr.width, hdr.height, TEXTURE_FORMAT_RGB_32F, 1 );


		// get character ptr and total bytecount
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();


		// read floating point data from file
		ifs.read( ptr, bytecount );
		if ( ifs.fail() )
		{
			ifs.close();
			GEM_THROW( "Failed to read image data." );
		}
	}

 
	// 32-bit/channel floating point monochrome
	else
	{
		// set variables and allocate data
		createMipLevels( hdr.width, hdr.height, TEXTURE_FORMAT_R_32F, 1 );
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();


		// read floating point data from file
		ifs.read( ptr, bytecount );
		if ( ifs.fail() )
		{
			ifs.close();
			GEM_THROW( "Failed to read image data." );
		}
	}


	// clean up and return
	ifs.close();
}

void
TextureLoader::savePFM( const std::string& _path )
{
	// local vars
	PFMHEADER hdr;
	std::ofstream ofs;


	// Stupidity check
	if ( !mipLevels_[0].isAlloc() )
	{
		GEM_THROW( "Texture has not been allocated." );
	}


	// open file for writing
	ofs.open( _path.c_str(), std::ofstream::out | std::ofstream::binary );
	if ( ofs.fail() )
	if ( ofs.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}


	// 16-bit/channel floating point RGB
	// 32-bit/channel floating point RGB
	// 16-bit/channel floating point monochrome
	// 32-bit/channel floating point monochrome
	ALLOC_TYPE allocType = mipLevels_[0].getType();
	ALLOC_DIM allocDim = mipLevels_[0].getDim();
	if ( allocType == ALLOC_TYPE_32F )
	{
		// set header info
		hdr.p = 'P';
		hdr.f = allocDim == ALLOC_DIM_SCALAR ? 'f' : 'F';
		hdr.width = mipLevels_[0].getWidth();
		hdr.height = mipLevels_[0].getHeight();
		hdr.scalefactor = -1;


		// write header to file
		ofs << hdr.p << hdr.f << '\n'
			<< hdr.width << ' ' << hdr.height << '\n'
			<< std::fixed << std::setprecision(6) << hdr.scalefactor << '\n';
		if ( ofs.fail() )
		{
			ofs.close();
			GEM_THROW( "Could not write header to file" );
		}


		// get data pointer
		char* ptr = mipLevels_[0].getWritePtr<char>();
		int bytecount =  mipLevels_[0].getByteCount();


		// write data to file
		ofs.write( ptr, bytecount );
		if ( ofs.fail() )
		{
			ofs.close();
			GEM_THROW( "Could not write data to file" );
		}
	}


	// format not supported
	else
	{
		ofs.close();
		GEM_THROW( "PFM format not supported" );
	}


	// cleanup and return
	ofs.close();
}

void
TextureLoader::loadDDS( const std::string& _path )
{
	// local vars
	std::ifstream ifs;
	DDSHEADER ddshdr;


	// open file for reading
	ifs.open( _path.c_str(), std::ifstream::in | std::ifstream::binary );
	if ( ifs.fail() )
	{
		GEM_THROW( "Could not open file " + _path );
	}


	// read dds header
	ifs.read( reinterpret_cast<char*>(&ddshdr), sizeof( DDSHEADER ) );
	if ( ifs.fail( ) )
	{
		ifs.close();
		GEM_THROW( "Failed to read image file header." );
	}


	// make sure its a .dds file
	if ( ddshdr.type == "DDS " )
	{
		ifs.close();
		GEM_THROW( "File is not a DDS file." );
	}


	// DXT1 format
	if ( ddshdr.pixelFormat.fourCC==FOURCC_DXT1 )
	{
		// allocate data
		createMipLevels( ddshdr.width, ddshdr.height,
						 TEXTURE_FORMAT_RGB_DXT1, ddshdr.mipMapCount );


		// read image data for each mipmap
		for ( unsigned int i=0; i<mipLevelCount_; ++i )
		{
			// get character ptr and total bytecount
			char* ptr = mipLevels_[i].getWritePtr<char>();
			int bytecount =  mipLevels_[i].getByteCount();

			ifs.read( ptr, bytecount );
			if ( ifs.fail() )
			{
				ifs.close();
				GEM_THROW( "Failed to read image data." );
			}
		}
	}


	// format not supported
	else
	{
		ifs.close();
		GEM_THROW( "This file is not a DXT1 file." );
	}


	// close file and return
	ifs.close();
}

void
TextureLoader::saveDDS( const std::string& _path )
{
}

//==============================================================================
GEM_END_NAMESPACE
//==============================================================================