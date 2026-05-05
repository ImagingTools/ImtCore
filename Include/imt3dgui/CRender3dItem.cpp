// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CRender3dItem.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtQml/qqml.h>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGRenderNode>
#include <rhi/qrhi.h>


namespace
{


// Unit cube: 24 vertices (4 per face × 6 faces), 36 indices.
// Canonical 9-float format: position.xyz, normal.xyz, color.rgb — stride 36 bytes.
// Side length = 1.0 (from -0.5 to +0.5).
struct CubeData
{
	static constexpr int s_vertexCount = 24;
	static constexpr int s_indexCount = 36;
	static constexpr int s_stride = 9 * sizeof(float);

	float vertices[s_vertexCount * 9]{};
	uint32_t indices[s_indexCount]{};

	CubeData()
	{
		// 8 base corner positions
		const float c[8][3] = {
			{ -0.5f, -0.5f, -0.5f },
			{  0.5f, -0.5f, -0.5f },
			{  0.5f,  0.5f, -0.5f },
			{ -0.5f,  0.5f, -0.5f },
			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f },
			{ -0.5f,  0.5f,  0.5f },
		};

		struct FaceDesc { float nx, ny, nz; int corners[4]; };
		const FaceDesc faces[6] = {
			{  1, 0, 0, { 1, 5, 6, 2 } },
			{ -1, 0, 0, { 4, 0, 3, 7 } },
			{  0, 1, 0, { 3, 2, 6, 7 } },
			{  0,-1, 0, { 4, 5, 1, 0 } },
			{  0, 0, 1, { 5, 4, 7, 6 } },
			{  0, 0,-1, { 0, 1, 2, 3 } },
		};

		int vOff = 0;
		int iOff = 0;
		for (int f = 0; f < 6; ++f){
			const FaceDesc& fd = faces[f];
			const int baseVertex = f * 4;
			for (int v = 0; v < 4; ++v){
				const float* p = c[fd.corners[v]];
				float* dst = &vertices[vOff];
				dst[0] = p[0]; dst[1] = p[1]; dst[2] = p[2];
				dst[3] = fd.nx; dst[4] = fd.ny; dst[5] = fd.nz;
				dst[6] = 1.0f; dst[7] = 1.0f; dst[8] = 1.0f;
				vOff += 9;
			}
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 0);
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 1);
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 2);
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 0);
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 2);
			indices[iOff++] = static_cast<uint32_t>(baseVertex + 3);
		}
	}
};


} // namespace


namespace imt3dgui
{


// ──────────────────────────────────────────────────────────────────
// CRender3dNode — QSGRenderNode that issues RHI draw commands inline
// ──────────────────────────────────────────────────────────────────

class CRender3dNode: public QSGRenderNode
{
public:
	explicit CRender3dNode(CRender3dItem* itemPtr)
		:m_itemPtr(itemPtr)
	{
	}

	~CRender3dNode() override
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

		if (!m_initialized){
			InitResources(rhi, state);
		}

		if (!m_initialized){
			return;
		}

		// Build matrices
		QMatrix4x4 viewMatrix;
		QMatrix4x4 projMatrix;
		QVector3D camPos;
		BuildMatrices(viewMatrix, projMatrix, camPos);

		// Upload UBO data
		QRhiResourceUpdateBatch* batch = rhi->nextResourceUpdateBatch();

		// GlobalUBO
		GlobalUboData globalData{};
		FillGlobal(viewMatrix, projMatrix, camPos, globalData);
		batch->updateDynamicBuffer(m_globalUbo, 0, sizeof(GlobalUboData), &globalData);

		// DrawUBO
		DrawUboData drawData{};
		FillDraw(drawData);
		batch->updateDynamicBuffer(m_drawUbo, 0, sizeof(DrawUboData), &drawData);

		// Upload cube vertex/index data if first time
		if (m_needsUpload){
			CubeData cube;
			batch->updateDynamicBuffer(
						m_vertexBuffer, 0,
						sizeof(cube.vertices), cube.vertices);
			batch->updateDynamicBuffer(
						m_indexBuffer, 0,
						sizeof(cube.indices), cube.indices);
			m_needsUpload = false;
		}

		commandBuffer()->resourceUpdate(batch);
	}

