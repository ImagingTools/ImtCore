// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CScene3DItem.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QDebug>
#include <QtQml/qqml.h>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGRenderNode>
#include <rhi/qrhi.h>

// ImtCore includes
#include <imt3dgui/CRhiRenderBackend.h>
#include <imt3dview/SceneState.h>


namespace imt3dgui
{


// ──────────────────────────────────────────────────────────────────
// CScene3DNode — QSGRenderNode that renders the full IScene3d scene
// ──────────────────────────────────────────────────────────────────

class CScene3DNode: public QSGRenderNode
{
public:
	explicit CScene3DNode(CScene3DItem* itemPtr)
		:m_itemPtr(itemPtr)
	{
	}

	~CScene3DNode() override
	{
		ReleaseResources();
	}

	// QSGRenderNode API
	StateFlags changedStates() const override
	{
		return DepthState | StencilState | ScissorState | ColorState | BlendState
			| CullState | ViewportState | RenderTargetState;
	}

	RenderingFlags flags() const override
	{
		return BoundedRectRendering | DepthAwareRendering;
	}

	QRectF rect() const override
	{
		return QRectF(0, 0, m_itemPtr->width(), m_itemPtr->height());
	}

	void prepare(const RenderState* state) override
	{
		QRhi* rhi = state->rhi();
		if (!rhi){
			return;
		}

		// On the first call (or after device-lost), (re)initialize the backend.
		if (!m_backendReady){
			// Detach scene from a previous backend if any.
			m_scene->OnAttachBackend(nullptr);
			m_backend.Shutdown();

			m_backend.SetRhiContext(
						rhi,
						renderTarget(),
						commandBuffer(),
						renderTarget()->renderPassDescriptor());

			if (!m_backend.Initialize()){
				qDebug() << "CScene3DNode: backend initialization failed";
				return;
			}

			m_scene->OnAttachBackend(&m_backend);
			m_backendReady = true;
		}
		else{
			// Update the per-frame RHI context.
			m_backend.SetRhiContext(
						rhi,
						renderTarget(),
						commandBuffer(),
						renderTarget()->renderPassDescriptor());
		}

		// Keep scene matrices in sync.
		const QRectF r = rect();
		const QRect viewPort(
					static_cast<int>(r.x()),
					static_cast<int>(r.y()),
					static_cast<int>(r.width()),
					static_cast<int>(r.height()));
		m_scene->SetViewPort(viewPort);

		QMatrix4x4 projMatrix;
		projMatrix.setToIdentity();
		const float w = static_cast<float>(r.width());
		const float h = static_cast<float>(r.height());
		const float aspect = (h > 0) ? w / h : 1.0f;
		projMatrix.perspective(45.0f, aspect, 0.1f, 100.0f);
		m_scene->SetProjection(projMatrix);

		// Build scene state from item properties.
		imt3dview::SceneState sceneState = BuildSceneState(projMatrix);

		// Run the scene render pipeline: BeginFrame → scene.Render → FlushPendingUpdates.
		m_backend.BeginFrame(sceneState);
		m_scene->Render(m_backend);

		QRhiResourceUpdateBatch* batch = m_backend.FlushPendingUpdates();
		if (batch){
			commandBuffer()->resourceUpdate(batch);
		}
	}

	void render(const RenderState* state) override
	{
		if (!m_backendReady){
			return;
		}

		QRhiCommandBuffer* cb = commandBuffer();

		// Viewport
		const QRectF r = rect();
		cb->setViewport(QRhiViewport(
					static_cast<float>(r.x()),
					static_cast<float>(r.y()),
					static_cast<float>(r.width()),
					static_cast<float>(r.height())));

		// Scissor — clip to item rectangle
		if (state->scissorEnabled()){
			const QRect sc = state->scissorRect();
			cb->setScissor(QRhiScissor(sc.x(), sc.y(), sc.width(), sc.height()));
		}

		// Issue all queued draw calls inline.
		m_backend.IssuePendingDrawCalls();
	}

	void releaseResources() override
	{
		ReleaseResources();
	}

	void SetScene(imt3dview::CScene3d* scenePtr)
	{
		m_scene = scenePtr;
	}

private:
	void ReleaseResources()
	{
		if (m_backendReady && m_scene){
			m_scene->OnAttachBackend(nullptr);
		}
		m_backend.Shutdown();
		m_backendReady = false;
	}

