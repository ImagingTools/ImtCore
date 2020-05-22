#pragma once


// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CGeneralEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	CGeneralEventItem(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent = nullptr);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};


} // namespace imtloggui


