// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CRhiRenderBackend.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QMatrix4x4>

// ImtCore includes
#include <imt3dgui/CRhiGeometryResource.h>
#include <imt3dview/SceneState.h>


namespace imt3dgui
{


// public methods

CRhiRenderBackend::CRhiRenderBackend()
{
}


CRhiRenderBackend::~CRhiRenderBackend()
{
	Shutdown();
}


bool CRhiRenderBackend::PipelineKey::operator<(const PipelineKey& o) const
{
	if (topology != o.topology){
		return topology < o.topology;
	}
	return (!cullFace && o.cullFace);
}


void CRhiRenderBackend::SetRhiContext(
			QRhi* rhi,
			QRhiRenderTarget* renderTarget,
			QRhiCommandBuffer* commandBuffer,
			QRhiRenderPassDescriptor* renderPassDescriptor)
{
	m_rhi = rhi;
	m_renderTarget = renderTarget;
	m_commandBuffer = commandBuffer;
	m_rpDesc = renderPassDescriptor;
}


bool CRhiRenderBackend::Initialize()
{
	if (m_initialized){
		return true;
	}
	if (m_rhi == nullptr){
		qDebug() << "CRhiRenderBackend::Initialize: no QRhi set";
		return false;
	}

	// Load pre-compiled QSB shaders (produced by QMAKE_SHADER_FILES in the QMake build).
	m_vertexShader = LoadShader(":/RhiShaders/vshader_rhi.vert.qsb");
	m_fragmentShader = LoadShader(":/RhiShaders/fshader_rhi.frag.qsb");
	if (!m_vertexShader.isValid() || !m_fragmentShader.isValid()){
		qDebug() << "CRhiRenderBackend::Initialize: failed to load RHI shaders from Qt resources";
		return false;
	}

	// Aligned draw-UBO slot size (≥ sizeof(DrawUboData), multiple of ubufAlignment).
	m_alignedDrawUboSize = static_cast<int>(
				m_rhi->ubufAligned(static_cast<quint32>(sizeof(DrawUboData))));

	// GlobalUBO — one per frame.
	m_globalUbo = m_rhi->newBuffer(
				QRhiBuffer::Dynamic,
				QRhiBuffer::UniformBuffer,
				sizeof(GlobalUboData));
	if (!m_globalUbo->create()){
		qDebug() << "CRhiRenderBackend::Initialize: failed to create GlobalUBO";
		Shutdown();
		return false;
	}

	// DrawUBO — pre-allocated for s_maxDrawsPerFrame slots.
	m_drawUbo = m_rhi->newBuffer(
				QRhiBuffer::Dynamic,
				QRhiBuffer::UniformBuffer,
				s_maxDrawsPerFrame * m_alignedDrawUboSize);
	if (!m_drawUbo->create()){
		qDebug() << "CRhiRenderBackend::Initialize: failed to create DrawUBO";
		Shutdown();
		return false;
	}

	// Shader resource bindings (layout used for pipeline creation).
	m_srb = BuildSrb();
	if (m_srb == nullptr){
		qDebug() << "CRhiRenderBackend::Initialize: failed to create shader resource bindings";
		Shutdown();
		return false;
	}

	m_initialized = true;
	return true;
}


void CRhiRenderBackend::Shutdown()
{
	// Destroy pipelines first — they reference m_srb and m_rpDesc.
	for (auto& [key, pipeline] : m_pipelines){
		delete pipeline;
	}
	m_pipelines.clear();

	delete m_srb;
	m_srb = nullptr;
	delete m_globalUbo;
	m_globalUbo = nullptr;
	delete m_drawUbo;
	m_drawUbo = nullptr;

	m_pendingDraws.clear();
	m_pendingUploads.clear();
	m_vertexShader = QShader();
	m_fragmentShader = QShader();

	m_initialized = false;
}


void CRhiRenderBackend::BeginFrame(const imt3dview::SceneState& sceneState)
{
	m_sceneState = sceneState;
	m_pendingDraws.clear();
}


QRhiResourceUpdateBatch* CRhiRenderBackend::FlushPendingUpdates()
{
	if (!m_initialized || m_rhi == nullptr){
		return nullptr;
	}

	const int drawCount = static_cast<int>(m_pendingDraws.size());
	if (drawCount > s_maxDrawsPerFrame){
		qDebug() << "CRhiRenderBackend::FlushPendingUpdates: draw count" << drawCount
		         << "exceeds s_maxDrawsPerFrame =" << s_maxDrawsPerFrame
		         << "; excess draws will be skipped";
	}
	const int effectiveDrawCount = qMin(drawCount, s_maxDrawsPerFrame);

	QRhiResourceUpdateBatch* batch = m_rhi->nextResourceUpdateBatch();

	// 1. Flush pending geometry uploads.
	for (const PendingUpload& upload : m_pendingUploads){
		if (!upload.geometry || !upload.geometry->IsCreated()){
			continue;
		}
		batch->updateDynamicBuffer(
					upload.geometry->GetVertexBuffer(),
					0,
					upload.vertexData.size(),
					upload.vertexData.constData());

		if (!upload.vertexOnly && !upload.indexData.isEmpty()){
			batch->updateDynamicBuffer(
						upload.geometry->GetIndexBuffer(),
						0,
						upload.indexData.size(),
						upload.indexData.constData());
		}
	}
	m_pendingUploads.clear();

	// 2. Fill GlobalUBO.
	GlobalUboData globalData{};
	FillGlobalUbo(m_sceneState, globalData);
	batch->updateDynamicBuffer(m_globalUbo, 0, sizeof(GlobalUboData), &globalData);

	// 3. Fill per-draw DrawUBO slots.
	for (int i = 0; i < effectiveDrawCount; ++i){
		DrawUboData drawData{};
		FillDrawUbo(m_pendingDraws[i], drawData);
		batch->updateDynamicBuffer(
					m_drawUbo,
					i * m_alignedDrawUboSize,
					sizeof(DrawUboData),
					&drawData);
	}

	return batch;
}


void CRhiRenderBackend::IssuePendingDrawCalls()
{
	if (!m_initialized || m_commandBuffer == nullptr){
		return;
	}

	const int drawCount = static_cast<int>(m_pendingDraws.size());
	const int effectiveDrawCount = qMin(drawCount, s_maxDrawsPerFrame);

	const bool cullFace =
		m_sceneState.renderHints & imt3dview::SceneState::RH_CULLFACE;

	for (int i = 0; i < effectiveDrawCount; ++i){
		const PendingDraw& pd = m_pendingDraws[i];

		auto* geometry = dynamic_cast<CRhiGeometryResource*>(pd.command.geometry.get());
		if (geometry == nullptr || !geometry->IsCreated()){
			continue;
		}

		const int indexCount =
			pd.command.indexCount > 0 ? pd.command.indexCount : geometry->GetIndexCount();
		if (indexCount <= 0){
			continue;
		}

		const PipelineKey key{
			ToRhiTopology(pd.command.primitive),
			cullFace
		};
		QRhiGraphicsPipeline* pipeline = GetOrCreatePipeline(key);
		if (pipeline == nullptr){
			continue;
		}

		m_commandBuffer->setGraphicsPipeline(pipeline);

		const quint32 dynamicOffset = static_cast<quint32>(i * m_alignedDrawUboSize);
		m_commandBuffer->setShaderResources(m_srb, 1, &dynamicOffset);

		const QRhiCommandBuffer::VertexInput vbufBinding{
			geometry->GetVertexBuffer(), 0
		};
		m_commandBuffer->setVertexInput(
					0, 1, &vbufBinding,
					geometry->GetIndexBuffer(), 0,
					QRhiCommandBuffer::IndexUInt32);

		m_commandBuffer->drawIndexed(
					indexCount,
					/*instanceCount*/ 1,
					pd.command.indexOffset);
	}

	m_pendingDraws.clear();
}


void CRhiRenderBackend::EndFrame()
{
	if (!m_initialized || m_commandBuffer == nullptr || m_renderTarget == nullptr){
		return;
	}

	QRhiResourceUpdateBatch* batch = FlushPendingUpdates();
	if (batch == nullptr){
		return;
	}

	// Open the render pass — this atomically applies the batch before any GPU work.
	const QColor& cc = m_sceneState.clearColor;
	m_commandBuffer->beginPass(
				m_renderTarget,
				cc,
				{ 1.0f, 0 },
				batch);

	// Set the full-widget viewport once (shapes don't adjust it individually).
	const QRect& vp = m_sceneState.viewport;
	m_commandBuffer->setViewport(QRhiViewport(
				static_cast<float>(vp.x()),
				static_cast<float>(vp.y()),
				static_cast<float>(vp.width()),
				static_cast<float>(vp.height())));

	IssuePendingDrawCalls();

	m_commandBuffer->endPass();
}


imt3dview::IRenderResourcePtr CRhiRenderBackend::CreateGeometry(
			const imt3dview::VertexLayout& layout)
{
	if (m_rhi == nullptr){
		return {};
	}
	return std::make_shared<CRhiGeometryResource>(m_rhi, layout);
}


void CRhiRenderBackend::UpdateGeometry(
			imt3dview::IRenderResource& resource,
			const void* vertexData,
			size_t vertexBytes,
			const uint32_t* indices,
			size_t indexCount)
{
	auto* geometry = dynamic_cast<CRhiGeometryResource*>(&resource);
	if (geometry == nullptr || vertexData == nullptr || indices == nullptr){
		return;
	}

	const imt3dview::VertexLayout& layout = geometry->GetSourceLayout();
	const int vertexCount =
		(layout.stride > 0)
			? static_cast<int>(vertexBytes) / layout.stride
			: 0;

	if (!geometry->Create(vertexCount, static_cast<int>(indexCount))){
		qDebug() << "CRhiRenderBackend::UpdateGeometry: failed to create GPU buffers";
		return;
	}
	geometry->SetIndexCount(static_cast<int>(indexCount));

	PendingUpload upload;
	upload.geometry = geometry;
	upload.vertexData = ConvertToCanonical(vertexData, vertexBytes, layout);
	upload.indexData = QByteArray(
				reinterpret_cast<const char*>(indices),
				static_cast<qsizetype>(indexCount * sizeof(quint32)));
	upload.vertexOnly = false;
	m_pendingUploads.push_back(std::move(upload));
}


void CRhiRenderBackend::RefreshVertices(
			imt3dview::IRenderResource& resource,
			const void* vertexData,
			size_t vertexBytes)
{
	auto* geometry = dynamic_cast<CRhiGeometryResource*>(&resource);
	if (geometry == nullptr || !geometry->IsCreated() || vertexData == nullptr){
		return;
	}

	PendingUpload upload;
	upload.geometry = geometry;
	upload.vertexData = ConvertToCanonical(
				vertexData, vertexBytes, geometry->GetSourceLayout());
	upload.vertexOnly = true;
	m_pendingUploads.push_back(std::move(upload));
}


void CRhiRenderBackend::Draw(const imt3dview::DrawCommand& command)
{
	if (!m_initialized){
		return;
	}
	m_pendingDraws.push_back({ command });
}


void CRhiRenderBackend::DestroyResource(imt3dview::IRenderResource& resource)
{
	auto* geometry = dynamic_cast<CRhiGeometryResource*>(&resource);
	if (geometry != nullptr){
		geometry->Destroy();
	}
}


// private methods

QByteArray CRhiRenderBackend::ConvertToCanonical(
			const void* src,
			size_t srcBytes,
			const imt3dview::VertexLayout& layout)
{
	if (layout.stride <= 0 || src == nullptr || srcBytes == 0){
		return {};
	}

	const int vertexCount = static_cast<int>(srcBytes) / layout.stride;
	QByteArray dst(
				vertexCount * CRhiGeometryResource::s_canonicalStride,
				Qt::Uninitialized);

	const char* srcPtr = static_cast<const char*>(src);
	float* dstFloats = reinterpret_cast<float*>(dst.data());

	// Locate attribute offsets within the source layout.
	int posOffset = -1;
	int normalOffset = -1;
	int colorOffset = -1;

	for (const imt3dview::VertexAttribute& attr : layout.attributes){
		switch (attr.attribute){
		case imt3dview::VA_POSITION: posOffset = attr.offset; break;
		case imt3dview::VA_NORMAL:   normalOffset = attr.offset; break;
		case imt3dview::VA_COLOR:    colorOffset = attr.offset; break;
		}
	}

	for (int v = 0; v < vertexCount; ++v){
		const char* vSrc = srcPtr + v * layout.stride;
		float*      vDst = dstFloats + v * 9;

		// Position — always expected to be present.
		if (posOffset >= 0){
			const float* p = reinterpret_cast<const float*>(vSrc + posOffset);
			vDst[0] = p[0]; vDst[1] = p[1]; vDst[2] = p[2];
		}
		else{
			vDst[0] = vDst[1] = vDst[2] = 0.0f;
		}

		// Normal — default: (0, 0, 1) = outward-facing along +Z.
		if (normalOffset >= 0){
			const float* n = reinterpret_cast<const float*>(vSrc + normalOffset);
			vDst[3] = n[0]; vDst[4] = n[1]; vDst[5] = n[2];
		}
		else{
			vDst[3] = 0.0f; vDst[4] = 0.0f; vDst[5] = 1.0f;
		}

		// Colour — default: (1, 1, 1) = white.
		if (colorOffset >= 0){
			const float* c = reinterpret_cast<const float*>(vSrc + colorOffset);
			vDst[6] = c[0]; vDst[7] = c[1]; vDst[8] = c[2];
		}
		else{
			vDst[6] = vDst[7] = vDst[8] = 1.0f;
		}
	}

	return dst;
}


QShader CRhiRenderBackend::LoadShader(const QString& resourcePath) const
{
	QFile f(resourcePath);
	if (!f.open(QIODevice::ReadOnly)){
		qDebug() << "CRhiRenderBackend::LoadShader: cannot open" << resourcePath;
		return {};
	}
	return QShader::fromSerialized(f.readAll());
}


QRhiShaderResourceBindings* CRhiRenderBackend::BuildSrb() const
{
	QRhiShaderResourceBindings* srb = m_rhi->newShaderResourceBindings();

	const QRhiShaderResourceBinding::StageFlags allStages =
		QRhiShaderResourceBinding::VertexStage |
		QRhiShaderResourceBinding::FragmentStage;

	srb->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(
			0,
			allStages,
			m_globalUbo),

		QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(
			1,
			allStages,
			m_drawUbo,
			static_cast<quint32>(sizeof(DrawUboData)))
	});

	if (!srb->create()){
		delete srb;
		return nullptr;
	}
	return srb;
}


