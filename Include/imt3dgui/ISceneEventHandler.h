#pragma once


// Qt includes
#include <QtCore/QRect>
#include <QtGui/QMouseEvent>


namespace imt3dgui
{


/**
	Scene event handler Interface
*/
class ISceneEventHandler
{
public:
	virtual void OnShowGrid(bool show) = 0;
	virtual void OnShowAxis(bool show) = 0;
	virtual void OnShowRuler(bool show) = 0;
	virtual void OnPointSelection(const QPoint& point, bool clearPreviousSelection) = 0;
	virtual void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnClearSelection() = 0;
	virtual void OnAllSelection() = 0;
	virtual void OnInvertSelection() = 0;
	virtual void OnDeleteSelection() = 0;
	virtual bool OnMousePress(QMouseEvent& e) = 0;
	virtual bool OnMouseMove(QMouseEvent& e) = 0;
	virtual bool OnMouseRelease(QMouseEvent& e) = 0;
};


} //namespace imt3dgui


