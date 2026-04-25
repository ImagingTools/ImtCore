// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CRhiGeometryResource.h>


namespace imt3dgui
{


// public methods

CRhiGeometryResource::CRhiGeometryResource(QRhi* rhi, const imt3dview::SVertexLayout& sourceLayout)
	:m_rhi(rhi),
	m_sourceLayout(sourceLayout)
{
}


CRhiGeometryResource::~CRhiGeometryResource()
{
	Destroy();
}


bool CRhiGeometryResource::Create(int vertexCount, int indexCount)
{
	Destroy();

	if (vertexCount <= 0 || indexCount <= 0){
		return false;
	}

	m_vertexBuffer = m_rhi->newBuffer(
				QRhiBuffer::Dynamic,
				QRhiBuffer::VertexBuffer,
				vertexCount * s_canonicalStride);
	if (!m_vertexBuffer->create()){
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
		return false;
	}

	m_indexBuffer = m_rhi->newBuffer(
				QRhiBuffer::Dynamic,
				QRhiBuffer::IndexBuffer,
				indexCount * static_cast<int>(sizeof(quint32)));
	if (!m_indexBuffer->create()){
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
		return false;
	}

	m_indexCount = indexCount;
	return true;
}


void CRhiGeometryResource::Destroy()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;
	m_indexCount = 0;
}


bool CRhiGeometryResource::IsCreated() const
{
	return m_vertexBuffer != nullptr && m_indexBuffer != nullptr;
}


} // namespace imt3dgui