QRhiGraphicsPipeline* CRhiRenderBackend::GetOrCreatePipeline(const PipelineKey& key)
{
	auto it = m_pipelines.find(key);
	if (it != m_pipelines.end()){
		return it->second;
	}

	if (m_rpDesc == nullptr || m_srb == nullptr){
		return nullptr;
	}

	QRhiGraphicsPipeline* pipeline = m_rhi->newGraphicsPipeline();

	pipeline->setShaderStages({
		{ QRhiShaderStage::Vertex,   m_vertexShader   },
		{ QRhiShaderStage::Fragment, m_fragmentShader  }
	});

	// Fixed canonical vertex input: (pos3f, normal3f, colour3f) = 36 bytes stride.
	QRhiVertexInputLayout vertexLayout;
	vertexLayout.setBindings({
		QRhiVertexInputBinding(CRhiGeometryResource::s_canonicalStride)
	});
	vertexLayout.setAttributes({
		QRhiVertexInputAttribute(0, 0, QRhiVertexInputAttribute::Float3, 0),
		QRhiVertexInputAttribute(0, 1, QRhiVertexInputAttribute::Float3, 3 * sizeof(float)),
		QRhiVertexInputAttribute(0, 2, QRhiVertexInputAttribute::Float3, 6 * sizeof(float))
	});
	pipeline->setVertexInputLayout(vertexLayout);

	pipeline->setTopology(key.topology);

	pipeline->setDepthTest(true);
	pipeline->setDepthWrite(true);
	pipeline->setDepthOp(QRhiGraphicsPipeline::LessOrEqual);

	if (key.cullFace){
		pipeline->setCullMode(QRhiGraphicsPipeline::Back);
		pipeline->setFrontFace(QRhiGraphicsPipeline::CCW);
	}
	else{
		pipeline->setCullMode(QRhiGraphicsPipeline::None);
	}

	pipeline->setShaderResourceBindings(m_srb);
	pipeline->setRenderPassDescriptor(m_rpDesc);

	if (!pipeline->create()){
		qDebug() << "CRhiRenderBackend::GetOrCreatePipeline: pipeline creation failed";
		delete pipeline;
		return nullptr;
	}

	m_pipelines[key] = pipeline;
	return pipeline;
}


