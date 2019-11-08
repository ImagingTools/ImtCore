#pragma once

// Qt includes
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>
#include <QtCore/QRect>


namespace imt3dgui {

/**
	Scene event handler Interface
*/
class ISceneEventHandler
{
public:
	virtual void OnCameraPoseChanged(const QQuaternion& rotation, const QVector3D& position) = 0;
	virtual void OnShowGrid(bool show) = 0;
	virtual void OnShowAxis(bool show) = 0;
	virtual void OnNoSelection() = 0;
	virtual void OnPointSelection(const QPoint& point, bool clearPreviousSelection) = 0;
	virtual void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnClearSelection() = 0;
	virtual void OnAllSelection() = 0;
	virtual void OnInvertSelection() = 0;
	virtual void OnDeleteSelection() = 0;
	virtual void OnBoxFromSelection() = 0;
	virtual void OnFreeRotation() = 0;
	virtual void OnRotationAroundX() = 0;
	virtual void OnRotationAroundY() = 0;
	virtual void OnRotationAroundZ() = 0;
};

} //namespace imt3dgui
