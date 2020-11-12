#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{


// public methods

CShape3dBase::CShape3dBase()
	:m_scale(1.0),
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
	if (point.isNull() || m_vertices.isEmpty()){
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

	for (int i = 0; i < m_vertices.size(); ++i){
		float distanceToRay = qAbs(m_vertices[i].position.distanceToLine(rayFrom, rayDirection));

		// if epsilon is given, we search for a vertex lying at that distance from the ray, closest to the beginning of the ray (to the camera)
		// otherwise we look for any vertex closest to the ray
		if (limitDistance){
			float distanceToRayStart = qAbs(m_vertices[i].position.distanceToPoint(rayFrom));
			if (distanceToRay < distanceEpsilon && distanceToRayStart < minDistance){
				minDistance = distanceToRayStart;
				retVal = i;
			}
		}
		else{
			if (distanceToRay < minDistance){
				minDistance = distanceToRay;
				retVal = i;
			}
		}
	}

	if (retVal >= 0 && positionPtr){
		*positionPtr = m_vertices[retVal].position;
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
	if (contextPtr != NULL){
		OnContextInitialized(contextPtr);
	}
	else{
		OnContextDestroyed(m_contextPtr);
	}

	m_contextPtr = contextPtr;
}


// pseudo-reimplemented (IDrawable)

void CShape3dBase::DrawGl(QOpenGLShaderProgram &program)
{
	if (!m_isVisible || m_vertices.isEmpty() || m_indices.isEmpty() || !IsValid()){
		return;
	}

	m_vertexBuffer.bind();
	m_indexBuffer.bind();

	// set model matrix
	program.setUniformValue("modelMatrix", GetModelMatrix());

	// set point size
	program.setUniformValue("usePointSize", false);

	// set points
	program.enableAttributeArray("pointPosition");
	program.setAttributeBuffer("pointPosition", GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

	// set normals
	program.setUniformValue("useNormals", HasNormals());

	if (HasNormals()){
		program.enableAttributeArray("pointNormal");
		program.setAttributeBuffer("pointNormal", GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));
	}

	// set color
	program.setUniformValue("colorMode", GetColorMode());

	switch (GetColorMode()){
		case IShape3d::CM_POINT:
			program.enableAttributeArray("pointColor");
			program.setAttributeBuffer("pointColor", GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));
			break;
		case IShape3d::CM_ITEM:
			program.setUniformValue("itemColor", GetColor());
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


// reimplemented (IShape3d)

void CShape3dBase::OnContextInitialized(QOpenGLContext* contextPtr)
{
	if (contextPtr){
		if (!m_vertexBuffer.isCreated()){
			m_vertexBuffer.create();
		}

		if (!m_indexBuffer.isCreated()){
			m_indexBuffer.create();
		}

		Q_ASSERT(m_vertexBuffer.isCreated());
		Q_ASSERT(m_indexBuffer.isCreated());

		UpdateGeometry(istd::IChangeable::GetAllChanges());
	}
}


void CShape3dBase::OnContextDestroyed(QOpenGLContext* /*contextPtr*/)
{
	m_vertexBuffer.destroy();
	m_indexBuffer.destroy();
}


// protected methods

void CShape3dBase::UpdateGeometry(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateShapeGeometry(changeSet);

	UploadGeometry(true, m_vertices, m_vertexBuffer);
	UploadGeometry(true, m_indices, m_indexBuffer);
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


// reimplemented (imod::CSingleModelObserverBase)

void CShape3dBase::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (IsValid()){
		UpdateGeometry(changeSet);
	}
}



} // namespace imt3dgui