	void render(const RenderState* state) override
	{
		if (!m_initialized){
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

		cb->setGraphicsPipeline(m_pipeline);
		cb->setShaderResources(m_srb);

		const QRhiCommandBuffer::VertexInput vbufBinding{ m_vertexBuffer, 0 };
		cb->setVertexInput(0, 1, &vbufBinding, m_indexBuffer, 0,
						   QRhiCommandBuffer::IndexUInt32);

		cb->drawIndexed(CubeData::s_indexCount);
	}

	void releaseResources() override
	{
		ReleaseResources();
	}

private:
	// std140 layout matching CRhiRenderBackend shaders (176 bytes)
	struct GlobalUboData
	{
		float viewMatrix[16];
		float projMatrix[16];
		float viewPosition[4];
		float lightPosition[4];
		float lightColor[4];
	};

	// std140 layout matching CRhiRenderBackend shaders (96 bytes)
	struct DrawUboData
	{
		float modelMatrix[16];
		float itemColor[4];
		qint32 colorMode;
		qint32 useNormals;
		qint32 usePointSize;
		float pointSize;
	};

	void InitResources(QRhi* rhi, const RenderState* state)
	{
		// Load pre-compiled QSB shaders (same ones CRhiRenderBackend uses)
		const QString vertPath = QStringLiteral(":/RhiShaders/vshader_rhi.vert.qsb");
		const QString fragPath = QStringLiteral(":/RhiShaders/fshader_rhi.frag.qsb");
		m_vertexShader = LoadShader(vertPath);
		m_fragmentShader = LoadShader(fragPath);
		if (!m_vertexShader.isValid() || !m_fragmentShader.isValid()){
			qDebug() << "CRender3dNode: failed to load RHI shaders —"
					 << "vertex:" << vertPath << (m_vertexShader.isValid() ? "ok" : "MISSING")
					 << "fragment:" << fragPath << (m_fragmentShader.isValid() ? "ok" : "MISSING");
			return;
		}

		// Vertex buffer (canonical 9-float format)
		m_vertexBuffer = rhi->newBuffer(
					QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer,
					CubeData::s_vertexCount * CubeData::s_stride);
		if (!m_vertexBuffer->create()){
			return;
		}

		// Index buffer
		m_indexBuffer = rhi->newBuffer(
					QRhiBuffer::Dynamic, QRhiBuffer::IndexBuffer,
					CubeData::s_indexCount * sizeof(uint32_t));
		if (!m_indexBuffer->create()){
			return;
		}

		// GlobalUBO
		m_globalUbo = rhi->newBuffer(
					QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer,
					sizeof(GlobalUboData));
		if (!m_globalUbo->create()){
			return;
		}

		// DrawUBO
		m_drawUbo = rhi->newBuffer(
					QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer,
					sizeof(DrawUboData));
		if (!m_drawUbo->create()){
			return;
		}

		// SRB: binding 0 = GlobalUBO, binding 1 = DrawUBO
		m_srb = rhi->newShaderResourceBindings();
		m_srb->setBindings({
			QRhiShaderResourceBinding::uniformBuffer(
				0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
				m_globalUbo),
			QRhiShaderResourceBinding::uniformBuffer(
				1, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
				m_drawUbo),
		});
		if (!m_srb->create()){
			return;
		}

		// Pipeline
		m_pipeline = rhi->newGraphicsPipeline();
		m_pipeline->setShaderStages({
			{ QRhiShaderStage::Vertex, m_vertexShader },
			{ QRhiShaderStage::Fragment, m_fragmentShader },
		});

		// Vertex input layout: canonical 9-float interleaved
		QRhiVertexInputLayout inputLayout;
		inputLayout.setBindings({
			{ CubeData::s_stride }
		});
		inputLayout.setAttributes({
			{ 0, 0, QRhiVertexInputAttribute::Float3, 0 },                    // position
			{ 0, 1, QRhiVertexInputAttribute::Float3, 3 * sizeof(float) },    // normal
			{ 0, 2, QRhiVertexInputAttribute::Float3, 6 * sizeof(float) },    // color
		});
		m_pipeline->setVertexInputLayout(inputLayout);

		m_pipeline->setShaderResourceBindings(m_srb);
		m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());

		m_pipeline->setTopology(QRhiGraphicsPipeline::Triangles);
		m_pipeline->setDepthTest(true);
		m_pipeline->setDepthWrite(true);
		m_pipeline->setCullMode(QRhiGraphicsPipeline::Back);
		m_pipeline->setFrontFace(QRhiGraphicsPipeline::CCW);

		if (!m_pipeline->create()){
			return;
		}

		m_needsUpload = true;
		m_initialized = true;
	}

	void ReleaseResources()
	{
		delete m_pipeline; m_pipeline = nullptr;
		delete m_srb; m_srb = nullptr;
		delete m_globalUbo; m_globalUbo = nullptr;
		delete m_drawUbo; m_drawUbo = nullptr;
		delete m_vertexBuffer; m_vertexBuffer = nullptr;
		delete m_indexBuffer; m_indexBuffer = nullptr;
		m_initialized = false;
	}

	static QShader LoadShader(const QString& resourcePath)
	{
		QFile f(resourcePath);
		if (!f.open(QIODevice::ReadOnly)){
			return QShader();
		}
		return QShader::fromSerialized(f.readAll());
	}

	static constexpr float s_degToRad = static_cast<float>(M_PI) / 180.0f;

