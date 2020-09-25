#pragma once


// Qt includes
#include <QtCore/QMargins.h>
#include <QtCore/QRect.h>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtloggui
{


class IViewPropertyProvider: virtual public istd::IPolymorphic
{
public:
	virtual QRectF GetSceneRect() const = 0;
	virtual QRectF GetViewRect() const = 0;
	virtual QMargins GetMargins() const = 0;
	virtual istd::CRange GetScaleRangeX() const = 0;
	virtual istd::CRange GetScaleRangeY() const = 0;
	virtual double GetScaleX() const = 0;
	virtual double GetScaleY() const = 0;
};


} // namespace imtloggui


