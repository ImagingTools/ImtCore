// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CShape3dBase.h>


// ImtCore includes
#include <imt3dview/CPointFormatTraits.h>
#include <imt3dview/IScene3dCamera.h>


namespace imt3dgui
{


QVector3D ExtractPosition(const imt3d::IPointsBasedObject& points, int index)
{
	QVector3D result;

	switch (points.GetPointFormat())
	{
	case imt3d::IPointsBasedObject::PF_XYZ_32:
	{
		const imt3d::IPointsBasedObject::PointXyz32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyz32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZ_64:
	{
		const imt3d::IPointsBasedObject::PointXyz64* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyz64*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
	{
		const imt3d::IPointsBasedObject::PointXyzAbc32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyzAbc32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZW_32:
	{
		const imt3d::IPointsBasedObject::PointXyzw32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyzw32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
	{
		const imt3d::IPointsBasedObject::PointXyzwNormal32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyzwNormal32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
	{
		const imt3d::IPointsBasedObject::PointXyzwNormalRgba32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyzwNormalRgba32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
	{
		const imt3d::IPointsBasedObject::PointXyzwRgba32* dataPtr = static_cast<const imt3d::IPointsBasedObject::PointXyzwRgba32*>(points.GetPointData(index));
		result.setX(static_cast<float>(dataPtr->data[0]));
		result.setY(static_cast<float>(dataPtr->data[1]));
		result.setZ(static_cast<float>(dataPtr->data[2]));
	}
	break;
	default:
		break;
	}

	return result;
}

// public methods

CShape3dBase::CShape3dBase()
	: m_pointsDataPtr(nullptr),
	m_scale(1.0),
	m_backendPtr(nullptr),
	m_geometryFormat(imt3d::IPointsBasedObject::PF_UNDEFINED),
	m_cameraPtr(nullptr),
	m_isVisible(true)
{
}


CShape3dBase::~CShape3dBase()
{
}


int CShape3dBase::FindVertex(const QPoint& point, bool limitDistance, QVector3D* positionPtr) const
{
	if (point.isNull() || m_pointsDataPtr == nullptr || m_pointsDataPtr->IsEmpty()){
		return -1;
	}

	// project window 2D coordinate to near and far planes getting 3D world coordinates
	// create a ray between those points
	QVector3D rayFrom = WindowToModel(point, 0.0);
	QVector3D rayTo = WindowToModel(point, 1.0);
	QVector3D rayDirection = (rayTo - rayFrom).normalized();

	// find a vertex closest to the ray
	float distanceEpsilon = 0.1 * m_scale;
	float minDistance = qInf();
	int retVal = -1;

	for (int i = 0; i < m_pointsDataPtr->GetPointsCount(); ++i){
		QVector3D vertPosition = ExtractPosition(*m_pointsDataPtr, i);
		float distanceToRay = qAbs(vertPosition.distanceToLine(rayFrom, rayDirection));

		// if epsilon is given, we search for a vertex lying at that distance from the ray, closest to the beginning of the ray (to the camera)
		// otherwise we look for any vertex closest to the ray
		if (limitDistance){
			float distanceToRayStart = qAbs(vertPosition.distanceToPoint(rayFrom));
			if (distanceToRay < distanceEpsilon && distanceToRayStart < minDistance){
				minDistance = distanceToRayStart;
				retVal = i;
			}
		}
		else {
			if (distanceToRay < minDistance){
				minDistance = distanceToRay;
				retVal = i;
			}
		}
	}

	if (retVal >= 0 && positionPtr){
		*positionPtr = ExtractPosition(*m_pointsDataPtr, retVal);
	}

	return retVal;
}


// reimplemented (IShape3d)

bool CShape3dBase::IsValid() const
{
	return m_backendPtr != nullptr && m_geometry.get() != nullptr;
}


bool CShape3dBase::IsVisible() const
{
	return m_isVisible;
}


void CShape3dBase::SetVisible(bool isVisible)
{
	m_isVisible = isVisible;
}


// reimplemented (IScene3dItem)

void CShape3dBase::SetCamera(const imt3dview::IScene3dCamera* cameraPtr)
{
	m_cameraPtr = cameraPtr;
}


void CShape3dBase::SetProjection(const QMatrix4x4& projection)
{
	m_projection = projection;
}


void CShape3dBase::SetViewPort(const QRect& viewPort)
{
	m_viewPort = viewPort;
}


const QVector3D& CShape3dBase::GetPosition() const
{
	return m_position;
}


void CShape3dBase::SetPosition(const QVector3D& position)
{
	m_position = position;
}


const QQuaternion& CShape3dBase::GetRotation() const
{
	return m_rotation;
}


void CShape3dBase::SetRotation(const QQuaternion &rotation)
{
	m_rotation = rotation;
}


float CShape3dBase::GetScale() const
{
	return m_scale;
}


void CShape3dBase::SetScale(float scale)
{
	m_scale = scale;
}


// reimplemented (IDrawable)

void CShape3dBase::OnAttachBackend(imt3dview::IRenderBackend* backendPtr)
{
	if (backendPtr != nullptr){
		if (m_backendPtr == backendPtr){
			return;
		}

		m_backendPtr = backendPtr;
		// geometry is created lazily as soon as we know the point format
		m_geometry.reset();
		m_geometryFormat = imt3d::IPointsBasedObject::PF_UNDEFINED;

		UpdateGeometry(istd::IChangeable::GetAllChanges());
	}
	else {
		m_geometry.reset();
		m_geometryFormat = imt3d::IPointsBasedObject::PF_UNDEFINED;
		m_backendPtr = nullptr;
	}
}


void CShape3dBase::Render(imt3dview::IRenderBackend& backend)
{
	if (!m_isVisible ||
				!m_geometry ||
				m_pointsDataPtr == nullptr ||
				m_pointsDataPtr->GetData() == nullptr ||
				m_pointsDataPtr->IsEmpty() ||
				m_indices.isEmpty()){
		return;
	}

	imt3dview::DrawCommand command;
	command.geometry = m_geometry;
	command.primitive = GetPrimitiveType();
	command.indexCount = m_indices.size();
	command.indexOffset = 0;
	command.modelMatrix = GetModelMatrix();

	FillMaterial(command.material);

	backend.Draw(command);
}


void CShape3dBase::DrawOverlay(QPainter& /*painter*/)
{
}


// protected methods

void CShape3dBase::FillMaterial(imt3dview::Material& material) const
{
	if (m_pointsDataPtr != nullptr && imt3dview::CPointFormatTraits::HasColor(m_pointsDataPtr->GetPointFormat())){
		material.colorMode = imt3dview::Material::CM_PER_VERTEX;
	}
	else{
		material.colorMode = imt3dview::Material::CM_SOLID;
		material.solidColor = GetColor();
	}

	material.useNormals = (m_pointsDataPtr != nullptr && imt3dview::CPointFormatTraits::HasNormals(m_pointsDataPtr->GetPointFormat()));
}


void CShape3dBase::UpdateGeometry(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateShapeGeometry(changeSet);

	CreateGeometry();
}


QMatrix4x4 CShape3dBase::GetModelMatrix() const
{
	QMatrix4x4 modelMatrix;

	modelMatrix.scale(QVector3D(m_scale, m_scale, m_scale));
	modelMatrix.translate(m_position);
	modelMatrix.rotate(m_rotation);

	return modelMatrix;
}


QPoint CShape3dBase::ModelToWindow(const QVector3D& modelCoordinate) const
{
	if (!m_cameraPtr){
		return QPoint();
	}

	QMatrix4x4 modelMatrix = GetModelMatrix();
	QMatrix4x4 viewMatrix = m_cameraPtr->GetViewMatrix();
	viewMatrix.translate(QVector3D(0,0,0));

	QVector3D windowCoordinate = modelCoordinate.project(viewMatrix * modelMatrix, m_projection, m_viewPort);

	// QVector3D::project method returns Y coordinate in OpenGL orientation (bottom is 0)
	// as opposed to Qt widget orientation (top is 0)
	// so make the y coordinate transformation to widget orientation
	windowCoordinate.setY(m_viewPort.height() - windowCoordinate.y());

	return windowCoordinate.toPoint();
}


QVector3D CShape3dBase::WindowToModel(const QPoint& windowCoordinate, float z) const
{
	if (!m_cameraPtr){
		return QVector3D();
	}

	QMatrix4x4 modelMatrix = GetModelMatrix();
	QMatrix4x4 viewMatrix = m_cameraPtr->GetViewMatrix();

	QVector3D windowCoordinateTmp;
	windowCoordinateTmp.setX(windowCoordinate.x());
	windowCoordinateTmp.setY(m_viewPort.height() - windowCoordinate.y());
	windowCoordinateTmp.setZ(z);

	return windowCoordinateTmp.unproject(viewMatrix * modelMatrix, m_projection, m_viewPort);
}


void CShape3dBase::CreateGeometry()
{
	if (m_backendPtr == nullptr ||
				m_pointsDataPtr == nullptr ||
				m_pointsDataPtr->GetData() == nullptr ||
				m_pointsDataPtr->IsEmpty() ||
				m_indices.isEmpty()){
		return;
	}

	imt3d::IPointsBasedObject::PointFormat pointFormat = m_pointsDataPtr->GetPointFormat();

	// (re)create the backend resource if format changed or it doesn't yet exist
	if (!m_geometry || m_geometryFormat != pointFormat){
		imt3dview::VertexLayout layout = imt3dview::CPointFormatTraits::BuildLayout(pointFormat);
		// fall back to the actual stride reported by the points object in case of unknown formats
		if (layout.stride == 0){
			layout.stride = m_pointsDataPtr->GetPointBytesSize();
		}
		m_geometry = m_backendPtr->CreateGeometry(layout);
		m_geometryFormat = pointFormat;
	}

	if (!m_geometry){
		return;
	}

	const size_t vertexBytes = static_cast<size_t>(m_pointsDataPtr->GetPointsCount()) *
				static_cast<size_t>(m_pointsDataPtr->GetPointBytesSize());

	m_backendPtr->UpdateGeometry(
				*m_geometry,
				m_pointsDataPtr->GetData(),
				vertexBytes,
				reinterpret_cast<const uint32_t*>(m_indices.data()),
				static_cast<size_t>(m_indices.size()));
}


void CShape3dBase::RefreshGeometry()
{
	if (m_backendPtr == nullptr ||
				!m_geometry ||
				m_pointsDataPtr == nullptr ||
				m_pointsDataPtr->GetData() == nullptr ||
				m_pointsDataPtr->IsEmpty()){
		return;
	}

	const size_t vertexBytes = static_cast<size_t>(m_pointsDataPtr->GetPointsCount()) *
				static_cast<size_t>(m_pointsDataPtr->GetPointBytesSize());

	m_backendPtr->RefreshVertices(*m_geometry, m_pointsDataPtr->GetData(), vertexBytes);
}


// reimplemented (imod::CSingleModelObserverBase)

void CShape3dBase::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_backendPtr != nullptr){
		UpdateGeometry(changeSet);
	}
}


} // namespace imt3dgui
