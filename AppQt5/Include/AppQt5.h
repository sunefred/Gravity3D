//==============================================================================
//
//
//==============================================================================


#ifndef APPQT5_H
#define APPQT5_H


//== INCLUDES ==================================================================

#include "Gem.h" // include before because of glew //
#include <QtOpenGL\QGLWidget>
#include <QtCore\QElapsedTimer>


//== NAMESPACES ================================================================

using namespace Mem;
using namespace Gem;


//== CLASS PREDEFENITIONS ======================================================

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE


//== CLASS DEFENITIONS =========================================================

class AppQt5 : public QGLWidget
{
	Q_OBJECT

public:

	//-- constructors/destructor -----------------------------------------------

	// default constructor

	// constructor with format
	AppQt5( const QGLFormat& format );

	// default copy constructor is ok

	// default destructor ok


protected:

	//-- update/draw related ---------------------------------------------------

	void initializeGL();
	void resizeGL( int _width, int _height );
	void paintGL();

	
	//-- keyboard callbacks ----------------------------------------------------

	void keyPressEvent( QKeyEvent* _e );
	void keyReleaseEvent( QKeyEvent* _e );


	//-- mouse callbacks -------------------------------------------------------

	void mousePressEvent( QMouseEvent* _e );
	void mouseReleaseEvent( QMouseEvent* _e );
	void mouseDoubleClickEvent( QMouseEvent* _e );
	void mouseMoveEvent( QMouseEvent* _e );
	void wheelEvent( QWheelEvent* _e );


private slots:


private:

	QElapsedTimer elapsedTimer_;


private:

	// Global paths
	std::string pathDataIn_;
	std::string pathDataOut_;
	std::string pathShaders_;


	// Loaders
	MeshLoader mesh_;
	ShaderLoader vertexShader_;
	ShaderLoader fragmentShader_;
	ShaderLoader geometryShader_;
	ShaderLoader tessCtrlShader_;
	ShaderLoader tessEvalShader_;


	// Rendering
	RenderStage renderStage_;
	float tesselationLevel_;

	
	// Nodes
	SceneNode root_;
	SceneNode objPivot_;
	SceneNode camPivot_;
	SceneNode lgtPivot_;
	CameraNode cam_;
	LightNode lgt_;


	// Controllers
	OrbitalController orbi_;


	// Animators
	long long frame_;

};


//== THE END ===================================================================

#endif