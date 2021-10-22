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


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imt3d/CCuboid.h>
#include <imt3dview/IScene3d.h>


namespace imt3dview
{


/**
	3D-shape's scene.
*/
class CScene3d: virtual public imt3dview::IScene3d
{
public:
	CScene3d();

	// reimplemented (IScene3d)
	void SetCamera(IScene3dCamera* cameraPtr) override;
	void SetProjection(const QMatrix4x4& projection) override;
	void SetViewPort(const QRect& viewPort) override;
	imt3d::CCuboid GetBoundingCuboid() const override;
	QByteArrayList GetShapeIds() const override;
	imt3dview::IScene3dItem* GetShapePtr(const QByteArray& id) const override;
	QByteArray AddShapeToScene(imt3dview::IScene3dItem* objectPtr, int itemFlags = SF_NONE) override;
	void RemoveShapeFromScene(imt3dview::IScene3dItem* objectPtr) override;

	// reimplemented (imt3dview::IDrawable)
	void SetContext(QOpenGLContext* contextPtr) override;
	void DrawGl(QOpenGLShaderProgram& program);
	void Draw(QPainter& painter);

private:
	struct ShapeInfo
	{
		ShapeInfo()
			:shapePtr(NULL),
			flags(SF_NONE),
			modelId(-1)
		{
		}

		imt3dview::IScene3dItem* shapePtr;
		int flags;
		int modelId;
	};

	void UpdateBoundingCuboid();
	void UpdateBoundingCuboid(const imt3dview::IScene3dItem* objectPtr);

	void UpdateItemsScale();
	void UpdateItemScale(IScene3dItem& scene3dItem);

	static imt3d::CCuboid GetItemBoundingCuboid(const IScene3dItem* scene3dItemPtr);

private:
	typedef QSharedPointer<ShapeInfo> ShapeInfoPtr;
	typedef QMap<QByteArray, ShapeInfoPtr> Shapes;
	Shapes m_shapes;

	QOpenGLContext* m_contextPtr;
	int m_nextModelId;
	imt3dview::IScene3dCamera* m_cameraPtr;
};


} // namespace imt3dview


