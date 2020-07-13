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
	virtual QPointF GetZoomFactors() const = 0;

	/**
		Set zoom factors for X and Y of view.
	*/
	virtual bool SetZoomFactors(const QPointF& factors) = 0;

	/**
		Get percentage scrollings steps for scrolling by keys
	*/
	virtual QPointF GetPercentageScrollingSteps() const = 0;

	/**
		Set percentage scrollings step for scrolling by keys
	*/
	virtual bool SetPercentageScrollingSteps(const QPointF& steps) = 0;
};


} // namespace imtloggui


