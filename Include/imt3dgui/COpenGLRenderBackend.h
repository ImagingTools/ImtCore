// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QOpenGLFunctions>
#if QT_VERSION < 0x060000
#include <QtGui/QOpenGLShaderProgram>
#else
#include <QtOpenGL/QOpenGLShaderProgram>
#endif

// ImtCore includes
#include <imt3dview/IRenderBackend.h>


class QOpenGLContext;


namespace imt3dgui
{


/**
	OpenGL implementation of imt3dview::IRenderBackend.

	Owns the QOpenGLShaderProgram (loaded from `:/Shaders/vshader.glsl` and
	`:/Shaders/fshader.glsl`) and translates backend-neutral DrawCommand /
	SceneState data into Qt OpenGL calls.
*/
class COpenGLRenderBackend: public imt3dview::IRenderBackend
{
public:
	COpenGLRenderBackend();
	virtual ~COpenGLRenderBackend();

	/**
		Provide the OpenGL context the backend will operate on. Must be called
		before Initialize().
	*/
	void SetContext(QOpenGLContext* contextPtr);

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
	void ApplyRenderHints(int renderHints);
	void ApplyAttributeBindings(const imt3dview::VertexLayout& layout);
	void DisableAttributes();
	void ApplyMaterial(const imt3dview::Material& material);

	static GLenum ToGlPrimitive(imt3dview::PrimitiveType type);

private:
	QOpenGLContext* m_contextPtr;
	QOpenGLShaderProgram* m_programPtr;
	bool m_isInitialized;
};


} // namespace imt3dgui
