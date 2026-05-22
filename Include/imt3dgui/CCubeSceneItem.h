// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

// ImtCore includes
#include <imt3dview/DrawCommand.h>
#include <imt3dview/IRenderBackend.h>
#include <imt3dview/IRenderResource.h>
#include <imt3dview/IScene3dItem.h>
#include <imt3dview/Material.h>
#include <imt3dview/VertexLayout.h>


namespace imt3dgui
{


/**
	Lightweight IScene3dItem that renders a unit cube (-0.5 … +0.5).

	Designed for demos and tests. The shape generates its own
	vertex/index data (24 vertices, 36 indices) and submits a single
	DrawCommand when Render() is called.

	### Usage
	@code
	CCubeSceneItem cube;
	cube.SetColor(QVector3D(0.91f, 0.27f, 0.38f));
	scene.AddShapeToScene(&cube);
	@endcode
*/
class CCubeSceneItem: virtual public imt3dview::IScene3dItem
{
public:
	CCubeSceneItem();
	~CCubeSceneItem() override;

	void SetColor(const QVector3D& color);
	QVector3D GetColor() const;

	// reimplemented (imt3dview::IScene3dItem)
	bool IsValid() const override;
	void SetCamera(const imt3dview::IScene3dCamera* cameraPtr) override;
	void SetProjection(const QMatrix4x4& projection) override;
	void SetViewPort(const QRect& viewPort) override;
	const QVector3D& GetPosition() const override;
	void SetPosition(const QVector3D& position) override;
	const QQuaternion& GetRotation() const override;
	void SetRotation(const QQuaternion& rotation) override;
	float GetScale() const override;
	void SetScale(float scale) override;

	// reimplemented (imt3dview::IDrawable)
	void OnAttachBackend(imt3dview::IRenderBackend* backendPtr) override;
	void Render(imt3dview::IRenderBackend& backend) override;
	void DrawOverlay(QPainter& painter) override;

private:
	static constexpr int s_vertexCount = 24;
	static constexpr int s_indexCount = 36;
	static constexpr int s_floatsPerVertex = 9; // pos(3) + normal(3) + color(3)
	static constexpr int s_stride = s_floatsPerVertex * static_cast<int>(sizeof(float));

	void CreateGeometry();
	QMatrix4x4 GetModelMatrix() const;

	imt3dview::IRenderBackend* m_backendPtr = nullptr;
	imt3dview::IRenderResourcePtr m_geometry;

	QVector3D m_color{0.91f, 0.27f, 0.38f};
	QVector3D m_position;
	QQuaternion m_rotation;
	float m_scale = 1.0f;
	bool m_geometryDirty = true;
};


} // namespace imt3dgui
