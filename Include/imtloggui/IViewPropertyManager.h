#pragma once


// ImtCore includes
#include <imtloggui/IViewPropertyProvider.h>


namespace imtloggui
{


class IViewPropertyManager: virtual public IViewPropertyProvider
{
public:
	virtual bool SetSceneRect(const QRectF& rect) = 0;
	virtual bool SetViewRect(const QRectF& rect) = 0;
	virtual bool SetMargins(const QMargins& margins) = 0;
	virtual bool SetScaleRangeX(const istd::CRange& range) = 0;
	virtual bool SetScaleRangeY(const istd::CRange& range) = 0;
};


} // namespace imtloggui


