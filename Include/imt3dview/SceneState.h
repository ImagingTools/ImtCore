// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>


namespace imt3dview
{


/**
	Per-frame scene state pushed into the backend at BeginFrame time.
	Replaces the global GL uniforms previously set inline in COpenGLWidget::SetGlUniformValues
	and the GL flags previously set in COpenGLWidget::SetGlFlags.
*/
struct SceneState
{
	enum RenderHint
	{
		RH_NONE = 0,
		RH_ANTIALIASING = 0x1,
		RH_CULLFACE = 0x2,
		RH_BLEND = 0x4
	};

	QMatrix4x4 viewMatrix;
	QMatrix4x4 projectionMatrix;
	QRect viewport;

	QVector3D cameraPosition;
	QVector3D lightPosition;
	QVector3D lightColor;
	QColor clearColor;

	int renderHints = RH_NONE;
};


} // namespace imt3dview
