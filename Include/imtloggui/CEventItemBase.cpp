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


CEventItemBase::CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent)
	:BaseClass(parent)
{
	m_messagePtr = message;
}


QDateTime CEventItemBase::GetEventTimeStamp() const
{
	if (m_messagePtr.IsValid()){
		return m_messagePtr->GetInformationTimeStamp();
	}

	return QDateTime();
}


const CEventItemBase::MetaInfo& CEventItemBase::GetMetaInfo() const
{
	return m_metaInfo;
}


void CEventItemBase::AddMetaInfo(const MetaInfoItem& metaInfoItem)
{
	m_metaInfo.append(metaInfoItem);
}


void CEventItemBase::AddMetaInfo(const QString& key, const QString& value)
{
	MetaInfoItem metaInfoItem;
	metaInfoItem.key = key;
	metaInfoItem.value = value;
	m_metaInfo.append(metaInfoItem);
}


} // namespace imtloggui


