#pragma once


// ImtCore includes
#include <imt3dview/IScene3dCamera.h>
#include <imt3dgui/CShape3dBase.h>


namespace imt3dview
{


/**
	Static view camera.
*/
class CTrackballCamera: public imt3dview::IScene3dCamera
{
public:
	CTrackballCamera();

	// reimplement (imt3dview::IScene3dCamera)
	void SetPlanes(float nearPlane, float farPlane);
	void SetViewPortSize(const QSize& size) override;

	const QVector3D& GetPosition() const override;
	void MoveTo(const QVector3D& position) override;
	void MoveTo(const QPoint& pointFrom, const QPoint& pointTo) override;

	const QQuaternion& GetRotation() const override;
	void RotateTo(const QQuaternion& rotation) override;
	void RotateTo(const QPoint& pointFrom, const QPoint& pointTo) override;

	void ZoomIn() override;
	void ZoomOut() override;
	QMatrix4x4 GetViewMatrix() const override;

private:
	QVector3D GetTrackballVector(const QPoint& point2d) const;

private:
	float m_nearPlane;
	float m_farPlane;
	QVector3D m_position;
	QQuaternion m_rotation;
	QSize m_viewPortSize;

	static const double s_zoomSpeed;
	static const double s_moveSpeed;
	static const double s_rotationSpeed;
};


} // namespace imt3dview


