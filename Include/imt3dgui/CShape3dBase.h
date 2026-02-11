// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore>

#if QT_VERSION < 0x060000
#include <QtGui/QOpenGLBuffer>
#else
#include <QtOpenGL/QOpenGLBuffer>
#endif

#include <QtGui/QOpenGLFunctions>

// ACF includes
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imt3dgui/IShape3d.h>
#include <imt3d/IPointsBasedObject.h>


class QOpenGLContext;


namespace imt3dgui
{


/**
	Base implementation for GL shapes (not thread-safe)
*/
class CShape3dBase: public imod::CSingleModelObserverBase, virtual public IShape3d
{
public:
	typedef imod::CSingleModelObserverBase BaseClass;

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
	virtual void SetContext(QOpenGLContext* contextPtr) override;
	virtual void DrawGl(QOpenGLShaderProgram& program) override;
	virtual void Draw(QPainter& /*painter*/) override;

protected:
	// reimplement (IShape3d)
	virtual QVector3D GetColor() const override { return QVector3D(); }

protected:
	// to be implemented in descendants
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) = 0;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) = 0;

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
	QVector<GLuint> m_indices;

	QVector3D m_position;
	QQuaternion m_rotation;
	float m_scale;

	QOpenGLContext* m_contextPtr;
	const imt3dview::IScene3dCamera* m_cameraPtr;
	QRect m_viewPort;
	QOpenGLBuffer m_vertexBuffer;
	QOpenGLBuffer m_indexBuffer;
	QMatrix4x4 m_projection;

#if QT_VERSION >= 0x060000
	mutable QRecursiveMutex m_bufferMutex;
#else
	mutable QMutex m_bufferMutex;
#endif

	bool m_isVisible;
};

} // namespace imt3dgui


