#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ImtCore includes
#include <ilog/IMessageConsumer.h>
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


class CItemBase:
			public QGraphicsItem,
			virtual public IItemBase
{
public:
	typedef QGraphicsItem BaseClass;

	// reimplemented (imtloggui::IItemBase)
	virtual bool CollidesWithItem(const IItemBase* otherPtr, double scale) const override;
};


} // namespace imtloggui


