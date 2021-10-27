#pragma once


// Qt includes
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

// ACF includes
#include <istd/IPolymorphic.h>

// Imt includes
#include <imt3d/CCuboid.h>

namespace imt3dview
{


/**
	Basic 3D-scene-camera interface
*/
class IScene3dCamera: virtual public istd::IPolymorphic
{
public:

	virtual const QVector3D& GetPosition() const = 0;
	virtual void MoveTo(const QVector3D& position) = 0;
	virtual void MoveTo(const QPoint& pointFrom, const QPoint& pointTo) = 0;

	virtual const QQuaternion& GetRotation() const = 0;
	virtual void RotateTo(const QQuaternion& rotation) = 0;
	virtual void RotateTo(const QPoint& pointFrom, const QPoint& pointTo, const QVector3D& axis = QVector3D()) = 0;

	virtual void ZoomIn() = 0;
	virtual void ZoomOut() = 0;
	virtual QMatrix4x4 GetViewMatrix() const = 0;

	virtual void SetBoundingCuboid(const imt3d::CCuboid& cuboid) = 0;
	virtual imt3d::CCuboid GetBoundingCuboid() const = 0;
};


} // namespace imt3dview


