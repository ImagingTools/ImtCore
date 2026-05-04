// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtQuick/QQuickItem>
#include <QtGui/QMatrix4x4>


namespace imt3dgui
{


/**
	Pure-QML 3D viewport item.

	CRender3DItem is a QQuickItem that renders a 3D cube using a
	QSGRenderNode — exactly the same pattern Qt Quick 3D View3D uses in its
	"Inline" render mode.  The RenderNode issues RHI draw commands directly
	into the Qt Quick scene graph's existing render pass, achieving full GPU
	acceleration (Vulkan/Metal/D3D/OpenGL) without any QWidget dependency.

	The shaders and UBO layout are identical to those used by
	CRhiRenderBackend, ensuring visual consistency with the CRhiWidget path.

	### Usage from QML

	@code
	import imt3dgui 1.0

	Render3DView {
		anchors.fill: parent
		cubeColor: "#e94560"
		cameraDistance: 5.0
		rotationX: 30
		rotationY: 45
	}
	@endcode

	### Registration

	Call CRender3DItem::RegisterQmlType() once before loading QML, or use
	qmlRegisterType<CRender3DItem>("imt3dgui", 1, 0, "Render3DView") directly.

	### Thread safety

	All rendering happens on the Qt Quick render thread via the
	QSGRenderNode::render() callback.  No external synchronisation is required.
*/
class CRender3DItem: public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(QColor cubeColor READ GetCubeColor WRITE SetCubeColor NOTIFY CubeColorChanged)
	Q_PROPERTY(qreal cameraDistance READ GetCameraDistance WRITE SetCameraDistance NOTIFY CameraDistanceChanged)
	Q_PROPERTY(qreal rotationX READ GetRotationX WRITE SetRotationX NOTIFY RotationXChanged)
	Q_PROPERTY(qreal rotationY READ GetRotationY WRITE SetRotationY NOTIFY RotationYChanged)

public:
	explicit CRender3DItem(QQuickItem* parentPtr = nullptr);
	~CRender3DItem() override;

	static void RegisterQmlType();

	QColor GetCubeColor() const;
	void SetCubeColor(const QColor& color);

	qreal GetCameraDistance() const;
	void SetCameraDistance(qreal distance);

	qreal GetRotationX() const;
	void SetRotationX(qreal angle);

	qreal GetRotationY() const;
	void SetRotationY(qreal angle);

signals:
	void CubeColorChanged();
	void CameraDistanceChanged();
	void RotationXChanged();
	void RotationYChanged();

protected:
	QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;

private:
	QColor m_cubeColor;
	qreal m_cameraDistance;
	qreal m_rotationX;
	qreal m_rotationY;
};


} // namespace imt3dgui
