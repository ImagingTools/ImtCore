// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual void SetCamera(IScene3dCamera* cameraPtr) override;
	virtual void SetProjection(const QMatrix4x4& projection) override;
	virtual void SetViewPort(const QRect& viewPort) override;
	virtual imt3d::CCuboid GetBoundingCuboid() const override;
	virtual QByteArrayList GetShapeIds() const override;
	virtual imt3dview::IScene3dItem* GetShapePtr(const QByteArray& id) const override;
	virtual QByteArray AddShapeToScene(imt3dview::IScene3dItem* objectPtr, int itemFlags = SF_NONE) override;
	virtual void RemoveShapeFromScene(imt3dview::IScene3dItem* objectPtr) override;

	// reimplemented (imt3dview::IDrawable)
	virtual void SetContext(QOpenGLContext* contextPtr) override;
	virtual void DrawGl(QOpenGLShaderProgram& program) override;
	virtual void Draw(QPainter& painter) override;

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


