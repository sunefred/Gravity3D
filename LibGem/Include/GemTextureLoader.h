//==============================================================================
//
// An empty class, copy this when creating a new class.h
//
//==============================================================================


#ifndef GEM_EMPTY_H
#define GEM_EMPTY_H


//== INCLUDES ==================================================================

#include "GemPrerequisites.h"
#include "GemLoader.h"
#include "GemAllocator.h"


//== NAMESPACES ================================================================

GEM_BEGIN_NAMESPACE


//== CLASS DECLARATION =========================================================

class TextureLoader : public Loader
{

private:

	//-- define, typedef, enum -------------------------------------------------

	struct BITMAPINFOHEADER {
		unsigned long	size;
				 long	width;
				 long	height;
		unsigned short	planes;
		unsigned short	bitCount;
		unsigned long	compression;
		unsigned long	sizeImage;
				 long	xPelsPerMeter;
				 long	yPelsPerMeter;
		unsigned long	clrUsed;
		unsigned long	clrImportant;
	};

	struct BITMAPFILEHEADER {
		char			type[2];	
		unsigned long	size;
		unsigned short	reserved1;
		unsigned short	reserved2;
		unsigned long	offBits;
	};

	struct PFMHEADER {
		unsigned char	p;
		unsigned char	f;
		unsigned long	width;
		unsigned long	height;
		float			scalefactor;
	};

	// This is the DDSURFACEDESC2 struct from ddraw.h with all the union
	// defenitions removed and some of the unrelated vars compacted into
	// reserved[] arrays. Header is 32*4 = 128 bytes
	struct DDSHEADER {
		struct {
			char			type[4];
			unsigned int    size;		// size of the DDSURFACEDESC structure
			unsigned int    flags;		// determines what fields are valid
			unsigned int    height;		// height of surface to be created
			unsigned int    width;		// width of input surface
			unsigned int    linearSize;	// Formless late-allocated optimized...
			unsigned int    depth;		// the depth if this is a volume texture 
			unsigned int    mipMapCount;// number of mip-map levels requestde
			unsigned int	reserved1[11];			// things not used for dds
			//  DDPIXELFORMAT
			struct {
				unsigned int    size;				// size of structure
				unsigned int    flags;				// pixel format flags
				unsigned int    fourCC;				// (FOURCC code)
				unsigned int    rgbBitCount;		// how many bits per pixel
				unsigned int    rBitMask;			// mask for red bit
				unsigned int    gBitMask;			// mask for green bits
				unsigned int    bBitMask;			// mask for blue bits
				unsigned int    rgbAlphaBitMask;	// mask for alpha channel
			} pixelFormat;
			//  DDCAPS2
			struct {
				unsigned int    caps1;
				unsigned int    caps2;
				unsigned int    caps3;
				unsigned int    caps4;
			} caps;
			unsigned int    reserved2;
		};
	};

	#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
	#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
	#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
	#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
	#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))


public:
	
	//-- constructors/destructor -----------------------------------------------

	// default constructor
	TextureLoader();

	// default copy constructor is ok

	// destructor
	~TextureLoader();

	// default assignment operator is ok


	//-- copy and clear --------------------------------------------------------

	void copy( const TextureLoader& other );

	void clear();


public:

	//-- sets and gets ---------------------------------------------------------

	Allocator* getMipLevePtr( const unsigned int _mipLevel )
	{ return &mipLevels_[_mipLevel]; }

	unsigned int getMipLevelCount() const 
	{ return mipLevelCount_; }
	
	bool isLoaded() const
	{ return isLoaded_; }

public:

	//-- load and create -------------------------------------------------------

	void load( const std::string& _path,
			   const FILE_FORMAT _fileFormat = FILE_FORMAT_NONE );

	void reload();

	void create( const unsigned int _width, const unsigned int _height, 
				 const TEXTURE_FORMAT _textureFormat,
				 const bool _generateMipLevels = false );

	void save( const std::string& _path,
			   const FILE_FORMAT _fileFormat = FILE_FORMAT_NONE );



protected:


private:

	//-- load and create -------------------------------------------------------

	void createMipLevels( const unsigned int _width,
						  const unsigned int _height,
						  const TEXTURE_FORMAT _textureFormat,
						  unsigned int _maxMipLevelCount = MAX_MIP_LEVELS );


	//-- private file-io -------------------------------------------------------

	void loadBMP( const std::string& _path );

	void saveBMP( const std::string& _path );

	void loadPFM( const std::string& _path );

	void savePFM( const std::string& _path );

	void loadDDS( const std::string& _path );

	void saveDDS( const std::string& _path );

private:

	// file information
	std::string	 path_;
	FILE_FORMAT fileFormat_;

	// Mip level data store
	unsigned int width_;
	unsigned int height_;
	TEXTURE_FORMAT textureFormat_;
	unsigned int mipLevelCount_;
	Allocator mipLevels_[MAX_MIP_LEVELS];

	// status flags
	bool isLoaded_;
};


//==============================================================================
GEM_END_NAMESPACE
#endif
//==============================================================================