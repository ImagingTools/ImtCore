#include <imtloggui/CEventItemBase.h>


// Qt includes
#include <QDebug>
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


CEventItemBase::CEventItemBase(QGraphicsItem *parent)
	: BaseClass(parent)
{
}


void CEventItemBase::SetColor(QColor color)
{
	m_color = color;
}


ilog::IMessageConsumer::MessagePtr CEventItemBase::GetMessage()
{
	return m_messagePtr;
}


void CEventItemBase::SetMessage(ilog::IMessageConsumer::MessagePtr messagePtr)
{
	m_messagePtr = messagePtr;
}


// reimplemented (QGraphicsItem)

QRectF CEventItemBase::boundingRect() const
{
	return QRectF(-20,-20,40,40);
}


void CEventItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QBrush brush(m_color);
	painter->setBrush(brush);
	painter->drawEllipse(-20, -20, 40, 40);
}


} // namespace imtloggui


