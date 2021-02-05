#include <imtloggui/CIconBasedEventItem.h>


// Qt includes
#include <QtGui/QPainter>


namespace imtloggui
{


// public methods

void CIconBasedEventItem::SetParams(
			const QPixmap& icon,
			const ilog::IMessageConsumer::MessagePtr& messagePtr,
			QGraphicsItem* parentPtr)
{
	BaseClass::SetParams(messagePtr, parentPtr);
	m_icon = icon;
}


// reimplemented (QGraphicsItem)

QRectF CIconBasedEventItem::boundingRect() const
{
	if (m_boundingRect.isNull()){
		m_boundingRect = QRectF(-m_icon.width() / 2, -m_icon.height() / 2, m_icon.width(), m_icon.height());
	}

	return m_boundingRect;
}


void CIconBasedEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(m_boundingRect.topLeft(), m_icon);

	if (isSelected()){
		painter->setPen(QPen(Qt::red, 1));
		painter->drawRoundedRect(boundingRect().adjusted(-2, -2, 2, 2), 2, 2);
	}
}


} // namespace imtloggui


