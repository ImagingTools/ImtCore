#include <imtloggui/CIconBasedEventItem.h>


// Qt includes
#include <QtGui/QPainter>


namespace imtloggui
{


// public methods

CIconBasedEventItem::CIconBasedEventItem(ilog::IMessageConsumer::MessagePtr message, QGraphicsItem* parent)
	: BaseClass(message, parent)
{
	m_messagePtr = message;
	setToolTip(m_messagePtr->GetInformationDescription());
}


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


void CIconBasedEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(boundingRect().toRect(), m_icon.pixmap(m_iconSize));
}


} // namespace imtloggui