	void BuildMatrices(QMatrix4x4& viewOut, QMatrix4x4& projOut, QVector3D& camPosOut) const
	{
		const float dist = static_cast<float>(m_itemPtr->GetCameraDistance());
		const float rx = static_cast<float>(m_itemPtr->GetRotationX()) * s_degToRad;
		const float ry = static_cast<float>(m_itemPtr->GetRotationY()) * s_degToRad;

		camPosOut = QVector3D(
					dist * std::cos(rx) * std::sin(ry),
					dist * std::sin(rx),
					dist * std::cos(rx) * std::cos(ry));

		viewOut.setToIdentity();
		viewOut.lookAt(camPosOut, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

		const float w = static_cast<float>(m_itemPtr->width());
		const float h = static_cast<float>(m_itemPtr->height());
		const float aspect = (h > 0) ? w / h : 1.0f;
		projOut.setToIdentity();
		projOut.perspective(45.0f, aspect, 0.1f, 100.0f);
	}

	static void CopyMatrix(const QMatrix4x4& src, float* dst)
	{
		const float* data = src.constData();
		for (int i = 0; i < 16; ++i){
			dst[i] = data[i];
		}
	}

	void FillGlobal(const QMatrix4x4& view, const QMatrix4x4& proj,
					 const QVector3D& camPos, GlobalUboData& out) const
	{
		CopyMatrix(view, out.viewMatrix);
		CopyMatrix(proj, out.projMatrix);
		out.viewPosition[0] = camPos.x();
		out.viewPosition[1] = camPos.y();
		out.viewPosition[2] = camPos.z();
		out.viewPosition[3] = 0.0f;
		out.lightPosition[0] = 2.0f;
		out.lightPosition[1] = 3.0f;
		out.lightPosition[2] = 4.0f;
		out.lightPosition[3] = 0.0f;
		out.lightColor[0] = 1.0f;
		out.lightColor[1] = 1.0f;
		out.lightColor[2] = 1.0f;
		out.lightColor[3] = 0.0f;
	}

	void FillDraw(DrawUboData& out) const
	{
		// Identity model matrix
		QMatrix4x4 identity;
		identity.setToIdentity();
		CopyMatrix(identity, out.modelMatrix);

		const QColor& cc = m_itemPtr->GetCubeColor();
		out.itemColor[0] = static_cast<float>(cc.redF());
		out.itemColor[1] = static_cast<float>(cc.greenF());
		out.itemColor[2] = static_cast<float>(cc.blueF());
		out.itemColor[3] = 0.0f;
		out.colorMode = 1; // CM_SOLID
		out.useNormals = 1;
		out.usePointSize = 0;
		out.pointSize = 1.0f;
	}

	CRender3dItem* m_itemPtr = nullptr;

	QShader m_vertexShader;
	QShader m_fragmentShader;

	QRhiBuffer* m_vertexBuffer = nullptr;
	QRhiBuffer* m_indexBuffer = nullptr;
	QRhiBuffer* m_globalUbo = nullptr;
	QRhiBuffer* m_drawUbo = nullptr;
	QRhiShaderResourceBindings* m_srb = nullptr;
	QRhiGraphicsPipeline* m_pipeline = nullptr;

	bool m_initialized = false;
	bool m_needsUpload = true;
};


// ──────────────────────────────────────────────────────────────────
// CRender3dItem — QQuickItem implementation
// ──────────────────────────────────────────────────────────────────

CRender3dItem::CRender3dItem(QQuickItem* parentPtr)
	:QQuickItem(parentPtr),
	m_cubeColor("#e94560"),
	m_cameraDistance(3.0),
	m_rotationX(25.0),
	m_rotationY(45.0)
{
	setFlag(ItemHasContents, true);
}


CRender3dItem::~CRender3dItem()
{
}


void CRender3dItem::RegisterQmlType()
{
	qmlRegisterType<CRender3dItem>("imt3dgui", 1, 0, "Render3DView");
}


QColor CRender3dItem::GetCubeColor() const
{
	return m_cubeColor;
}


void CRender3dItem::SetCubeColor(const QColor& color)
{
	if (m_cubeColor != color){
		m_cubeColor = color;
		emit CubeColorChanged();
		update();
	}
}


qreal CRender3dItem::GetCameraDistance() const
{
	return m_cameraDistance;
}


void CRender3dItem::SetCameraDistance(qreal distance)
{
	if (!qFuzzyCompare(m_cameraDistance, distance)){
		m_cameraDistance = distance;
		emit CameraDistanceChanged();
		update();
	}
}


qreal CRender3dItem::GetRotationX() const
{
	return m_rotationX;
}


void CRender3dItem::SetRotationX(qreal angle)
{
	if (!qFuzzyCompare(m_rotationX, angle)){
		m_rotationX = angle;
		emit RotationXChanged();
		update();
	}
}


qreal CRender3dItem::GetRotationY() const
{
	return m_rotationY;
}


void CRender3dItem::SetRotationY(qreal angle)
{
	if (!qFuzzyCompare(m_rotationY, angle)){
		m_rotationY = angle;
		emit RotationYChanged();
		update();
	}
}


// reimplemented (QQuickItem)

QSGNode* CRender3dItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* /*data*/)
{
	CRender3dNode* node = static_cast<CRender3dNode*>(oldNode);

	if (width() <= 0 || height() <= 0){
		delete node;
		return nullptr;
	}

	if (!node){
		node = new CRender3dNode(this);
	}

	node->markDirty(QSGNode::DirtyMaterial);
	return node;
}


} // namespace imt3dgui
