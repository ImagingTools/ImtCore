#pragma once


// Qt includes
#include <QtCore/QMargins.h>
#include <QtCore/QRect.h>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtloggui
{


class IViewPropertyManager: virtual public istd::IPolymorphic
{
public:
	virtual bool SetSceneRect(const QRectF& rect) = 0;
	virtual bool SetViewRect(const QRectF& rect) = 0;
	virtual bool SetMargins(const QMargins& margins) = 0;
	virtual bool SetScaleRangeX(const istd::CRange& scaleX) = 0;
	virtual bool SetScaleRangeY(const istd::CRange& scaleY) = 0;
};


} // namespace imtloggui


