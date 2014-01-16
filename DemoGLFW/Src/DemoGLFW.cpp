//==============================================================================
//
//	
//
//==============================================================================


//== INCLUDES ==================================================================

#include "DemoGLFW.h"


//== CLASS IMPLEMENTATION ======================================================

DemoGLFW::DemoGLFW()
: frame_( 0 )
, tesselationLevel_( 3 )
, solidColor_( 1, 0, 0, 1 )
, wireColor_( 0, 1, 0, 1)
{
	pathDataIn_ = "..\\..\\..\\Data\\";
	pathDataOut_ = "..\\..\\..\\Out\\";
	pathShaders_ = "..\\..\\..\\Shaders\\";
}


//-- update/draw related -------------------------------------------------------
void
DemoGLFW::init()
{
	// init GLEW
	glewExperimental = GL_TRUE;
	if ( glewInit() == GLEW_OK )
	{
		GEM_CONSOLE( "glewInit() ok." );
	}
	else
	{
		GEM_ERROR( "glewInit() failed." );
	}


	// Test for OpenGL 4.2
	if ( GLEW_VERSION_4_2 )
	{
		GEM_CONSOLE( "OpenGL 4.2 supported." );
	}
	else
	{
		GEM_ERROR( "OpenGL 4.2 not supported." );
	}


	// Why isn' this core?
	if ( GL_EXT_texture_compression_s3tc )
	{
		GEM_CONSOLE( "S3TC texture compression supported." );
	}
	else
	{
		GEM_ERROR( "S3TC texture compression not supported." );
	}


	// Setup Scene Graph
	//
	//		root
	//		|--objectPivot
	//		|--cameraPivot
	//		|  `--camera
	//      |--lightPivot
	//		|  `--light
	//
	root_.setName( "root" );
	cam_.setName( "cam" );
	lgt_.setName( "lgt" );
	objPivot_.setName( "objPivot" );
	camPivot_.setName( "camPivot" );
	lgtPivot_.setName( "lgtPivot" );
	root_.addChild( &objPivot_ );
	root_.addChild( &camPivot_ );
	root_.addChild( &lgtPivot_ );
	camPivot_.addChild( &cam_ );
	lgtPivot_.addChild( &lgt_ );


	// Setup camera
	cam_.translate( 0, 0, 20 );
	cam_.setFrustum( 20 * Mem::DEG2RAD, 0.1, 1000 );
	camPivot_.rotate( -5 * Mem::DEG2RAD, 1, 0, 0 );


	// Setup lights
	lgt_.translate( 0, 0, 20 );
	//lgt_.setColor( 0.90, 0.90, 0.80 );
	lgtPivot_.rotate( 20 * Mem::DEG2RAD, 0, 1, 0 );


	// Navigation controller
	orbi_.setCameraPivot( &camPivot_ );
	orbi_.setCamera( &cam_ );


	// Load resources
	mesh_.load( pathDataIn_ + "box_triangles.obj" );
	meshtfb_.load( pathDataIn_ + "box_triangles.obj" );
	//meshtfb_.create( mesh_.getPrimitivesCount(),
	//				 mesh_.getVertexCount(),
	//				 mesh_.getPrimitivesType(),
	//				 mesh_.getVertexFormat( 0 ) );

	
	//texture_.load( pathDataIn_ + "world.200406.3x512x512.bmp" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512_monochrome.bmp" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512.pfm" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512_monochrome.pfm" );
	//texture_.load( pathDataIn_ + "world.200406.3x512x512.dds" );
	texture_.create( 512, 512, TEXTURE_FORMAT_RGB_32F );
	for ( unsigned int i = 0; i < 512; ++i )
	{
		for ( unsigned int j = 0; j < 512; ++j )
		{
			texture_.getMipLevePtr( 0 )->set<Vec3f>( i, j, Vec3f( 1, 0, 0 ) );
		}
	}
	vertexShader_.load( pathShaders_ + "pass_through.vert" );
	tessCtrlShader_.load( pathShaders_ + "pass_through.tesc" );
	tessEvalShader_.load( pathShaders_ + "pass_through.tese" );
	geometryShader_.load( pathShaders_ + "pass_through.geom" );
	fragmentShader_.load( pathShaders_ + "pass_through.frag" );


	//createPrimitiveSphere( 1, 64, 64 );
	createPrimitiveCube( 2 );


	// Setup render stages
	renderState_.setClear( true, Vec4f::ZERO, true, 1 );
	renderState_.setCulling( true, CULL_FACE_BACK );
	renderState_.setDepthTest( true, true, DEPTH_FUNC_LEQUAL );
	renderState_.setViewportPtr( cam_.getViewportWidthPtr(),
								 cam_.getViewportHeightPtr() );
	renderState_.setMesh( &mesh_ );
	////renderState_.setVertexIndexData( &primitives_ );
	////renderState_.setVertexAttributeData( &positions_, 0 );
	////renderState_.setVertexAttributeData( &normals_, 2 );
	////renderState_.setVertexAttributeData( &texCoords_, 8 );

	//renderState_.setTexture( &texture_, 0 );
	renderState_.setVertexShader( &vertexShader_ );
	//renderState_.setTessCtrlShader( &tessCtrlShader_ );
	//renderState_.setTessEvalShader( &tessEvalShader_ );
	renderState_.setGeometryShader( &geometryShader_ );
	renderState_.setFragmentShader( &fragmentShader_ );
	renderState_.setUniform( objPivot_.getDerivedTransformPtr(),
							 "modelMatrix" );
	renderState_.setUniform( cam_.getViewMatrixPtr(), "viewMatrix" );
	renderState_.setUniform( cam_.getProjMatrixPtr(), "projMatrix" );
	//renderState_.setUniform( &tesselationLevel_, "tesselationLevel" );
	////renderState_.setUniform( &solidColor_, "solidColor" );
	////renderState_.setUniform( &wireColor_, "wireColor" );
	//renderState_.setFramebuffer( &texture_, FRAMEBUFFER_ATTACHMENT_COLOR0 );
	//renderState_.setTransformFeedback( meshtfb_.getVertexAttributesPtr( 2 ),
	//								   "PerVertexXBF.Attr1" );
}


