#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <imtloggui/IChartComponent.h>
#include <ilog/IMessageConsumer.h>


namespace imtloggui
{


class CEventItemBase: public QGraphicsObject
{
public:
	typedef QGraphicsObject BaseClass;

	CEventItemBase(QGraphicsItem *parent = nullptr);

	void SetColor(QColor color);

	ilog::IMessageConsumer::MessagePtr GetMessage();
	void SetMessage(ilog::IMessageConsumer::MessagePtr messagePtr);

	// reimplemented (QGraphicsItem)
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
	QColor m_color;
	QRectF m_boundingRect;
};


} // namespace imtloggui


