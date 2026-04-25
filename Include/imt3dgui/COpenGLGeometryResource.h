// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#if QT_VERSION < 0x060000
#include <QtGui/QOpenGLBuffer>
#else
#include <QtOpenGL/QOpenGLBuffer>
#endif

// ImtCore includes
#include <imt3dview/IRenderResource.h>
#include <imt3dview/VertexLayout.h>


namespace imt3dgui
{


/**
	OpenGL geometry resource: pair of VBO + IBO + the layout describing the vertex format.
	Owned by COpenGLRenderBackend; held by shapes through std::shared_ptr<IRenderResource>.
*/
class COpenGLGeometryResource: public imt3dview::IRenderResource
{
public:
	COpenGLGeometryResource(const imt3dview::VertexLayout& layout);
	virtual ~COpenGLGeometryResource();

	const imt3dview::VertexLayout& GetLayout() const { return m_layout; }

	QOpenGLBuffer& GetVertexBuffer() { return m_vertexBuffer; }
	QOpenGLBuffer& GetIndexBuffer() { return m_indexBuffer; }

	int GetIndexCount() const { return m_indexCount; }
	void SetIndexCount(int count) { m_indexCount = count; }

	bool IsCreated() const { return m_vertexBuffer.isCreated() && m_indexBuffer.isCreated(); }

	bool Create();
	void Destroy();

private:
	imt3dview::VertexLayout m_layout;
	QOpenGLBuffer m_vertexBuffer;
	QOpenGLBuffer m_indexBuffer;
	int m_indexCount;
};


} // namespace imt3dgui
