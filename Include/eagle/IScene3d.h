#pragma once


// Qt includes
#include <QtCore/QByteArray>

// Eagle includes
#include <eagle/IScene3dItem.h>
#include <eagle/IDrawable.h>


namespace eagle
{


/**
	Common interface for a 3D-scene.
*/
class IScene3d: virtual public IDrawable
{
public:
	enum ShapeFlags
	{
		SF_NONE = 0
	};

	/**
		Set scene camera.
	*/
	virtual void SetCamera(const IScene3dCamera* cameraPtr) = 0;

	/**
		Set scene projection matrix.
	*/
	virtual void SetProjection(const QMatrix4x4& projection) = 0;

	/**
		Get scene bounding cuboid.
	*/
	virtual const imt3d::CCuboid& GetBoundingCuboid() const = 0;

	/**
		Get the list of shape IDs existing in the scene.
	*/
	virtual QByteArrayList GetShapeIds() const = 0;

	/**
		Get a scene shape by ID.
	*/
	virtual eagle::IScene3dItem* GetShapePtr(const QByteArray& id) const = 0;

	/**
		Add an 3D-shape to the scene.
		\return	If successfull, the method returns the UUID of the attached 3D-object, or an empty ID otherwise.
	*/
	virtual QByteArray AddShapeToScene(eagle::IScene3dItem* objectPtr, int shapeFlags = SF_NONE) = 0;

	/**
		Remove an existing 3D-shape from the scene.
	*/
	virtual void RemoveShapeFromScene(eagle::IScene3dItem* objectPtr) = 0;
};


} // namespace eagle


