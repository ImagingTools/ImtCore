#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/IChangeable.h>


namespace imtloggui
{


class IGraphicsItemProvider: virtual public istd::IChangeable
{
public:
	typedef QList<QGraphicsItem*> GraphicsItemList;

	virtual GraphicsItemList GetAddedItems() const = 0;
	virtual GraphicsItemList GetRemovedItems() const = 0;
};


} // namespace imtloggui


