// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CCubeSceneItem.h>


namespace imt3dgui
{


CCubeSceneItem::CCubeSceneItem()
{
}


CCubeSceneItem::~CCubeSceneItem()
{
	if (m_backendPtr && m_geometry){
		m_backendPtr->DestroyResource(*m_geometry);
	}
}


void CCubeSceneItem::SetColor(const QVector3D& color)
{
	m_color = color;
	m_geometryDirty = true;
}


QVector3D CCubeSceneItem::GetColor() const
{
	return m_color;
}


// reimplemented (imt3dview::IScene3dItem)

bool CCubeSceneItem::IsValid() const
{
	return true;
}


void CCubeSceneItem::SetCamera(const imt3dview::IScene3dCamera* /*cameraPtr*/)
{
}


void CCubeSceneItem::SetProjection(const QMatrix4x4& /*projection*/)
{
}


void CCubeSceneItem::SetViewPort(const QRect& /*viewPort*/)
{
}


const QVector3D& CCubeSceneItem::GetPosition() const
{
	return m_position;
}


void CCubeSceneItem::SetPosition(const QVector3D& position)
{
	m_position = position;
}


const QQuaternion& CCubeSceneItem::GetRotation() const
{
	return m_rotation;
}


void CCubeSceneItem::SetRotation(const QQuaternion& rotation)
{
	m_rotation = rotation;
}


float CCubeSceneItem::GetScale() const
{
	return m_scale;
}


void CCubeSceneItem::SetScale(float scale)
{
	m_scale = scale;
}


// reimplemented (imt3dview::IDrawable)

void CCubeSceneItem::OnAttachBackend(imt3dview::IRenderBackend* backendPtr)
{
	if (m_backendPtr && m_geometry){
		m_backendPtr->DestroyResource(*m_geometry);
	}
	m_geometry.reset();
	m_backendPtr = backendPtr;
	m_geometryDirty = true;
}


void CCubeSceneItem::Render(imt3dview::IRenderBackend& backend)
{
	if (!m_backendPtr){
		return;
	}

	if (m_geometryDirty){
		CreateGeometry();
		m_geometryDirty = false;
	}

	if (!m_geometry){
		return;
	}

	imt3dview::DrawCommand cmd;
	cmd.geometry = m_geometry;
	cmd.primitive = imt3dview::PT_TRIANGLES;
	cmd.indexCount = s_indexCount;
	cmd.indexOffset = 0;
	cmd.modelMatrix = GetModelMatrix();
	cmd.material.colorMode = imt3dview::Material::CM_SOLID;
	cmd.material.solidColor = m_color;
	cmd.material.useNormals = true;
	cmd.material.useLighting = true;

	backend.Draw(cmd);
}


void CCubeSceneItem::DrawOverlay(QPainter& /*painter*/)
{
}


void CCubeSceneItem::CreateGeometry()
{
	if (!m_backendPtr){
		return;
	}

	// Build vertex layout: position(3) + normal(3) + color(3)
	imt3dview::VertexLayout layout;
	layout.stride = s_stride;
	layout.attributes = {
		{ imt3dview::VA_POSITION, imt3dview::AT_FLOAT_32, 3, 0 },
		{ imt3dview::VA_NORMAL, imt3dview::AT_FLOAT_32, 3, 3 * static_cast<int>(sizeof(float)) },
		{ imt3dview::VA_COLOR, imt3dview::AT_FLOAT_32, 3, 6 * static_cast<int>(sizeof(float)) },
	};

	if (!m_geometry){
		m_geometry = m_backendPtr->CreateGeometry(layout);
	}

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
		{  1, 0, 0, { 1, 2, 6, 5 } },
		{ -1, 0, 0, { 4, 7, 3, 0 } },
		{  0, 1, 0, { 3, 7, 6, 2 } },
		{  0,-1, 0, { 4, 0, 1, 5 } },
		{  0, 0, 1, { 5, 6, 7, 4 } },
		{  0, 0,-1, { 0, 3, 2, 1 } },
	};

	float vertices[s_vertexCount * s_floatsPerVertex]{};
	uint32_t indices[s_indexCount]{};

	const float cr = m_color.x();
	const float cg = m_color.y();
	const float cb = m_color.z();

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
			dst[6] = cr; dst[7] = cg; dst[8] = cb;
			vOff += s_floatsPerVertex;
		}
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 0);
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 1);
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 2);
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 0);
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 2);
		indices[iOff++] = static_cast<uint32_t>(baseVertex + 3);
	}

	m_backendPtr->UpdateGeometry(
				*m_geometry,
				vertices, sizeof(vertices),
				indices, s_indexCount);
}


QMatrix4x4 CCubeSceneItem::GetModelMatrix() const
{
	QMatrix4x4 model;
	model.setToIdentity();
	model.translate(m_position);
	model.rotate(m_rotation);
	model.scale(m_scale);
	return model;
}


} // namespace imt3dgui
