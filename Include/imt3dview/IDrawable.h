/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#pragma once


// Qt includes
#include <QtGui/QPainter>
#include <QtGui/QOpenGLShaderProgram>


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


