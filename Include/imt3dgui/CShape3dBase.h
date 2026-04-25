// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QVector>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

// ACF includes
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imt3d/IPointsBasedObject.h>
#include <imt3dgui/IShape3d.h>
#include <imt3dview/IRenderBackend.h>
#include <imt3dview/IRenderResource.h>
#include <imt3dview/DrawCommand.h>
#include <imt3dview/Material.h>


namespace imt3dgui
{


/**
	Base implementation for shapes built on top of imt3dview::IRenderBackend
	(not thread-safe). Subclasses provide the backend-neutral primitive type
	and material via GetPrimitiveType()/FillMaterial(), plus geometry updates
	via UpdateShapeGeometry().
*/
class CShape3dBase: public imod::CSingleModelObserverBase, virtual public IShape3d
{
public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef uint32_t IndexType;

	CShape3dBase();
	virtual ~CShape3dBase();

	int FindVertex(const QPoint& point, bool limitDistance, QVector3D* positionPtr = nullptr) const;

	// reimplement (imt3dgui::IShape3d)
	virtual bool IsValid() const override;
	virtual bool IsVisible() const override;
	virtual void SetVisible(bool isVisible) override;

	// reimplement (imt3dview::IScene3dItem)
	virtual void SetCamera(const imt3dview::IScene3dCamera* cameraPtr) override;
	virtual void SetProjection(const QMatrix4x4& projection) override;
	virtual void SetViewPort(const QRect& viewPort) override;

	virtual const QVector3D& GetPosition() const override;
	virtual void SetPosition(const QVector3D& position) override;

	virtual const QQuaternion& GetRotation() const override;
	virtual void SetRotation(const QQuaternion& rotation) override;

	virtual float GetScale() const override;
	virtual void SetScale(float scale) override;

	// reimplement (imt3dview::IDrawable)
	virtual void OnAttachBackend(imt3dview::IRenderBackend* backendPtr) override;
	virtual void Render(imt3dview::IRenderBackend& backend) override;
	virtual void DrawOverlay(QPainter& /*painter*/) override;

protected:
	// reimplement (IShape3d)
	virtual QVector3D GetColor() const override { return QVector3D(); }

protected:
	// to be implemented in descendants
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) = 0;

	/**
		Backend-neutral primitive type for this shape (Triangles, Lines, Points, ...).
	*/
	virtual imt3dview::PrimitiveType GetPrimitiveType() const = 0;

	/**
		Fill in shape-specific material/uniform settings (line width, point size,
		color mode, lighting, ...). The base implementation seeds default colorMode
		and the GetColor()/HasNormals defaults; subclasses may further customize.
	*/
	virtual void FillMaterial(imt3dview::Material& material) const;

protected:
	void UpdateGeometry(const istd::IChangeable::ChangeSet& changeSet);
	QMatrix4x4 GetModelMatrix() const;
	QPoint ModelToWindow(const QVector3D& modelCoordinate) const;
	QVector3D WindowToModel(const QPoint& windowCoordinate, float z) const;

	void CreateGeometry();
	void RefreshGeometry();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	const imt3d::IPointsBasedObject* m_pointsDataPtr;
	QVector<IndexType> m_indices;

	QVector3D m_position;
	QQuaternion m_rotation;
	float m_scale;

	imt3dview::IRenderBackend* m_backendPtr;
	imt3dview::IRenderResourcePtr m_geometry;
	imt3d::IPointsBasedObject::PointFormat m_geometryFormat;

	const imt3dview::IScene3dCamera* m_cameraPtr;
	QRect m_viewPort;
	QMatrix4x4 m_projection;

	bool m_isVisible;
};

} // namespace imt3dgui
