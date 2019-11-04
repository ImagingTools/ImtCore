#pragma once


// Qt includes
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

// ImtCore includes
#include <imt3dview/IDrawable.h>
#include <imt3dview/IScene3dCamera.h>


namespace imt3dview
{


/**
	Basic drawable object interface
*/
class IScene3dItem: virtual public IDrawable
{
public:
	virtual bool IsValid() const = 0;

	virtual void SetCamera(const IScene3dCamera* cameraPtr) = 0;
	virtual void SetProjection(const QMatrix4x4& projection) = 0;

	virtual const QVector3D& GetPosition() const = 0;
	virtual void SetPosition(const QVector3D& position) = 0;

	virtual const QQuaternion& GetRotation() const = 0;
	virtual void SetRotation(const QQuaternion& rotation) = 0;

	virtual const QVector3D& GetScale() const = 0;
	virtual void SetScale(const QVector3D& scale) = 0;
};


} // namespace imt3dview


