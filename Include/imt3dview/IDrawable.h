#pragma once


// Qt includes
#include <QtGui/QPainter>
#if QT_VERSION < 0x060000
#include <QtGui/QOpenGLShaderProgram>
#else
#include <QtOpenGL/QOpenGLShaderProgram>
#endif

// ACF includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


/**
	Basic drawable object interface
*/
class IDrawable: virtual public istd::IPolymorphic
{
public:
	/**
		Set used OpenGL context for the drawing.
	*/
	virtual void SetContext(QOpenGLContext* contextPtr) = 0;

	/**
		Draw the shape using the given shader program and available OpenGL functions.
	*/
	virtual void DrawGl(QOpenGLShaderProgram& program) = 0;

	/**
		Draw the shape using painter.
	*/
	virtual void Draw(QPainter& painter) = 0;
};


} // namespace imt3dview


