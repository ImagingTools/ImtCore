#include <imtview3d/CTrackballCamera.h>


namespace imtview3d
{


// static members

const double CTrackballCamera::s_zoomSpeed = 0.05;
const double CTrackballCamera::s_moveSpeed = 0.005;
const double CTrackballCamera::s_rotationSpeed = 10.0;


// public methods

CTrackballCamera::CTrackballCamera()
	:m_nearPlane(0.0),
	m_farPlane(0.0)
{
}


// reimplement (imtview3d::IScene3dCamera)

void CTrackballCamera::SetPlanes(float nearPlane, float farPlane)
{
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}


void CTrackballCamera::SetViewPortSize(const QSize& size)
{
	m_viewPortSize = size;
}


const QVector3D& CTrackballCamera::GetPosition() const
{
	return m_position;
}


void CTrackballCamera::MoveTo(const QVector3D& position)
{
	m_position = position;
}


void CTrackballCamera::MoveTo(const QPoint& pointFrom, const QPoint& pointTo)
{
	QPoint mousePositionDiff = pointTo - pointFrom;

	m_position += QVector3D(-mousePositionDiff.x() * s_moveSpeed, mousePositionDiff.y() * s_moveSpeed, 0.0);
}


const QQuaternion& CTrackballCamera::GetRotation() const
{
	return m_rotation;
}


void CTrackballCamera::RotateTo(const QQuaternion& rotation)
{
	m_rotation = rotation;
}


void CTrackballCamera::RotateTo(const QPoint& pointFrom, const QPoint& pointTo)
{
	QVector3D point3dFrom = GetTrackballVector(pointFrom);
	QVector3D point3dTo = GetTrackballVector(pointTo);

	if ((point3dFrom - point3dTo).length() > FLT_MIN)
	{
		QVector3D rotationAxis = QVector3D::crossProduct(point3dFrom, point3dTo);
		float rotationAngle = qRadiansToDegrees(std::asin(rotationAxis.length())) * s_rotationSpeed;

		m_rotation = QQuaternion::fromAxisAndAngle(rotationAxis.normalized(), rotationAngle) * m_rotation;
	}
}


void CTrackballCamera::ZoomIn()
{
	if (m_position.z() - s_zoomSpeed > m_nearPlane){
		m_position.setZ(m_position.z() - s_zoomSpeed);
	}
}


void CTrackballCamera::ZoomOut()
{
	if (m_position.z() + s_zoomSpeed < m_farPlane){
		m_position.setZ(m_position.z() + s_zoomSpeed);
	}
}


QMatrix4x4 CTrackballCamera::GetViewMatrix() const
{
	QMatrix4x4 viewMatrix;
	viewMatrix.translate(-m_position);
	viewMatrix.rotate(m_rotation);

	return viewMatrix;
}


QVector3D CTrackballCamera::GetTrackballVector(const QPoint& point2d) const
{
	QVector3D point3d;

	point3d.setX(((float)point2d.x() * 2.0f) / (float)m_viewPortSize.width() - 1.0);
	point3d.setY(((float)point2d.y() * 2.0f) / (float)m_viewPortSize.height() - 1.0);
	point3d.setY(-point3d.y());
	point3d.setZ(0.0);

	float sqrZ = 1.0f - QVector3D::dotProduct(point3d, point3d);

	if (sqrZ > 0.0f)
		point3d.setZ(std::sqrt(sqrZ));
	else
		point3d.normalize();

	return point3d;
}


} // namespace imtview3d


