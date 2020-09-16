#include <imtloggui/CLoginEventItem.h>


// Qt includes
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/CLoginEvent.h>


namespace imtloggui
{


// public methods

void CLoginEventItem::SetParams(
			const QIcon& iconLogin,
			const QIcon& iconLogout,
			const QSize& iconSize,
			const ilog::IMessageConsumer::MessagePtr& message,
			QGraphicsItem* parentPtr)
{
	BaseClass::SetParams(message, parentPtr);

	m_iconLogin = iconLogin;
	m_iconLogout = iconLogout;
	m_iconSize = iconSize;
}


// reimplemented (idoc::IDocumentMetaInfo)

idoc::IDocumentMetaInfo::MetaInfoTypes CLoginEventItem::GetMetaInfoTypes(bool allowReadOnly) const
{
	idoc::IDocumentMetaInfo::MetaInfoTypes types = BaseClass::GetMetaInfoTypes();
	if (allowReadOnly){
		types.insert(MIT_ACTION);
		types.insert(MIT_USER_NAME);
	}

	return types;
}


QVariant CLoginEventItem::GetMetaInfo(int metaInfoType) const
{
	const imtlog::CLoginEvent* eventPtr = dynamic_cast<const imtlog::CLoginEvent*>(GetInformationProvider());
	Q_ASSERT(eventPtr != nullptr);

	switch (metaInfoType){
	case MIT_ACTION:
		if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGIN){
			return QObject::tr("Login");
		}
		else if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGOUT){
			return QObject::tr("Logout");
		}
	case MIT_USER_NAME:
		return eventPtr->GetLoginEventInfo().userName;
	default:
		return BaseClass::GetMetaInfo(metaInfoType);
	}
}


QString CLoginEventItem::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case MIT_ACTION:
		return QObject::tr("Action");
	case MIT_USER_NAME:
		return QObject::tr("User name");
	default:
		return BaseClass::GetMetaInfoName(metaInfoType);
	}
}


// reimplemented (QGraphicsItem)

QRectF CLoginEventItem::boundingRect() const
{
	QFontMetrics fontMetrics(m_font);

	QString user = GetInformationProvider()->GetInformationDescription();
	QRectF labelRect = fontMetrics.boundingRect(user);

	QRectF rect(0, 0, m_iconSize.width(), m_iconSize.height());
	rect.setWidth(qMax(labelRect.width(), rect.width()));
	rect.setHeight(labelRect.height() + rect.height());

	return QRectF(-rect.width() / 2, -rect.height() / 2, rect.width(), rect.height());
}


void CLoginEventItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	const imtlog::CLoginEvent* eventPtr = dynamic_cast<const imtlog::CLoginEvent*>(GetInformationProvider());
	if (eventPtr == nullptr){
		return;
	}

	if (painter->font().key() != m_font.key()){
		m_font = painter->font();
	}

	QFontMetrics fontMetrics(m_font);

	QString user = eventPtr->GetLoginEventInfo().userName;
	QRectF labelRect = fontMetrics.boundingRect(user);
	labelRect.setWidth(labelRect.width() + 1);
	const QRectF bounding = boundingRect();

	QIcon icon;

	if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGIN){
		icon = m_iconLogin;
	}
	else if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGOUT){
		icon = m_iconLogout;
	}

	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(QRect(-m_iconSize.width() / 2, bounding.top(), m_iconSize.width(), m_iconSize.height()), icon.pixmap(m_iconSize));
	painter->drawText(labelRect.translated(QPointF(-labelRect.width() / 2, bounding.bottom() - labelRect.bottom())), user);

	if (isSelected()){
		painter->setPen(QPen(Qt::red, 1));
		painter->drawRoundedRect(bounding.adjusted(-2, -2, 2, 2), 2, 2);
	}
}


} // namespace imtloggui


