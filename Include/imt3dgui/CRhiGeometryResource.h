// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <rhi/qrhi.h>

// ImtCore includes
#include <imt3dview/IRenderResource.h>
#include <imt3dview/SVertexLayout.h>


namespace imt3dgui
{


/**
	Qt RHI geometry resource: a pair of QRhiBuffer objects (vertex + index) together
	with the source SVertexLayout (kept so that CRhiRenderBackend can convert from the
	original format to the canonical upload format on demand).

	The vertex buffer always stores data in the **canonical 9-float** interleaved format:
	    position.xyz  (floats 0-2, 12 bytes)
	    normal.xyz    (floats 3-5, 12 bytes)
	    colour.rgb    (floats 6-8, 12 bytes)
	    stride = 36 bytes
	This keeps pipeline vertex-input layout constant regardless of the source PointFormat.
	Missing normal / colour data is padded with sensible defaults during conversion.
*/
class CRhiGeometryResource: public imt3dview::IRenderResource
{
public:
	// Total size of one canonical vertex in bytes (9 floats × 4 bytes).
	static constexpr int s_canonicalStride = 9 * sizeof(float);

	explicit CRhiGeometryResource(QRhi* rhi, const imt3dview::SVertexLayout& sourceLayout);
	~CRhiGeometryResource() override;

	// Allocate / reallocate GPU buffers for the given counts.
	// Destroys any previously allocated buffers first.
	bool Create(int vertexCount, int indexCount);

	// Release GPU buffers.
	void Destroy();

	bool IsCreated() const;

	QRhiBuffer* GetVertexBuffer() const { return m_vertexBuffer; }
	QRhiBuffer* GetIndexBuffer()  const { return m_indexBuffer; }

	const imt3dview::SVertexLayout& GetSourceLayout() const { return m_sourceLayout; }

	int  GetIndexCount() const      { return m_indexCount; }
	void SetIndexCount(int n)       { m_indexCount = n; }

private:
	QRhi*                    m_rhi;
	imt3dview::SVertexLayout m_sourceLayout;
	QRhiBuffer*              m_vertexBuffer = nullptr;
	QRhiBuffer*              m_indexBuffer  = nullptr;
	int                      m_indexCount   = 0;
};


} // namespace imt3dgui
