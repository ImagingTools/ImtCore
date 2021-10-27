#include <imt3dview/CTrackballCameraComp.h>
#include <cfloat>


// Qt includes
#include <QtCore/QtMath>


namespace imt3dview
{


// static members

const double CTrackballCameraComp::s_zoomSpeed = 0.05;
const double CTrackballCameraComp::s_moveSpeed = 0.005;
const double CTrackballCameraComp::s_rotationSpeed = 1.0;


// public methods

CTrackballCameraComp::CTrackballCameraComp()
	:m_position(0.0, 0.0, 5.0),
	m_sceneBoundingCuboid(-1., 1., -1., 1., 1., -1.)
{
}


// reimplemented (imt3dview::IScene3dCamera)

const QVector3D& CTrackballCameraComp::GetPosition() const
{
	return m_position;
}


void CTrackballCameraComp::MoveTo(const QVector3D& position)
{
	m_position = position;
}


void CTrackballCameraComp::MoveTo(const QPoint& pointFrom, const QPoint& pointTo)
{
	QPoint mousePositionDiff = pointTo - pointFrom;

	m_position += QVector3D(-mousePositionDiff.x() * s_moveSpeed, mousePositionDiff.y() * s_moveSpeed, 0.0);
}


const QQuaternion& CTrackballCameraComp::GetRotation() const
{
	return m_rotation;
}


void CTrackballCameraComp::RotateTo(const QQuaternion& rotation)
{
	m_rotation = rotation;
}


void CTrackballCameraComp::RotateTo(const QPoint& pointFrom, const QPoint& pointTo, const QVector3D& axis)
{
	QVector3D rotationAxis;
	float rotationAngle = 0.0;

	if (!axis.isNull()){
		rotationAxis = axis;

		if (!qFuzzyIsNull(axis.x())){
			rotationAngle = (pointTo - pointFrom).y() * s_rotationSpeed;
		}
		else{
			rotationAngle = (pointTo - pointFrom).x() * s_rotationSpeed;
		}
	}
	else{
		// no rotation axis specified, do free rotation
		QVector3D point3dFrom = GetTrackballVector(pointFrom);
		QVector3D point3dTo = GetTrackballVector(pointTo);

		if ((point3dFrom - point3dTo).length() > FLT_MIN){
			rotationAxis = QVector3D::crossProduct(point3dFrom, point3dTo);
			rotationAngle = qRadiansToDegrees(std::asin(rotationAxis.length())) * s_rotationSpeed;

			m_rotation = QQuaternion::fromAxisAndAngle(rotationAxis.normalized(), rotationAngle) * m_rotation;
		}
	}

	m_rotation = QQuaternion::fromAxisAndAngle(rotationAxis.normalized(), rotationAngle) * m_rotation;
}


void CTrackballCameraComp::ZoomIn()
{
	m_position.setZ(m_position.z() - s_zoomSpeed);
}


void CTrackballCameraComp::ZoomOut()
{
	m_position.setZ(m_position.z() + s_zoomSpeed);
}


QMatrix4x4 CTrackballCameraComp::GetViewMatrix() const
{
	QMatrix4x4 viewMatrix;
	viewMatrix.translate(-m_position);
	viewMatrix.rotate(m_rotation);

	return viewMatrix;
}

void CTrackballCameraComp::SetBoundingCuboid(const imt3d::CCuboid & cuboid)
{
	if (cuboid.IsValid()) {
		if (cuboid.GetBottom() < m_sceneBoundingCuboid.GetBottom()) {
			m_sceneBoundingCuboid.SetBottom(cuboid.GetBottom());
		}

		if (cuboid.GetTop() > m_sceneBoundingCuboid.GetTop()) {
			m_sceneBoundingCuboid.SetTop(cuboid.GetTop());
		}

		if (cuboid.GetLeft() < m_sceneBoundingCuboid.GetLeft()) {
			m_sceneBoundingCuboid.SetLeft(cuboid.GetLeft());
		}

		if (cuboid.GetRight() > m_sceneBoundingCuboid.GetRight()) {
			m_sceneBoundingCuboid.SetRight(cuboid.GetRight());
		}

		if (cuboid.GetFar() < m_sceneBoundingCuboid.GetFar()) {
			m_sceneBoundingCuboid.SetFar(cuboid.GetFar());
		}

		if (cuboid.GetNear() > m_sceneBoundingCuboid.GetNear()) {
			m_sceneBoundingCuboid.SetNear(cuboid.GetNear());
		}
	}
}

imt3d::CCuboid CTrackballCameraComp::GetBoundingCuboid() const
{
	return m_sceneBoundingCuboid;
}


// protected methods

QVector3D CTrackballCameraComp::GetTrackballVector(const QPoint& point2d) const
{
	QVector3D point3d;

	point3d.setX(point2d.x() * 0.001f - 1.0f);
	point3d.setY(1.0f - point2d.y() * 0.001f);
	point3d.setZ(0.0);

	float sqrZ = 1.0f - QVector3D::dotProduct(point3d, point3d);

	if (sqrZ > 0.0f){
		point3d.setZ(std::sqrt(sqrZ));
	}
	else{
		point3d.normalize();
	}

	return point3d;
}


} // namespace imt3dview