	static constexpr float s_degToRad = static_cast<float>(M_PI) / 180.0f;

	imt3dview::SceneState BuildSceneState(const QMatrix4x4& projMatrix) const
	{
		imt3dview::SceneState state;

		const float dist = static_cast<float>(m_itemPtr->GetCameraDistance());
		const float rx = static_cast<float>(m_itemPtr->GetRotationX()) * s_degToRad;
		const float ry = static_cast<float>(m_itemPtr->GetRotationY()) * s_degToRad;

		const QVector3D camPos(
					dist * std::cos(rx) * std::sin(ry),
					dist * std::sin(rx),
					dist * std::cos(rx) * std::cos(ry));

		QMatrix4x4 viewMatrix;
		viewMatrix.setToIdentity();
		viewMatrix.lookAt(camPos, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

		state.viewMatrix = viewMatrix;
		state.projectionMatrix = projMatrix;
		state.cameraPosition = camPos;

		const QRectF r = rect();
		state.viewport = QRect(
					static_cast<int>(r.x()),
					static_cast<int>(r.y()),
					static_cast<int>(r.width()),
					static_cast<int>(r.height()));

		state.lightPosition = QVector3D(2.0f, 3.0f, 4.0f);
		state.lightColor = QVector3D(1.0f, 1.0f, 1.0f);
		state.clearColor = m_itemPtr->GetBackgroundColor();
		state.renderHints = imt3dview::SceneState::RH_CULLFACE;

		return state;
	}

	CScene3DItem* m_itemPtr = nullptr;
	imt3dview::CScene3d* m_scene = nullptr;
	CRhiRenderBackend m_backend;
	bool m_backendReady = false;
};


// ──────────────────────────────────────────────────────────────────
// CScene3DItem — QQuickItem implementation
// ──────────────────────────────────────────────────────────────────

CScene3DItem::CScene3DItem(QQuickItem* parentPtr)
	:QQuickItem(parentPtr),
	m_cameraDistance(3.0),
	m_rotationX(25.0),
	m_rotationY(45.0),
	m_backgroundColor(Qt::black)
{
	setFlag(ItemHasContents, true);
}


CScene3DItem::~CScene3DItem()
{
}


void CScene3DItem::RegisterQmlType()
{
	qmlRegisterType<CScene3DItem>("imt3dgui", 1, 0, "Scene3DView");
}


imt3dview::IScene3d* CScene3DItem::GetScene()
{
	return &m_scene;
}


qreal CScene3DItem::GetCameraDistance() const
{
	return m_cameraDistance;
}


void CScene3DItem::SetCameraDistance(qreal distance)
{
	if (!qFuzzyCompare(m_cameraDistance, distance)){
		m_cameraDistance = distance;
		emit CameraDistanceChanged();
		update();
	}
}


qreal CScene3DItem::GetRotationX() const
{
	return m_rotationX;
}


void CScene3DItem::SetRotationX(qreal angle)
{
	if (!qFuzzyCompare(m_rotationX, angle)){
		m_rotationX = angle;
		emit RotationXChanged();
		update();
	}
}


qreal CScene3DItem::GetRotationY() const
{
	return m_rotationY;
}


void CScene3DItem::SetRotationY(qreal angle)
{
	if (!qFuzzyCompare(m_rotationY, angle)){
		m_rotationY = angle;
		emit RotationYChanged();
		update();
	}
}


QColor CScene3DItem::GetBackgroundColor() const
{
	return m_backgroundColor;
}


void CScene3DItem::SetBackgroundColor(const QColor& color)
{
	if (m_backgroundColor != color){
		m_backgroundColor = color;
		emit BackgroundColorChanged();
		update();
	}
}


QSGNode* CScene3DItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* /*data*/)
{
	CScene3DNode* node = static_cast<CScene3DNode*>(oldNode);

	if (width() <= 0 || height() <= 0){
		delete node;
		return nullptr;
	}

	if (!node){
		node = new CScene3DNode(this);
		node->SetScene(&m_scene);
	}

	node->markDirty(QSGNode::DirtyMaterial);
	return node;
}


} // namespace imt3dgui
