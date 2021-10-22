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
#include <QtCore/QByteArray>

// ImtCore includes
#include <imt3dview/IScene3dItem.h>
#include <imt3dview/IDrawable.h>


namespace imt3dview
{


/**
	Common interface for a 3D-scene.
*/
class IScene3d: virtual public IDrawable
{
public:
	enum ShapeFlags
	{
		SF_NONE = 0
	};

	/**
		Set scene camera.
	*/
	virtual void SetCamera(IScene3dCamera* cameraPtr) = 0;

	/**
		Set scene projection matrix.
	*/
	virtual void SetProjection(const QMatrix4x4& projection) = 0;

	/**
		Set scene view port.
	*/
	virtual void SetViewPort(const QRect& viewPort) = 0;

	/**
		Get scene bounding cuboid.
	*/
	virtual imt3d::CCuboid GetBoundingCuboid() const = 0;

	/**
		Get the list of shape IDs existing in the scene.
	*/
	virtual QByteArrayList GetShapeIds() const = 0;

	/**
		Get a scene shape by ID.
	*/
	virtual imt3dview::IScene3dItem* GetShapePtr(const QByteArray& id) const = 0;

	/**
		Add an 3D-shape to the scene.
		\return	If successfull, the method returns the UUID of the attached 3D-object, or an empty ID otherwise.
	*/
	virtual QByteArray AddShapeToScene(imt3dview::IScene3dItem* objectPtr, int shapeFlags = SF_NONE) = 0;

	/**
		Remove an existing 3D-shape from the scene.
	*/
	virtual void RemoveShapeFromScene(imt3dview::IScene3dItem* objectPtr) = 0;
};


} // namespace imt3dview


