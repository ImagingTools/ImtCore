// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/COpenGLRenderBackend.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QOpenGLContext>

// ImtCore includes
#include <imt3dgui/COpenGLGeometryResource.h>
#include <imt3dview/SSceneState.h>


namespace imt3dgui
{


// public methods

COpenGLRenderBackend::COpenGLRenderBackend()
	:m_contextPtr(nullptr),
	m_programPtr(nullptr),
	m_isInitialized(false)
{
}


COpenGLRenderBackend::~COpenGLRenderBackend()
{
	Shutdown();
}


void COpenGLRenderBackend::SetContext(QOpenGLContext* contextPtr)
{
	m_contextPtr = contextPtr;
}


bool COpenGLRenderBackend::Initialize()
{
	if (m_isInitialized){
		return true;
	}

	if (m_contextPtr == nullptr){
		qDebug() << "COpenGLRenderBackend::Initialize: no OpenGL context set";
		return false;
	}

	if (m_programPtr == nullptr){
		m_programPtr = new QOpenGLShaderProgram();
	}

	if (!m_programPtr->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vshader.glsl")){
		qDebug() << "COpenGLRenderBackend: vertex shader could not be loaded";
		return false;
	}

	if (!m_programPtr->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fshader.glsl")){
		qDebug() << "COpenGLRenderBackend: fragment shader could not be loaded";
		return false;
	}

	if (!m_programPtr->link()){
		qDebug() << "COpenGLRenderBackend: failed to link shader program: " << m_programPtr->log();
		return false;
	}

	m_isInitialized = true;
	return true;
}


void COpenGLRenderBackend::Shutdown()
{
	if (m_programPtr != nullptr){
		delete m_programPtr;
		m_programPtr = nullptr;
	}
	m_isInitialized = false;
}


void COpenGLRenderBackend::BeginFrame(const imt3dview::SSceneState& sceneState)
{
	if (!m_isInitialized || m_programPtr == nullptr || !m_programPtr->isLinked()){
		return;
	}

	QOpenGLFunctions* functions = m_contextPtr ? m_contextPtr->functions() : nullptr;
	if (functions == nullptr){
		return;
	}

	functions->glClearColor(
				sceneState.clearColor.redF(),
				sceneState.clearColor.greenF(),
				sceneState.clearColor.blueF(),
				1.0f);
	functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ApplyRenderHints(sceneState.renderHints);

	m_programPtr->bind();
	m_programPtr->setUniformValue("viewPosition", sceneState.cameraPosition);
	m_programPtr->setUniformValue("viewMatrix", sceneState.viewMatrix);
	m_programPtr->setUniformValue("projectionMatrix", sceneState.projectionMatrix);
	m_programPtr->setUniformValue("lightPosition", sceneState.lightPosition);
	m_programPtr->setUniformValue("lightColor", sceneState.lightColor);
}


void COpenGLRenderBackend::EndFrame()
{
	if (m_programPtr != nullptr && m_programPtr->isLinked()){
		m_programPtr->release();
	}
}


imt3dview::IRenderResourcePtr COpenGLRenderBackend::CreateGeometry(const imt3dview::SVertexLayout& layout)
{
	auto geometry = std::make_shared<COpenGLGeometryResource>(layout);
	if (!geometry->Create()){
		return imt3dview::IRenderResourcePtr();
	}
	return geometry;
}


void COpenGLRenderBackend::UpdateGeometry(
			imt3dview::IRenderResource& resource,
			const void* vertexData,
			size_t vertexBytes,
			const uint32_t* indices,
			size_t indexCount)
{
	COpenGLGeometryResource* geometry = dynamic_cast<COpenGLGeometryResource*>(&resource);
	if (geometry == nullptr || !geometry->IsCreated()){
		return;
	}

	geometry->GetVertexBuffer().bind();
	geometry->GetVertexBuffer().allocate(vertexData, static_cast<int>(vertexBytes));
	geometry->GetVertexBuffer().release();

	geometry->GetIndexBuffer().bind();
	geometry->GetIndexBuffer().allocate(indices, static_cast<int>(indexCount * sizeof(uint32_t)));
	geometry->GetIndexBuffer().release();

	geometry->SetIndexCount(static_cast<int>(indexCount));
}


void COpenGLRenderBackend::RefreshVertices(
			imt3dview::IRenderResource& resource,
			const void* vertexData,
			size_t vertexBytes)
{
	COpenGLGeometryResource* geometry = dynamic_cast<COpenGLGeometryResource*>(&resource);
	if (geometry == nullptr || !geometry->IsCreated()){
		return;
	}

	geometry->GetVertexBuffer().bind();
	geometry->GetVertexBuffer().write(0, vertexData, static_cast<int>(vertexBytes));
	geometry->GetVertexBuffer().release();
}


void COpenGLRenderBackend::Draw(const imt3dview::SDrawCommand& command)
{
	if (!m_isInitialized || m_programPtr == nullptr || !m_programPtr->isLinked()){
		return;
	}

	COpenGLGeometryResource* geometry = dynamic_cast<COpenGLGeometryResource*>(command.geometry.get());
	if (geometry == nullptr || !geometry->IsCreated()){
		return;
	}

	QOpenGLFunctions* functions = m_contextPtr ? m_contextPtr->functions() : nullptr;
	if (functions == nullptr){
		return;
	}

	geometry->GetVertexBuffer().bind();
	geometry->GetIndexBuffer().bind();

	m_programPtr->setUniformValue("modelMatrix", command.modelMatrix);

	ApplyAttributeBindings(geometry->GetLayout());
	ApplyMaterial(command.material);

	if (command.material.lineWidth > 0.0f){
		functions->glLineWidth(command.material.lineWidth);
	}

	GLenum mode = ToGlPrimitive(command.primitive);
	const void* indexOffsetPtr = reinterpret_cast<const void*>(
				static_cast<size_t>(command.indexOffset) * sizeof(uint32_t));
	functions->glDrawElements(mode, command.indexCount, GL_UNSIGNED_INT, indexOffsetPtr);

	DisableAttributes();

	geometry->GetVertexBuffer().release();
	geometry->GetIndexBuffer().release();
}


void COpenGLRenderBackend::DestroyResource(imt3dview::IRenderResource& resource)
{
	COpenGLGeometryResource* geometry = dynamic_cast<COpenGLGeometryResource*>(&resource);
	if (geometry != nullptr){
		geometry->Destroy();
	}
}


// private methods

void COpenGLRenderBackend::ApplyRenderHints(int renderHints)
{
	QOpenGLFunctions* functions = m_contextPtr ? m_contextPtr->functions() : nullptr;
	if (functions == nullptr){
		return;
	}

	functions->glEnable(GL_DEPTH_TEST);
	functions->glDepthFunc(GL_LEQUAL);

	if (renderHints & imt3dview::SSceneState::RH_ANTIALIASING){
		functions->glEnable(GL_LINE_SMOOTH);
		functions->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		functions->glEnable(GL_POINT_SMOOTH);
		functions->glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	else{
		functions->glDisable(GL_LINE_SMOOTH);
		functions->glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		functions->glDisable(GL_POINT_SMOOTH);
		functions->glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
	}

	if (renderHints & imt3dview::SSceneState::RH_BLEND){
		functions->glEnable(GL_BLEND);
		functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else{
		functions->glDisable(GL_BLEND);
		functions->glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	}

	if (renderHints & imt3dview::SSceneState::RH_CULLFACE){
		functions->glCullFace(GL_BACK);
		functions->glFrontFace(GL_CCW);
	}
	else{
		functions->glDisable(GL_CULL_FACE);
	}
}


void COpenGLRenderBackend::ApplyAttributeBindings(const imt3dview::SVertexLayout& layout)
{
	const int stride = layout.stride;

	for (const imt3dview::SVertexAttribute& attr : layout.attributes){
		const char* name = nullptr;
		switch (attr.attribute){
		case imt3dview::VA_POSITION: name = "pointPosition"; break;
		case imt3dview::VA_NORMAL:   name = "pointNormal";   break;
		case imt3dview::VA_COLOR:    name = "pointColor";    break;
		}
		if (name == nullptr){
			continue;
		}

		GLenum glType = GL_FLOAT;
		switch (attr.type){
		case imt3dview::AT_FLOAT_32: glType = GL_FLOAT;          break;
		case imt3dview::AT_FLOAT_64: glType = GL_DOUBLE;         break;
		case imt3dview::AT_UINT_8:   glType = GL_UNSIGNED_BYTE;  break;
		}

		m_programPtr->enableAttributeArray(name);
		m_programPtr->setAttributeBuffer(name, glType, attr.offset, attr.componentCount, stride);
	}
}


void COpenGLRenderBackend::DisableAttributes()
{
	// Best-effort: keep the attribute arrays in a known state by disabling the standard slots
	// before the next draw rebinds them. Disabling unknown attributes is harmless in Qt.
	m_programPtr->disableAttributeArray("pointPosition");
	m_programPtr->disableAttributeArray("pointNormal");
	m_programPtr->disableAttributeArray("pointColor");
}


void COpenGLRenderBackend::ApplyMaterial(const imt3dview::SMaterial& material)
{
	switch (material.colorMode){
	case imt3dview::SMaterial::CM_PER_VERTEX:
		m_programPtr->setUniformValue("colorMode", 0);
		break;
	case imt3dview::SMaterial::CM_SOLID:
	default:
		m_programPtr->setUniformValue("colorMode", 1);
		m_programPtr->setUniformValue("itemColor", material.solidColor);
		break;
	}

	m_programPtr->setUniformValue("useNormals", material.useNormals);
	m_programPtr->setUniformValue("usePointSize", material.usePointSize);
	if (material.usePointSize){
		m_programPtr->setUniformValue("pointSize", material.pointSize);
	}
}


GLenum COpenGLRenderBackend::ToGlPrimitive(imt3dview::EPrimitiveType type)
{
	switch (type){
	case imt3dview::PT_TRIANGLES:  return GL_TRIANGLES;
	case imt3dview::PT_LINES:      return GL_LINES;
	case imt3dview::PT_LINE_STRIP: return GL_LINE_STRIP;
	case imt3dview::PT_POINTS:     return GL_POINTS;
	}
	return GL_TRIANGLES;
}


} // namespace imt3dgui
