#include <imtloggui/CEventItemBase.h>


// Qt includes
#include <QtCore/QRectF>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>


namespace imtloggui
{


// public methods

void CEventItemBase::SetParams(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parentPtr)
{
	BaseClass::SetParams(parentPtr);

	m_messagePtr = message;
}


// reimplemented (imtloggui::IEventItem)

const istd::IInformationProvider* CEventItemBase::GetInformationProvider() const
{
	return m_messagePtr.GetPtr();
}


// reimplemented (idoc::IDocumentMetaInfo)

idoc::IDocumentMetaInfo::MetaInfoTypes CEventItemBase::GetMetaInfoTypes(bool allowReadOnly) const
{
	if (allowReadOnly){
		return {MIT_TIMESTAMP, MIT_SOURCE, MIT_MESSAGE, MIT_STATUS};
	}

	return {};
}


QVariant CEventItemBase::GetMetaInfo(int metaInfoType) const
{
	switch (metaInfoType){
	case MIT_TIMESTAMP:
		return m_messagePtr->GetInformationTimeStamp();
	case MIT_SOURCE:
		return m_messagePtr->GetInformationSource();
	case MIT_MESSAGE:
		return m_messagePtr->GetInformationDescription();
	case MIT_STATUS:
		{
			switch (m_messagePtr->GetInformationCategory()){
			case istd::IInformationProvider::IC_NONE:
				return QObject::tr("UNKNOWN");
			case istd::IInformationProvider::IC_INFO:
				return QObject::tr("OK");
			case istd::IInformationProvider::IC_WARNING:
				return QObject::tr("WARNING");
			case istd::IInformationProvider::IC_ERROR:
				return QObject::tr("ERROR");
			case istd::IInformationProvider::IC_CRITICAL:
				return QObject::tr("CRITICAL");
			}
		};
	default:
		return QVariant();
	}
}


QString CEventItemBase::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case MIT_TIMESTAMP:
		return QObject::tr("Timestamp");
	case MIT_SOURCE:
		return QObject::tr("Source");
	case MIT_MESSAGE:
		return QObject::tr("Message");
	case MIT_STATUS:
		return QObject::tr("Status");
	default:
		return QString();
	}
}


} // namespace imtloggui


