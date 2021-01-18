#pragma once


// Qt Includes
#include <QtWidgets/QGraphicsItem.h>

// ACF Includes
#include <istd/TSmartPtr.h>
#include <imod/IModel.h>


namespace imtloggui
{


class IRepresentationView: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItem;
	typedef QList<GraphicsItem> GraphicsItemList;

	virtual bool SetModelPtr(imod::IModel* modelPtr) = 0;
	virtual GraphicsItemList GetGraphicItems() const = 0;
};


} // namespace imtloggui


