#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/IPolymorphic.h>
#include <istd/TSmartPtr.h>


namespace imtloggui
{


class IGraphicsItemProvider: virtual public istd::IPolymorphic
{
public:
	typedef QList<QGraphicsItem*> GraphicsItemList;

	virtual GraphicsItemList GetGraphicsItems() const = 0;
};


} // namespace imtloggui


