#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem.h>

// ACF includes
#include <istd/IPolymorphic.h>


class QGraphicsItems;

namespace imtloggui
{


class IGraphicsController: virtual public istd::IPolymorphic
{
public:
	typedef QVector<QGraphicsItems*> GraphicsItemList;

	virtual GraphicsItemList GetPermanentGraphicsItems() const = 0;
};


} // namespace imtloggui