void
DemoGLFW::resize( int _width, int _height )
{
	cam_.setViewport( _width, _height );
}


void
DemoGLFW::draw()
{
	frame_++;


	// update scene
	root_.update();


	// Draw each Render State
	renderState_.draw();
}


//-- keyboard callbacks ----------------------------------------------------

void
DemoGLFW::keyboardPressFunc( const int _key, const float _x, const float _y,
							const int _mousestate, const int _modifiers )
{
	if ( _modifiers & KEYBOARD_MODIFIER_CTRL )
	{
		switch ( _key )
		{
		case KEYBOARD_KEY_R:
			mesh_.reload();
			texture_.reload();
			vertexShader_.reload();
			tessCtrlShader_.reload();
			tessEvalShader_.reload();
			geometryShader_.reload();
			fragmentShader_.reload();
			break;
		case KEYBOARD_KEY_S:
			renderState_.downloadBuffers();
			mesh_.save( pathDataOut_ + "mesh_.obj" );
			meshtfb_.save( pathDataOut_ + "meshtfb_.obj" );
			texture_.save( pathDataOut_ + "attachment_color0.pfm" );
			break;
		default:
			//
			break;
		}
	}

	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

void
DemoGLFW::keyboardReleaseFunc( const int _key, const float _x, const float _y,
							  const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _key, _x, _y, _mousestate, _modifiers );
}

//-- mouse callbacks -------------------------------------------------------

