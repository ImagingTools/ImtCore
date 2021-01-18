#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtloggui
{


class IGraphicsItemProvider: virtual public istd::IChangeable
{
public:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItem;
	typedef QList<GraphicsItem> GraphicsItemList;

	virtual GraphicsItemList GetItems() const = 0;
	virtual GraphicsItemList GetAddedItems() const = 0;
	virtual GraphicsItemList GetRemovedItems() const = 0;
};


} // namespace imtloggui


