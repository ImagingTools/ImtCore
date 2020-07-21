#include <imtgui/CApplicationChangedEventItem.h>


// Qt includes
#include <QtGui/QPainter>

// ImtCore includes
#include <imtgui/CApplicationChangedEvent.h>


namespace imtgui
{


// public methods

CApplicationChangedEventItem::CApplicationChangedEventItem(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent)
	:BaseClass(message, parent),
	m_font(QFont())
{
	m_messagePtr = message;
}


void CApplicationChangedEventItem::SetFont(const QFont& font)
{
	m_font = font;
}


// reimplemented (QGraphicsItem)

QRectF CApplicationChangedEventItem::boundingRect() const
{
	QFontMetrics fontMetrics(m_font);

	QString application;
	QString user;

	const CApplicationChangedEvent* messagePtr = dynamic_cast<const CApplicationChangedEvent*>((m_messagePtr.GetPtr()));
	if (messagePtr != nullptr){
		user = messagePtr->GetApplicationChangesInfo().userName;
		application = messagePtr->GetApplicationChangesInfo().applicationName;
	}

	QRectF applicationRect = fontMetrics.boundingRect(application);
	QRectF userRect = fontMetrics.boundingRect(user);


	QRectF rect(0, 0, m_iconSize.width(), m_iconSize.height());
	rect.setWidth(qMax(rect.width(), qMax(userRect.width(), applicationRect.width())));
	rect.setHeight(rect.height() + userRect.height() + applicationRect.height());

	return QRectF(-rect.width() / 2, -rect.height() / 2, rect.width(), rect.height());
}


void CApplicationChangedEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	QFontMetrics fontMetrics(painter->font());

	QString application;
	QString user;

	const CApplicationChangedEvent* messagePtr = dynamic_cast<const CApplicationChangedEvent*>((m_messagePtr.GetPtr()));
	if (messagePtr != nullptr){
		application = messagePtr->GetApplicationChangesInfo().applicationName;
		user = messagePtr->GetApplicationChangesInfo().userName;
	}

	QRectF applicationRect = fontMetrics.boundingRect(application);
	QRectF userRect = fontMetrics.boundingRect(user);

	applicationRect.setWidth(applicationRect.width() + 1);
	userRect.setWidth(userRect.width() + 1);

	const QRectF bounding = boundingRect();
	
	applicationRect.translate(QPointF(-applicationRect.width() / 2, bounding.bottom() - applicationRect.bottom() - userRect.height()));
	userRect.translate(QPointF(-userRect.width() / 2, bounding.bottom() - userRect.bottom()));

	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(QRect(-m_iconSize.width() / 2, bounding.top(), m_iconSize.width(), m_iconSize.height()), m_icon.pixmap(m_iconSize));
	painter->drawText(applicationRect, application);
	painter->drawText(userRect, user);

	if (isSelected()){
		painter->setPen(QPen(Qt::red, 1));
		painter->drawRoundedRect(bounding.adjusted(-2, -2, 2, 2), 2, 2);
	}
}


} // namespace imtgui


