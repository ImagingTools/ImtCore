#pragma once


// Qt includes
#include <QtCore/QMargins>
#include <QtCore/QRect>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/TRange.h>


namespace imtloggui
{


class IViewPropertyProvider: virtual public istd::IChangeable
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


