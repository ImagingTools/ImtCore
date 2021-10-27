#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{

QVector3D extractPosition(const imt3d::IPointsBasedObject& points, int index) {
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
	m_contextPtr(nullptr),
	m_cameraPtr(nullptr),
	m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
	m_indexBuffer(QOpenGLBuffer::IndexBuffer),
	m_isVisible(true)
{
	m_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	m_indexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
}


CShape3dBase::~CShape3dBase()
{
}


int CShape3dBase::FindVertex(const QPoint& point, bool limitDistance, QVector3D* positionPtr) const
{
	if (point.isNull() || m_pointsDataPtr == nullptr || m_pointsDataPtr->IsEmpty()) {
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

	for (int i = 0; i < m_pointsDataPtr->GetPointsCount(); ++i) {
		QVector3D vertPosition = extractPosition(*m_pointsDataPtr, i);
		float distanceToRay = qAbs(vertPosition.distanceToLine(rayFrom, rayDirection));

		// if epsilon is given, we search for a vertex lying at that distance from the ray, closest to the beginning of the ray (to the camera)
		// otherwise we look for any vertex closest to the ray
		if (limitDistance) {
			float distanceToRayStart = qAbs(vertPosition.distanceToPoint(rayFrom));
			if (distanceToRay < distanceEpsilon && distanceToRayStart < minDistance) {
				minDistance = distanceToRayStart;
				retVal = i;
			}
		}
		else {
			if (distanceToRay < minDistance) {
				minDistance = distanceToRay;
				retVal = i;
			}
		}
	}

	if (retVal >= 0 && positionPtr) {
		*positionPtr = extractPosition(*m_pointsDataPtr, retVal);
	}

	return retVal;
}


// reimplemented (IShape3d)

bool CShape3dBase::IsValid() const
{
	return m_contextPtr && m_vertexBuffer.isCreated() && m_indexBuffer.isCreated();
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


void CShape3dBase::SetContext(QOpenGLContext* contextPtr)
{
	if (contextPtr != nullptr) {
		if (m_contextPtr != nullptr)
			return;
		m_contextPtr = contextPtr;

		if (!m_vertexBuffer.isCreated()) {
			m_vertexBuffer.create();
		}

		if (!m_indexBuffer.isCreated()) {
			m_indexBuffer.create();
		}

		Q_ASSERT(m_vertexBuffer.isCreated());
		Q_ASSERT(m_indexBuffer.isCreated());

		UpdateGeometry(istd::IChangeable::GetAllChanges());
	}
	else {
		if (m_vertexBuffer.isCreated()) {
			m_vertexBuffer.destroy();
		}

		if (m_indexBuffer.isCreated()) {
			m_indexBuffer.destroy();
		}

		m_contextPtr = nullptr;
	}

}


// pseudo-reimplemented (IDrawable)

void CShape3dBase::DrawGl(QOpenGLShaderProgram &program)
{
	if (!m_isVisible || !IsValid() ||
		m_pointsDataPtr == nullptr || m_pointsDataPtr->GetData() == nullptr ||
		m_pointsDataPtr->IsEmpty() || m_indices.isEmpty()
		) {
		return;
	}

	m_vertexBuffer.bind();
	m_indexBuffer.bind();

	// set model matrix
	program.setUniformValue("modelMatrix", GetModelMatrix());

	// set point size
	program.setUniformValue("usePointSize", false);

	// set points
	switch (m_pointsDataPtr->GetPointFormat())
	{
	case imt3d::IPointsBasedObject::PF_XYZ_32:
	case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
	case imt3d::IPointsBasedObject::PF_XYZW_32:
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
	case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
	{
		program.enableAttributeArray("pointPosition");
		program.setAttributeBuffer("pointPosition", GL_FLOAT, 0, 3, m_pointsDataPtr->GetPointBytesSize());
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZ_64:
	{
		program.enableAttributeArray("pointPosition");
		program.setAttributeBuffer("pointPosition", GL_DOUBLE, 0, 3, m_pointsDataPtr->GetPointBytesSize());
	}
	break;
	default:
		break;
	}

	// set normals
	switch (m_pointsDataPtr->GetPointFormat())
	{
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
	{
		program.setUniformValue("useNormals", true);
		program.enableAttributeArray("pointNormal");
		program.setAttributeBuffer("pointNormal", GL_FLOAT, 4 * sizeof(float), 3, m_pointsDataPtr->GetPointBytesSize());
	}
	break;
	default:
		program.setUniformValue("useNormals", false);
		break;
	}

	// set color

	switch (m_pointsDataPtr->GetPointFormat())
	{
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
	{
		program.enableAttributeArray("pointColor");
		program.setUniformValue("colorMode", 0);
		program.setAttributeBuffer("pointColor", GL_FLOAT, 8 * sizeof(float), 3, m_pointsDataPtr->GetPointBytesSize());
	}
	break;
	case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
	{
		program.enableAttributeArray("pointColor");
		program.setUniformValue("colorMode", 0);
		program.setAttributeBuffer("pointColor", GL_FLOAT, 4 * sizeof(float), 3, m_pointsDataPtr->GetPointBytesSize());
	}
	break;
	default:
	{
		program.setUniformValue("colorMode", 1);
		program.setUniformValue("itemColor", GetColor());
	}
	break;
	}


	// draw shape specifics
	DrawShapeGl(program, *m_contextPtr->functions());

	m_vertexBuffer.release();
	m_indexBuffer.release();
}


void CShape3dBase::Draw(QPainter& /*painter*/)
{
}


// protected methods

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
	if (
		m_pointsDataPtr != nullptr && m_pointsDataPtr->GetData() != nullptr && !m_pointsDataPtr->IsEmpty() &&
		!m_indices.isEmpty() && IsValid()
	) {
		m_vertexBuffer.bind();
		m_indexBuffer.bind();

		m_vertexBuffer.allocate(m_pointsDataPtr->GetData(), m_pointsDataPtr->GetPointsCount() * m_pointsDataPtr->GetPointBytesSize());
		m_indexBuffer.allocate(m_indices.data(), m_indices.size() * sizeof(GLuint));

		m_vertexBuffer.release();
		m_indexBuffer.release();
	}
}

void CShape3dBase::RefreshGeometry()
{
	if (
		m_pointsDataPtr != nullptr && m_pointsDataPtr->GetData() != nullptr && !m_pointsDataPtr->IsEmpty() &&
		IsValid()
	) {
		m_vertexBuffer.bind();

		m_vertexBuffer.write(0, m_pointsDataPtr->GetData(), m_pointsDataPtr->GetPointsCount() * m_pointsDataPtr->GetPointBytesSize());

		m_vertexBuffer.release();
	}
}


// reimplemented (imod::CSingleModelObserverBase)

void CShape3dBase::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (IsValid()){
		UpdateGeometry(changeSet);
	}
}

// CShape3dGeometryUpdate impl


} // namespace imt3dgui