void CRhiRenderBackend::FillGlobalUbo(
			const imt3dview::SceneState& state,
			GlobalUboData& out) const
{
	// QMatrix4x4 is stored column-major; memcpy gives the correct std140 representation.
	memcpy(out.viewMatrix, state.viewMatrix.constData(), sizeof(out.viewMatrix));
	memcpy(out.projMatrix, state.projectionMatrix.constData(), sizeof(out.projMatrix));

	out.viewPosition[0] = state.cameraPosition.x();
	out.viewPosition[1] = state.cameraPosition.y();
	out.viewPosition[2] = state.cameraPosition.z();
	out.viewPosition[3] = 0.0f;

	out.lightPosition[0] = state.lightPosition.x();
	out.lightPosition[1] = state.lightPosition.y();
	out.lightPosition[2] = state.lightPosition.z();
	out.lightPosition[3] = 0.0f;

	out.lightColor[0] = state.lightColor.x();
	out.lightColor[1] = state.lightColor.y();
	out.lightColor[2] = state.lightColor.z();
	out.lightColor[3] = 0.0f;
}


void CRhiRenderBackend::FillDrawUbo(
			const PendingDraw& pd,
			DrawUboData& out) const
{
	const imt3dview::DrawCommand& cmd = pd.command;

	memcpy(out.modelMatrix, cmd.modelMatrix.constData(), sizeof(out.modelMatrix));

	out.itemColor[0] = cmd.material.solidColor.x();
	out.itemColor[1] = cmd.material.solidColor.y();
	out.itemColor[2] = cmd.material.solidColor.z();
	out.itemColor[3] = 0.0f;

	out.colorMode =
		(cmd.material.colorMode == imt3dview::Material::CM_PER_VERTEX) ? 0 : 1;
	out.useNormals = cmd.material.useNormals ? 1 : 0;
	out.usePointSize = cmd.material.usePointSize ? 1 : 0;
	out.pointSize = cmd.material.pointSize;
}


QRhiGraphicsPipeline::Topology CRhiRenderBackend::ToRhiTopology(imt3dview::PrimitiveType pt)
{
	switch (pt){
	case imt3dview::PT_LINES:      return QRhiGraphicsPipeline::Lines;
	case imt3dview::PT_LINE_STRIP: return QRhiGraphicsPipeline::LineStrip;
	case imt3dview::PT_POINTS:     return QRhiGraphicsPipeline::Points;
	case imt3dview::PT_TRIANGLES:
	default:                       return QRhiGraphicsPipeline::Triangles;
	}
}


} // namespace imt3dgui
