// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <map>
#include <vector>

// Qt includes
#include <QtCore/QByteArray>

// ImtCore includes
#include <imt3dgui/QtRhiCompat.h>
#include <imt3dview/IRenderBackend.h>
#include <imt3dview/SceneState.h>
#include <imt3dview/DrawCommand.h>


namespace imt3dgui
{


/**
	Qt RHI implementation of imt3dview::IRenderBackend.

	### Two-phase render model
	The QRhi API requires that all UBO / buffer uploads happen via a
	QRhiResourceUpdateBatch that is submitted to QRhiCommandBuffer::beginPass().
	To satisfy this constraint without changing the IRenderBackend interface,
	CRhiRenderBackend uses a two-phase approach:

	  * BeginFrame() — stores the scene state and clears the pending-draw list.
	  * Draw()       — appends DrawCommand entries to the pending-draw list.
	  * EndFrame()   — builds a single resource-update batch (pending geometry
	                   uploads + GlobalUBO + per-draw DrawUBO slots), opens the
	                   render pass with that batch, issues all collected draw calls
	                   in one go, then closes the pass.

	### Canonical vertex format
	All vertex data is converted to a fixed 9-float interleaved format
	(position × 3, normal × 3, colour × 3, stride = 36 bytes) on upload so that
	a single QRhiVertexInputLayout applies to every shape, making it possible to
	keep a small, flat pipeline cache indexed only by primitive topology and
	face-culling mode.

	### Shader resource layout
	  Binding 0 — GlobalUBO  (per-frame, 176 bytes)
	  Binding 1 — DrawUBO    (per-draw, dynamic offset into a pre-allocated buffer)

	### Usage
	The host widget (CRhiWidget) must call SetRhiContext() with the current frame's
	QRhi, QRhiRenderTarget and QRhiCommandBuffer before calling BeginFrame().
	It must also call Initialize() after the first SetRhiContext() call and
	Shutdown() before destroying the widget.

	### Known limitations
	* Line width is fixed at 1 pixel (QRhi does not expose glLineWidth portably).
	* The 2D QPainter overlay (DrawOverlay) is not rendered by this backend —
	  it must be handled by a separate overlay widget placed on top of CRhiWidget.
*/
class CRhiRenderBackend: public imt3dview::IRenderBackend
{
public:
	// Maximum number of draw calls per frame.
	// Sized for typical 3D scenes; increase if more than 256 shapes are needed.
	static constexpr int s_maxDrawsPerFrame = 256;

	CRhiRenderBackend();
	~CRhiRenderBackend() override;

	/**
		Supply the per-frame RHI objects. Must be called before every BeginFrame().
		On the first call (or after a device-lost reinit) it also records the render-pass
		descriptor so that GetOrCreatePipeline() can build pipelines against it.
	*/
	void SetRhiContext(
				QRhi* rhi,
				QRhiRenderTarget* renderTarget,
				QRhiCommandBuffer* commandBuffer,
				QRhiRenderPassDescriptor* renderPassDescriptor);

	/**
		Build a QRhiResourceUpdateBatch containing all pending geometry uploads,
		GlobalUBO and per-draw DrawUBO fills for the current frame. The caller
		is responsible for submitting the batch (e.g. via
		commandBuffer()->resourceUpdate() in a QSGRenderNode::render()).

		Important: the batch must be submitted inside an active render pass.
		QSGRenderNode::prepare() is called before beginPass(), so submitting
		there would trigger D3D11 assertion "cbD->commands.isEmpty()".

		This is the first phase of inline rendering; call IssuePendingDrawCalls()
		afterwards to emit the actual draw commands.
	*/
	QRhiResourceUpdateBatch* FlushPendingUpdates();

	/**
		Issue all pending draw calls to m_commandBuffer. The caller must have
		already applied the resource-update batch returned by FlushPendingUpdates()
		and set the viewport. After this call the pending-draw list is cleared.

		This is the second phase of inline rendering, used by QSGRenderNode
		implementations that are already inside a render pass owned by Qt Quick.
	*/
	void IssuePendingDrawCalls();

