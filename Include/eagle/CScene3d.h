#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imt3d/CCuboid.h>

// Eagle includes
#include <eagle/IScene3d.h>


namespace eagle
{


/**
	3D-shape's scene.
*/
class CScene3d: virtual public eagle::IScene3d
{
public:
	CScene3d();

	// reimplemented (IScene3d)
	void SetCamera(const IScene3dCamera* cameraPtr);
	void SetProjection(const QMatrix4x4& projection);
	const imt3d::CCuboid& GetBoundingCuboid() const override;
	QByteArrayList GetShapeIds() const override;
	eagle::IScene3dItem* GetShapePtr(const QByteArray& id) const override;
	QByteArray AddShapeToScene(eagle::IScene3dItem* objectPtr, int itemFlags = SF_NONE) override;
	void RemoveShapeFromScene(eagle::IScene3dItem* objectPtr) override;

	// reimplemented (eagle::IDrawable)
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

		eagle::IScene3dItem* shapePtr;
		int flags;
		int modelId;
	};

	void UpdateBoundingCuboid();
	void UpdateBoundingCuboid(const eagle::IScene3dItem* objectPtr);

	void UpdateItemsScale();
	void UpdateItemScale(IScene3dItem& scene3dItem);

	static imt3d::CCuboid GetItemBoundingCuboid(const IScene3dItem* scene3dItemPtr);

private:
	typedef QSharedPointer<ShapeInfo> ShapeInfoPtr;
	typedef QMap<QByteArray, ShapeInfoPtr> Shapes;
	Shapes m_shapes;

	QOpenGLContext* m_contextPtr;
	const eagle::IScene3dCamera* m_cameraPtr;
	QMatrix4x4 m_projection;
	imt3d::CCuboid m_sceneBoundingCuboid;

	int m_nextModelId;
};


} // namespace eagle


