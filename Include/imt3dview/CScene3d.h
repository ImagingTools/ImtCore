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
	void SetCamera(const IScene3dCamera* cameraPtr);
	void SetProjection(const QMatrix4x4& projection);
	const imt3d::CCuboid& GetBoundingCuboid() const override;
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
	const imt3dview::IScene3dCamera* m_cameraPtr;
	QMatrix4x4 m_projection;
	imt3d::CCuboid m_sceneBoundingCuboid;

	int m_nextModelId;
};


} // namespace imt3dview


