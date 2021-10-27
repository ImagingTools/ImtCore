#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imt3dview/IScene3dCamera.h>
#include <imt3dgui/CShape3dBase.h>


namespace imt3dview
{


/**
	Trackball camera component implementation.
*/
class CTrackballCameraComp:
			public imt3dview::IScene3dCamera,
			public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTrackballCameraComp);
		I_REGISTER_INTERFACE(imt3dview::IScene3dCamera);
	I_END_COMPONENT;

	CTrackballCameraComp();

	// reimplemented (imt3dview::IScene3dCamera)
	virtual const QVector3D& GetPosition() const override;
	virtual void MoveTo(const QVector3D& position) override;
	virtual void MoveTo(const QPoint& pointFrom, const QPoint& pointTo) override;

	virtual const QQuaternion& GetRotation() const override;
	virtual void RotateTo(const QQuaternion& rotation) override;
	virtual void RotateTo(const QPoint& pointFrom, const QPoint& pointTo, const QVector3D& axis = QVector3D()) override;

	virtual void ZoomIn() override;
	virtual void ZoomOut() override;
	virtual QMatrix4x4 GetViewMatrix() const override;

	virtual void SetBoundingCuboid(const imt3d::CCuboid& cuboid) override;
	virtual imt3d::CCuboid GetBoundingCuboid() const override;
private:
	QVector3D GetTrackballVector(const QPoint& point2d) const;

private:
	QVector3D m_position;
	QQuaternion m_rotation;

	static const double s_zoomSpeed;
	static const double s_moveSpeed;
	static const double s_rotationSpeed;
	imt3d::CCuboid m_sceneBoundingCuboid;
};


} // namespace imt3dview


