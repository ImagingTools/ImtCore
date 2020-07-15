#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


/**
	Interface for view controller.
*/
class IGraphicsViewController: virtual public istd::IPolymorphic
{
public:
	/**
		Get zoom factors for X and Y of view.
	*/
	virtual double GetZoomFactorX() const = 0;
	virtual double GetZoomFactorY() const = 0;

	/**
		Set zoom factors for X and Y of view.
	*/
	virtual bool SetZoomFactors(double zoomFactorX, double zoomFactorY) = 0;

	/**
		Get percentage scrolling steps (by keys)
	*/
	virtual double GetPercentageScrollingStepX() const = 0;
	virtual double GetPercentageScrollingStepY() const = 0;

	/**
		Set percentage scrolling steps (by keys)
	*/
	virtual bool SetPercentageScrollingSteps(double scrollingStepX, double scrollingStepY) = 0;

	/**
		Scroll view
	*/
	virtual bool Scroll(double dx, double dy) = 0;

	/**
		Zoom view
	*/
	virtual bool Zoom(const QPointF& zoomOrigin, double zoomFactorX, double zoomFactorY) = 0;
};


} // namespace imtloggui


