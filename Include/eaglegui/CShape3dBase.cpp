#include <eaglegui/CShape3dBase.h>


namespace eaglegui
{


// public methods

CShape3dBase::CShape3dBase()
	:m_scale(1.0, 1.0, 1.0),
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

void CShape3dBase::SetCamera(const eagle::IScene3dCamera* cameraPtr)
{
	m_cameraPtr = cameraPtr;
}


void CShape3dBase::SetProjection(const QMatrix4x4& projection)
{
	m_projection = projection;
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


const QVector3D& CShape3dBase::GetScale() const
{
	return m_scale;
}


void CShape3dBase::SetScale(const QVector3D& scale)
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

		UpdateGeometry();
	}
}


void CShape3dBase::OnContextDestroyed(QOpenGLContext* /*contextPtr*/)
{
	m_vertexBuffer.destroy();
	m_indexBuffer.destroy();
}


// protected methods

void CShape3dBase::UpdateGeometry()
{
	UpdateShapeGeometry();

	UploadGeometry(true, m_vertices, m_vertexBuffer);
	UploadGeometry(true, m_indices, m_indexBuffer);
}


QMatrix4x4 CShape3dBase::GetModelMatrix() const
{
	QMatrix4x4 modelMatrix;

	modelMatrix.scale(m_scale);
	modelMatrix.translate(m_position);
	modelMatrix.rotate(m_rotation);

	return modelMatrix;
}


QPoint CShape3dBase::ModelToWindow(const QVector3D& modelCoordinate, const QRect& viewPort) const
{
	if (!m_cameraPtr){
		return QPoint();
	}

	QMatrix4x4 modelMatrix = GetModelMatrix();
	QMatrix4x4 viewMatrix = m_cameraPtr->GetViewMatrix();

	QVector3D windowCoordinate = modelCoordinate.project(viewMatrix * modelMatrix, m_projection, viewPort);

	// QVector3D::project method returns Y coordinate in OpenGL orientation (bottom is 0)
	// as opposed to Qt widget orientation (top is 0)
	// so make the y coordinate transformation to widget orientation
	windowCoordinate.setY(viewPort.height() - windowCoordinate.y());

	return windowCoordinate.toPoint();
}


QVector3D CShape3dBase::WindowToModel(const QPoint& windowCoordinate, int z, const QRect& viewPort) const
{
	if (!m_cameraPtr){
		return QVector3D();
	}

	QMatrix4x4 modelMatrix = GetModelMatrix();
	QMatrix4x4 viewMatrix = m_cameraPtr->GetViewMatrix();

	QVector3D windowCoordinateTmp;
	windowCoordinateTmp.setX(windowCoordinate.x());
	windowCoordinateTmp.setY(viewPort.height() - windowCoordinateTmp.y());
	windowCoordinateTmp.setZ(z);

	return windowCoordinateTmp.unproject(viewMatrix * modelMatrix, m_projection, viewPort);
}


// reimplemented (imod::CSingleModelObserverBase)

void CShape3dBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (IsValid()){
		UpdateGeometry();
	}
}



} // namespace eaglegui


