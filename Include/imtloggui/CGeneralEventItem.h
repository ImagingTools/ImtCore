#pragma once


// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CGeneralEventItem: public CEventItemBase
{
	Q_DISABLE_COPY_MOVE(CGeneralEventItem);

public:
	typedef CEventItemBase BaseClass;

	CGeneralEventItem(ilog::IMessageConsumer::MessagePtr message, QGraphicsItem* parent = nullptr);

	// reimplemented (QGraphicsItem)
	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};


} // namespace imtloggui


