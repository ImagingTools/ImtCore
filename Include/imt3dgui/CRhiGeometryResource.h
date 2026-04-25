// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <rhi/qrhi.h>

// ImtCore includes
#include <imt3dview/IRenderResource.h>
#include <imt3dview/VertexLayout.h>


namespace imt3dgui
{


/**
	Qt RHI geometry resource: a pair of QRhiBuffer objects (vertex + index) together
	with the source VertexLayout (kept so that CRhiRenderBackend can convert from the
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

	explicit CRhiGeometryResource(QRhi* rhi, const imt3dview::VertexLayout& sourceLayout);
	~CRhiGeometryResource() override;

	// Allocate / reallocate GPU buffers for the given counts.
	// Destroys any previously allocated buffers first.
	bool Create(int vertexCount, int indexCount);

	// Release GPU buffers.
	void Destroy();

	bool IsCreated() const;

	QRhiBuffer* GetVertexBuffer() const;
	QRhiBuffer* GetIndexBuffer() const;

	const imt3dview::VertexLayout& GetSourceLayout() const;

	int GetIndexCount() const;
	void SetIndexCount(int n);

private:
	QRhi* m_rhi;
	imt3dview::VertexLayout m_sourceLayout;
	QRhiBuffer* m_vertexBuffer = nullptr;
	QRhiBuffer* m_indexBuffer = nullptr;
	int m_indexCount = 0;
};


} // namespace imt3dgui
