#include <imtloggui/CIconBasedEventItem.h>


// Qt includes
#include <QtGui/QPainter>


namespace imtloggui
{


// public methods

void CIconBasedEventItem::SetIcon(const QIcon& icon)
{
	m_icon = icon;
}


void CIconBasedEventItem::SetIconSize(const QSize& size)
{
	m_iconSize = size;
}


// reimplemented (QGraphicsItem)

QRectF CIconBasedEventItem::boundingRect() const
{
	return QRectF(-m_iconSize.width() / 2, -m_iconSize.height() / 2, m_iconSize.width(), m_iconSize.height());
}


void CIconBasedEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(boundingRect().toRect(), m_icon.pixmap(m_iconSize));

	if (isSelected()){
		painter->setPen(QPen(Qt::red, 1));
		painter->drawRoundedRect(boundingRect().adjusted(-2, -2, 2, 2), 2, 2);
	}
}


} // namespace imtloggui


