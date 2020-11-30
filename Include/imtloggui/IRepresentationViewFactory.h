#pragma once


// Qt Includes
#include <QtCore/QDateTime.h>
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtloggui
{


class IRepresentationViewFactory: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItem;
	typedef QList<GraphicsItem> GraphicsItemList;

	virtual GraphicsItemList CreateGraphicItems(const istd::TSmartPtr<istd::IChangeable> objectPtr, const QByteArray& groupId) const = 0;
};


} // namespace imtloggui


