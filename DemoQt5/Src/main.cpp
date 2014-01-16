
//==============================================================================
//
//
//
//==============================================================================


//== OS SPECIFIC DEFINES =======================================================


#ifdef _WIN32
#define NOMINMAX	/* protect against min/max macro */
#endif

//#define DEBUG_OPENGL_ENABLED 0

//== INCLUDES ==================================================================

#include "DemoQt5.h"
#include <QtWidgets\QApplication>


//== APPLICATION ENTRY POINT ===================================================

int main( int argc, char* argv[] )
{
	// specify format for OpenGL context window
	QGLFormat format;
	format.setAccum( false );			// default is false
	//format.setAccumBufferSize();		// 
	format.setAlpha( false );			// default is false
	//format.setAlphaBufferSize( 24 );
	//format.setBlueBufferSize( 24 );
	format.setDepth( true );			// default is true
	//format.setDepthBufferSize( 16 );	// ???
	format.setDirectRendering( true );	// default is true
	format.setDoubleBuffer( true );		// default is true
	//format.setGreenBufferSize( 24 ); 
	//format.setOption();				// combine flags into one function
	format.setOverlay( false );
	format.setPlane( 0 );
	format.setProfile( QGLFormat::CoreProfile );
	//format.setRedBufferSize( 24 );
	format.setRgba( true );				// default is true
	format.setSampleBuffers( false );
	//format.setSamples( 16 );
	format.setStencil( true );			// default is true
	//format.setStencilBufferSize();
	format.setStereo( false );			// default is false
	format.setSwapInterval( 0 );
	format.setVersion( 4, 2 );


	// create app and opengl window
	// show opengl window
	// start event loop
	QApplication app( argc, argv );
	DemoQt5 ogl( format );
	ogl.show();
	while (ogl.isVisible())
	{
		if (app.hasPendingEvents())
		{
			app.processEvents();
		}
		ogl.updateGL();
	}
	return 0;


	//std::cout << glFormat;
	// method 1 (qt mainloop, qt main window)
	//QApplication app(argc, argv);
	//MainWindow wnd;
	//wnd.show();
	//return app.exec();
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);


	// method 2 (qt mainloop, ogl is main window, relies on timer)
	//QApplication app(argc, argv);
	//AppBasic ogl;
	//oshow();
	//return app.exec();

}

//== THE END ===================================================================