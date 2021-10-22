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
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

// ACF includes
#include <istd/IPolymorphic.h>

// Imt includes
#include <imt3d/CCuboid.h>

namespace imt3dview
{


/**
	Basic 3D-scene-camera interface
*/
class IScene3dCamera: virtual public istd::IPolymorphic
{
public:

	virtual const QVector3D& GetPosition() const = 0;
	virtual void MoveTo(const QVector3D& position) = 0;
	virtual void MoveTo(const QPoint& pointFrom, const QPoint& pointTo) = 0;

	virtual const QQuaternion& GetRotation() const = 0;
	virtual void RotateTo(const QQuaternion& rotation) = 0;
	virtual void RotateTo(const QPoint& pointFrom, const QPoint& pointTo, const QVector3D& axis = QVector3D()) = 0;

	virtual void ZoomIn() = 0;
	virtual void ZoomOut() = 0;
	virtual QMatrix4x4 GetViewMatrix() const = 0;

	virtual void SetBoundingCuboid(const imt3d::CCuboid& cuboid) = 0;
	virtual imt3d::CCuboid GetBoundingCuboid() const = 0;
};


} // namespace imt3dview


