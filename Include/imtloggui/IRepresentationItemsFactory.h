#pragma once


// Qt Includes
#include <QtCore/QDateTime.h>
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtloggui
{


class IRepresentationItemsFactory: virtual public istd::IPolymorphic
{
public:
	struct TimeRange
	{
		QDateTime beginTime;
		QDateTime endTime;
	};

	typedef QVector<const QGraphicsItem*> GraphicsItemList;

	virtual GraphicsItemList CreateGraphicItems(const istd::IChangeable* objectPtr) const = 0;
};


} // namespace imtloggui


