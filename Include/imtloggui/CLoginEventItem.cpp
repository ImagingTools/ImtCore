#include <imtloggui/CLoginEventItem.h>


// Qt includes
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/CLoginEvent.h>


namespace imtloggui
{


// public methods

void CLoginEventItem::SetParams(
			const QPixmap& iconLogin,
			const QPixmap& iconLogout,
			const ilog::IMessageConsumer::MessagePtr& messagePtr,
			QGraphicsItem* parentPtr)
{
	BaseClass::SetParams(messagePtr, parentPtr);
	m_iconLogin = iconLogin;
	m_iconLogout = iconLogout;
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
	if (m_boundingRect.isNull()){
		const imtlog::CLoginEvent* eventPtr = dynamic_cast<const imtlog::CLoginEvent*>(GetInformationProvider());
		Q_ASSERT(eventPtr != nullptr);

		QFontMetrics fontMetrics(m_font);

		QString user = eventPtr->GetLoginEventInfo().userName;
		m_labelRect = fontMetrics.boundingRect(user);

		QRectF rect;

		if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGIN){
			rect = m_iconLogin.rect();
		}
		else{
			rect = m_iconLogout.rect();
		}

		rect.setWidth(qMax(m_labelRect.width(), rect.width()));
		rect.setHeight(m_labelRect.height() + rect.height());

		m_boundingRect = QRectF(-rect.width() / 2, -rect.height() / 2, rect.width(), rect.height());
	}

	return m_boundingRect;
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

	QString user = eventPtr->GetLoginEventInfo().userName;

	QPixmap icon;

	if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGIN){
		icon = m_iconLogin;
	}
	else if (eventPtr->GetLoginEventInfo().action == imtlog::CLoginEvent::ACT_LOGOUT){
		icon = m_iconLogout;
	}

	QPointF pixmapPos= m_boundingRect.topLeft();
	pixmapPos.rx() += ((m_boundingRect.width() - icon.width()) / 2);

	painter->drawPixmap(pixmapPos, icon);
	painter->drawText(m_labelRect.translated(QPointF(-m_labelRect.width() / 2, m_boundingRect.bottom() - m_labelRect.bottom())), user);

	if (isSelected()){
		painter->setPen(QPen(Qt::red, 1));
		painter->drawRoundedRect(m_boundingRect.adjusted(-2, -2, 2, 2), 2, 2);
	}
}


} // namespace imtloggui


