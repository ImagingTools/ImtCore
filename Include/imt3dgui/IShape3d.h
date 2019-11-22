#pragma once


// ImtCore includes
#include <imt3dview/IScene3dItem.h>


namespace imt3dgui
{


/**
	Basic drawable shape interface
*/
class IShape3d: virtual public imt3dview::IScene3dItem
{
public:
	enum ColorMode
	{
		CM_UNKNOWN = -1,
		CM_POINT = 0,	// one color for each item point
		CM_ITEM = 1,	// one color for entire item
	};

	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool isVisible) = 0;

protected:
	virtual void OnContextInitialized(QOpenGLContext* contextPtr) = 0;
	virtual void OnContextDestroyed(QOpenGLContext* contextPtr) = 0;
	virtual ColorMode GetColorMode() const = 0;
	virtual QVector3D GetColor() const = 0;
	virtual bool HasNormals() const = 0;
};


} // namespace imt3dgui


