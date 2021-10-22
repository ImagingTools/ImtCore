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
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

// ImtCore includes
#include <imt3dview/IDrawable.h>
#include <imt3dview/IScene3dCamera.h>


namespace imt3dview
{


/**
	Basic drawable object interface
*/
class IScene3dItem: virtual public IDrawable
{
public:
	virtual bool IsValid() const = 0;

	virtual void SetCamera(const IScene3dCamera* cameraPtr) = 0;
	virtual void SetProjection(const QMatrix4x4& projection) = 0;
	virtual void SetViewPort(const QRect& viewPort) = 0;

	virtual const QVector3D& GetPosition() const = 0;
	virtual void SetPosition(const QVector3D& position) = 0;

	virtual const QQuaternion& GetRotation() const = 0;
	virtual void SetRotation(const QQuaternion& rotation) = 0;

	virtual float GetScale() const = 0;
	virtual void SetScale(float scale) = 0;
};


} // namespace imt3dview