	// reimplemented (imt3dview::IRenderBackend)
	virtual bool Initialize() override;
	virtual void Shutdown() override;
	virtual void BeginFrame(const imt3dview::SceneState& sceneState) override;
	virtual void EndFrame() override;
	virtual imt3dview::IRenderResourcePtr CreateGeometry(const imt3dview::VertexLayout& layout) override;
	virtual void UpdateGeometry(
				imt3dview::IRenderResource& resource,
				const void* vertexData,
				size_t vertexBytes,
				const uint32_t* indices,
				size_t indexCount) override;
	virtual void RefreshVertices(
				imt3dview::IRenderResource& resource,
				const void* vertexData,
				size_t vertexBytes) override;
	virtual void Draw(const imt3dview::DrawCommand& command) override;
	virtual void DestroyResource(imt3dview::IRenderResource& resource) override;

private:
	// One entry per queued Draw() call.
	struct PendingDraw
	{
		imt3dview::DrawCommand command;
	};

	// One entry per deferred geometry upload (queued by UpdateGeometry / RefreshVertices,
	// flushed at the start of the next EndFrame via the resource-update batch).
	struct PendingUpload
	{
		class CRhiGeometryResource* geometry;
		QByteArray vertexData; // already in canonical 9-float format
		QByteArray indexData;  // raw quint32 array; empty for vertex-only
		bool vertexOnly;
	};

	// Key into the pipeline cache.
	struct PipelineKey
	{
		QRhiGraphicsPipeline::Topology topology;
		bool cullFace;

		bool operator<(const PipelineKey& o) const;
	};

	// std140-compatible GlobalUBO layout (176 bytes).
	struct GlobalUboData
	{
		float viewMatrix[16]; // offset 0
		float projMatrix[16]; // offset 64
		float viewPosition[4]; // offset 128 (xyz + 1 float padding)
		float lightPosition[4]; // offset 144
		float lightColor[4]; // offset 160
	}; // total: 176 bytes

	// std140-compatible DrawUBO layout (96 bytes).
	struct DrawUboData
	{
		float modelMatrix[16]; // offset 0
		float itemColor[4]; // offset 64 (xyz + 1 float padding)
		qint32 colorMode; // offset 80
		qint32 useNormals; // offset 84
		qint32 usePointSize; // offset 88
		float pointSize; // offset 92
	}; // total: 96 bytes

	static QByteArray ConvertToCanonical(
				const void* src,
				size_t srcBytes,
				const imt3dview::VertexLayout& layout);

	QShader LoadShader(const QString& resourcePath) const;
	QRhiShaderResourceBindings* BuildSrb() const;
	QRhiGraphicsPipeline* GetOrCreatePipeline(const PipelineKey& key);

	void FillGlobalUbo(const imt3dview::SceneState& state, GlobalUboData& out) const;
	void FillDrawUbo(const PendingDraw& draw, DrawUboData& out) const;

	static QRhiGraphicsPipeline::Topology ToRhiTopology(imt3dview::PrimitiveType pt);

private:
	// RHI context (supplied by CRhiWidget each frame)
	QRhi* m_rhi = nullptr;
	QRhiRenderTarget* m_renderTarget = nullptr;
	QRhiCommandBuffer* m_commandBuffer = nullptr;
	QRhiRenderPassDescriptor* m_rpDesc = nullptr;

	// Persistent GPU resources
	QRhiBuffer* m_globalUbo = nullptr;
	QRhiBuffer* m_drawUbo = nullptr;
	QRhiShaderResourceBindings* m_srb = nullptr;
	std::map<PipelineKey, QRhiGraphicsPipeline*> m_pipelines;

	// Aligned slot size for each per-draw UBO entry (≥ sizeof(DrawUboData),
	// rounded up to m_rhi->ubufAlignment()).
	int m_alignedDrawUboSize = 0;

	// Loaded shader objects
	QShader m_vertexShader;
	QShader m_fragmentShader;

	// Per-frame collections
	imt3dview::SceneState m_sceneState;
	std::vector<PendingDraw> m_pendingDraws;
	std::vector<PendingUpload> m_pendingUploads;

	bool m_initialized = false;
};


} // namespace imt3dgui
