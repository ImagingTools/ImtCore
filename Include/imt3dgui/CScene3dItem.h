// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtQuick/QQuickItem>
#include <QtGui/QMatrix4x4>

// ImtCore includes
#include <imt3dview/CScene3d.h>


namespace imt3dgui
{


/**
	Generic scene-based 3D viewport item for Qt Quick.

	CScene3dItem is a QQuickItem that renders the full contents of an
	imt3dview::IScene3d scene using a QSGRenderNode.  Unlike CRender3DItem
	(which renders only a hardcoded cube), CScene3dItem delegates to the
	IRenderBackend pipeline so that **any** IScene3dItem (meshes, point clouds,
	grids, axes, rulers, wires, …) added to the scene is rendered
	automatically — no per-shape QQuickItem subclass is required.

	The item provides a simple built-in orbit camera controlled by
	QML-bindable properties (cameraDistance, rotationX, rotationY).
	Shapes are added to the scene from C++ via GetScene()->AddShapeToScene().

	### Usage from QML

	@code
	import imt3dgui 1.0

	Scene3DView {
		anchors.fill: parent
		cameraDistance: 5.0
		rotationX: 30
		rotationY: 45
		backgroundColor: "black"
	}
	@endcode

	### Registration

	Call CScene3dItem::RegisterQmlType() once before loading QML, or use
	qmlRegisterType<CScene3dItem>("imt3dgui", 1, 0, "Scene3DView") directly.
*/
class CScene3dItem: public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(qreal cameraDistance READ GetCameraDistance WRITE SetCameraDistance NOTIFY CameraDistanceChanged)
	Q_PROPERTY(qreal rotationX READ GetRotationX WRITE SetRotationX NOTIFY RotationXChanged)
	Q_PROPERTY(qreal rotationY READ GetRotationY WRITE SetRotationY NOTIFY RotationYChanged)
	Q_PROPERTY(QColor backgroundColor READ GetBackgroundColor WRITE SetBackgroundColor NOTIFY BackgroundColorChanged)

public:
	explicit CScene3dItem(QQuickItem* parentPtr = nullptr);
	~CScene3dItem() override;

	static void RegisterQmlType();

	/**
		Access the 3D scene to add/remove shapes.
		Thread-safe only while the render thread is not active
		(i.e. call from the GUI thread before rendering starts, or from
		a signal connected with Qt::DirectConnection to the item).
	*/
	imt3dview::IScene3d* GetScene();

	qreal GetCameraDistance() const;
	void SetCameraDistance(qreal distance);

	qreal GetRotationX() const;
	void SetRotationX(qreal angle);

	qreal GetRotationY() const;
	void SetRotationY(qreal angle);

	QColor GetBackgroundColor() const;
	void SetBackgroundColor(const QColor& color);

signals:
	void CameraDistanceChanged();
	void RotationXChanged();
	void RotationYChanged();
	void BackgroundColorChanged();

protected:
	// reimplemented (QQuickItem)
	QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;

private:
	imt3dview::CScene3d m_scene;

	qreal m_cameraDistance = 3.0;
	qreal m_rotationX = 25.0;
	qreal m_rotationY = 45.0;
	QColor m_backgroundColor{Qt::black};
};


} // namespace imt3dgui
