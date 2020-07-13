#pragma once


// Qt includes
#include <QtCore/QMargins>

// ACF includes
#include <istd/TRange.h>

// ImtBase
#include <imtloggui/IViewRectProvider.h>


namespace imtloggui
{


/**
	Interface for view model
*/
class IGraphicsViewModel: virtual public IViewRectProvider
{
public:
	/**
		Get the area of the scene that can be visualized
	*/
	virtual QRectF GetSceneRect() const = 0;

	/**
		Set the area of the scene that can be visualized
	*/
	virtual bool SetSceneRect(const QRectF& rect) = 0;

	/**
		Set the area of the scene that must be visualized
	*/
	virtual bool SetViewRect(const QRectF& rect) = 0;

	/**
		Get scene margins visualized by the view in pixels
	*/
	virtual QMargins GetMargins() const = 0;

	/**
		Set scene margins visualized by the view in pixels
	*/
	virtual bool SetMargins(const QMargins& margins) = 0;

	/**
		Get current view scale
	*/
	virtual double GetScaleX() const = 0;
	virtual double GetScaleY() const = 0;

	/**
		Get scale range
	*/
	virtual istd::CRange GetScaleXRange() const = 0;
	virtual istd::CRange GetScaleYRange() const = 0;

	/**
		Set scale range
	*/
	virtual bool SetScaleXRange(const istd::CRange& range) = 0;
	virtual bool SetScaleYRange(const istd::CRange& range) = 0;

	/**
		Scroll view
		If inPercents = false scrolling in scene coordinates 
		If inPercents = true scrolling in percents of viewrect size
	*/
	virtual bool Scroll(const QPointF& delta, bool inPercents = false) = 0;

	/**
		Zoom view
	*/
	virtual bool Zoom(const QPointF& factors, const QPointF& zoomOrigin) = 0;
};


} // namespace imtloggui


