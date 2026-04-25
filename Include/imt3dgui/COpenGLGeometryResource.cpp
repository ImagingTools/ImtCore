// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/COpenGLGeometryResource.h>


namespace imt3dgui
{


// public methods

COpenGLGeometryResource::COpenGLGeometryResource(const imt3dview::SVertexLayout& layout)
	:m_layout(layout),
	m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
	m_indexBuffer(QOpenGLBuffer::IndexBuffer),
	m_indexCount(0)
{
	m_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	m_indexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
}


COpenGLGeometryResource::~COpenGLGeometryResource()
{
	Destroy();
}


bool COpenGLGeometryResource::Create()
{
	if (!m_vertexBuffer.isCreated()){
		if (!m_vertexBuffer.create()){
			return false;
		}
	}
	if (!m_indexBuffer.isCreated()){
		if (!m_indexBuffer.create()){
			return false;
		}
	}

	return true;
}


void COpenGLGeometryResource::Destroy()
{
	if (m_vertexBuffer.isCreated()){
		m_vertexBuffer.destroy();
	}
	if (m_indexBuffer.isCreated()){
		m_indexBuffer.destroy();
	}
	m_indexCount = 0;
}


} // namespace imt3dgui
