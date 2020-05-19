#include <imtloggui/CLoginEventItem.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>


namespace imtloggui
{


// public methods

CLoginEventItem::CLoginEventItem(ilog::IMessageConsumer::MessagePtr message, QGraphicsItem* parent)
	: BaseClass(message, parent)
{
	m_messagePtr = message;
}


void CLoginEventItem::SetFont(const QFont& font)
{
	m_font = font;
}


void CLoginEventItem::SetIcons(const QIcon& iconLogin, const QIcon& iconLogout)
{
	m_iconLogin = iconLogin;
	m_iconLogout = iconLogout;
}


void CLoginEventItem::SetIconSize(const QSize& size)
{
	m_iconSize = size;
}


// reimplemented (QGraphicsItem)

QRectF CLoginEventItem::boundingRect() const
{
	QFontMetrics fontMetrics(m_font);

	QString user = m_messagePtr->GetInformationDescription();
	QRectF labelRect = fontMetrics.boundingRect(user);

	QRectF rect(0, 0, m_iconSize.width(), m_iconSize.height());
	rect.setWidth(qMax(labelRect.width(), rect.width()));
	rect.setHeight(labelRect.height() + rect.height());

	return QRectF(-rect.width() / 2, -rect.height() / 2, rect.width(), rect.height());
}


void CLoginEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QFontMetrics fontMetrics(m_font);

	QString user = m_messagePtr->GetInformationDescription();
	QRectF labelRect = fontMetrics.boundingRect(user);
	labelRect.setWidth(labelRect.width() + 1);
	const QRectF bounding = boundingRect();

	QIcon icon;

	if (m_messagePtr->GetInformationId() == imtlog::IMessageGroupInfoProvider::MI_USER_LOGIN){
		icon = m_iconLogin;
	}
	else if (m_messagePtr->GetInformationId() == imtlog::IMessageGroupInfoProvider::MI_USER_LOGOUT){
		icon = m_iconLogout;
	}

	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(QRect(-m_iconSize.width() / 2, bounding.top(), m_iconSize.width(), m_iconSize.height()), icon.pixmap(m_iconSize));
	//painter->setPen(Qt::red);
	//painter->drawRect(labelRect.translated(QPointF(-labelRect.width() / 2, bounding.bottom() - labelRect.bottom())));
	//painter->setPen(Qt::black);
	painter->drawText(labelRect.translated(QPointF(-labelRect.width() / 2, bounding.bottom() - labelRect.bottom())), user);
	qDebug() << labelRect;
}


} // namespace imtloggui