void
DemoGLFW::mousePressFunc( const int _button, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	orbi_.mousePressFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoGLFW::mouseReleaseFunc( const int _button, const float _x, const float _y,
						   const int _mousestate, const int _modifiers )
{
	orbi_.mouseReleaseFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoGLFW::mouseDoubleClickFunc( const int _button, const float _x, const float _y,
							   const int _mousestate, const int _modifiers )
{
	orbi_.mouseDoubleClickFunc( _button, _x, _y, _mousestate, _modifiers );
}

void
DemoGLFW::mouseMoveFunc( const float _x, const float _y,
						const int _mousestate, const int _modifiers )
{
	orbi_.mouseMoveFunc( _x, _y,_mousestate, _modifiers );
}
void
DemoGLFW::mouseWheelFunc( const int _delta, const float _x, const float _y,
						 const int _mousestate, const int _modifiers )
{
	if ( _modifiers == KEYBOARD_MODIFIER_NONE )
	{
		tesselationLevel_ = tesselationLevel_+Mem::signum( _delta );
		tesselationLevel_ = Mem::clamp<float>( tesselationLevel_, 1, 20 );
	}

	orbi_.mouseWheelFunc( _delta, _x, _y, _mousestate, _modifiers );
}

//-- private temp --------------------------------------------------------------

void
DemoGLFW::createPrimitiveSphere( const float _radius,
								const float _slices,
								const float _stacks,
								const PRIM_TYPE _primType )
{

	unsigned int primCount = 2 * _slices * (_stacks - 1);
	unsigned int vertCount = (_slices + 1) * (_stacks + 1) - 2;


	primitives_.alloc( ALLOC_FORMAT_VEC3_32UI, primCount );
	positions_.alloc( ALLOC_FORMAT_VEC3_32F, vertCount );
	normals_.alloc( ALLOC_FORMAT_VEC3_32F, vertCount );
	texCoords_.alloc( ALLOC_FORMAT_VEC2_32F, vertCount );


	// create primitives
	//
	// The primitives are create top to bottom row by row. The first row is
	// connected to the top vertices and is triangles only, same with the last
	// row. Everything in betweeen is quads that we create two triangles from.
	//
	unsigned int idx0, idx1, idx2, idx3;

	for ( unsigned int i = 0; i < _stacks; ++i )
	{
		for ( unsigned int j = 0; j < _slices; ++j )
		{
			// north pole triangles
			if ( i == 0 )
			{
				idx0 = j;
				idx1 = (i+1)*(_slices+1) + j - 1;
				idx2 = (i+1)*(_slices+1) + j + 0;
				primitives_.write<Vec3ui>( Vec3ui( idx0, idx1, idx2 ) ) ;
			}

			// south pole triangles
			else if ( i == (_stacks-1) )
			{
				idx0 = vertCount-1;
				idx1 = (i )*(_slices+1) + j - 1;
				idx2 = (i )*(_slices+1) + j + 0;
				primitives_.write<Vec3ui>( Vec3ui( idx0, idx2, idx1 ) ) ;
			}

			// everything else is quads that we make into two triangles
			else
			{
				idx0 = (i )*(_slices+1) + j - 1;
				idx1 = (i )*(_slices+1) + j + 0;
				idx2 = (i+1)*(_slices+1) + j - 1;
				idx3 = (i+1)*(_slices+1) + j + 0;
				primitives_.write<Vec3ui>( Vec3ui( idx0, idx2, idx1 ) );
				primitives_.write<Vec3ui>( Vec3ui( idx1, idx2, idx3 ) );
			}
		}
	}


	// create vertices
	//
	// Positions are create top to bottom same as the indices. The first row is
	// the nortpole vertex only and the bottom row is the south pole only.
	//
	float theta, phi;
	float x, y, z, u, v;
	float deltaPhi = Mem::PI / _stacks;
	float deltaTheta = Mem::TWOPI / _slices;
	float deltaU = 1.0 / _slices;
	float deltaV = 1.0 / _stacks;


	// middle vertices
	for ( unsigned int i=0; i<=_stacks; ++i )
	{
		for ( unsigned int j=0; j<=_slices; ++j )
		{
			// for north pole we skip first vertex
			if ( i == 0 && j == 0 ) continue;

			// for south pole we skip last vertex
			if ( i == _stacks && j == _slices ) continue;

			// here is vertex creation
			phi = deltaPhi*i;
			theta = Mem::PI + deltaTheta*j;
			z = std::cos( theta ) * std::sin( phi );
			x = std::sin( theta ) * std::sin( phi );
			y = std::cos( phi );
			u = deltaU * j;
			v = 1.0 - deltaV * i;
			positions_.write<Vec3f>( _radius * Vec3f( x, y, z ) );
			normals_.write<Vec3f>( Vec3f( x, y, z ) );
			texCoords_.write<Vec2f>( Vec2f( u, v ) );
		}
	}
}

void
DemoGLFW::createPrimitiveCube( const float _size,
							 const PRIM_TYPE _primType )
{
	unsigned int primCount = 12;
	unsigned int vertCount = 24;


	unsigned int triangles[12*3] = 
	{ 
		// front
		0, 1, 2,
		0, 2, 3,
		// back
		4, 5, 6,
		4, 6, 7,
		// top
		8, 9, 10,
		8, 10, 11,
		// bottom
		12, 13, 14,
		12, 14, 15,
		// right
		16, 17, 18,
		16, 18, 19,
		// left
		20, 21, 22,
		20, 22, 23
	};


	float positions[24*3] =
	{
		// top
		-1.000000, -1.000000,  1.000000,
		 1.000000, -1.000000,  1.000000,
		 1.000000,  1.000000,  1.000000,
		-1.000000,  1.000000,  1.000000,
		// back
		 1.000000, -1.000000, -1.000000,
		-1.000000, -1.000000, -1.000000,
		-1.000000,  1.000000, -1.000000,
		 1.000000,  1.000000, -1.000000,
		// top
		-1.000000,  1.000000,  1.000000,
		 1.000000,  1.000000,  1.000000,
		 1.000000,  1.000000, -1.000000,
		-1.000000,  1.000000, -1.000000,
		// bottom
		-1.000000, -1.000000, -1.000000,
		 1.000000, -1.000000, -1.000000,
		 1.000000, -1.000000,  1.000000,
		-1.000000, -1.000000,  1.000000,
		// right
		 1.000000, -1.000000,  1.000000,
		 1.000000, -1.000000, -1.000000,
		 1.000000,  1.000000, -1.000000,
		 1.000000,  1.000000,  1.000000,
		// left
		-1.000000, -1.000000, -1.000000,
		-1.000000, -1.000000,  1.000000,
		-1.000000,  1.000000,  1.000000,
		-1.000000,  1.000000, -1.000000,
	};
	for ( unsigned int i = 0; i < 24*3; ++i )
	{
		positions[i] *= _size / 2.0f;
	}


	float normals[24*3] =
	{
		// top
		 0.000000,  0.000000,  1.000000,
		 0.000000,  0.000000,  1.000000,
		 0.000000,  0.000000,  1.000000,
		 0.000000,  0.000000,  1.000000,
		// back
		 0.000000,  0.000000, -1.000000,
		 0.000000,  0.000000, -1.000000,
		 0.000000,  0.000000, -1.000000,
		 0.000000,  0.000000, -1.000000,
		// top
		 0.000000,  1.000000,  0.000000,
		 0.000000,  1.000000,  0.000000,
		 0.000000,  1.000000,  0.000000,
		 0.000000,  1.000000,  0.000000,
		// bottom
		 0.000000, -1.000000,  0.000000,
		 0.000000, -1.000000,  0.000000,
		 0.000000, -1.000000,  0.000000,
		 0.000000, -1.000000,  0.000000,
		// right
		 1.000000,  0.000000,  0.000000,
		 1.000000,  0.000000,  0.000000,
		 1.000000,  0.000000,  0.000000,
		 1.000000,  0.000000,  0.000000,
		// left
		-1.000000,  0.000000,  0.000000,
		-1.000000,  0.000000,  0.000000,
		-1.000000,  0.000000,  0.000000,
		-1.000000,  0.000000,  0.000000,
	};

	float texCoords[24*2] =
	{
		// front
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
		// back
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
		// top
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
		// bottom
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
		// right
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
		// left
		0.000000, 0.000000,
		1.000000, 0.000000,
		1.000000, 1.000000,
		0.000000, 1.000000,
	};


	primitives_.alloc( ALLOC_FORMAT_VEC3_32UI, primCount );
	positions_.alloc( ALLOC_FORMAT_VEC3_32F, vertCount );
	normals_.alloc( ALLOC_FORMAT_VEC3_32F, vertCount );
	texCoords_.alloc( ALLOC_FORMAT_VEC2_32F, vertCount );


	primitives_.write<unsigned int>( triangles, 12*3 );
	positions_.write<float>( positions, 24*3 );
	normals_.write<float>( normals, 24*3 );
	texCoords_.write<float>( texCoords, 24*2 );

}

//== THE END ===================================================================