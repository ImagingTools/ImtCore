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
	m_messagePtr = message;
	setParentItem(parentPtr);
}


// reimplemented (imtloggui::IEventItem)

const istd::IInformationProvider* CEventItemBase::GetInformationProvider() const
{
	return m_messagePtr.GetPtr();
}


const CEventItemBase::MetaInfo& CEventItemBase::GetMetaInfo() const
{
	return m_metaInfo;
}


void CEventItemBase::SetMetaInfo(const QString& key, const QString& value)
{
	MetaInfoItem metaInfoItem;
	metaInfoItem.key = key;
	metaInfoItem.value = value;

	for (int i = 0; i < m_metaInfo.count(); i++){
		if (m_metaInfo[i].key == key){
			if (m_metaInfo[i].value != value){
				m_metaInfo[i].value = value;

				UpdateToolTip();
			}
			return;
		}
	}

	m_metaInfo.append(metaInfoItem);

	UpdateToolTip();
}


void CEventItemBase::RemoveMetaInfo(const QString& key)
{
	for (int i = 0; i < m_metaInfo.count(); i++){
		if (m_metaInfo[i].key == key){
			m_metaInfo.remove(i);

			UpdateToolTip();

			return;
		}
	}
}


// reimplemented (idoc::IDocumentMetaInfo)

idoc::IDocumentMetaInfo::MetaInfoTypes CEventItemBase::GetMetaInfoTypes(bool allowReadOnly) const
{
	MetaInfoTypes types;

	for (int i = 0; i < m_metaInfo.count(); i++){
		types.insert(i + idoc::IDocumentMetaInfo::MIT_USER);
	}

	return types;
}


QVariant CEventItemBase::GetMetaInfo(int metaInfoType) const
{
	metaInfoType -= idoc::IDocumentMetaInfo::MIT_USER;
	if (metaInfoType >= 0 && metaInfoType < m_metaInfo.count()){
		return m_metaInfo[metaInfoType].value;
	}

	return QVariant();
}


bool CEventItemBase::SetMetaInfo(int metaInfoType, const QVariant& metaInfo)
{
	return false;
}


QString CEventItemBase::GetMetaInfoName(int metaInfoType) const
{
	metaInfoType -= idoc::IDocumentMetaInfo::MIT_USER;
	if (metaInfoType >= 0 && metaInfoType < m_metaInfo.count()){
		return m_metaInfo[metaInfoType].key;
	}

	return QString();
}


QString CEventItemBase::GetMetaInfoDescription(int metaInfoType) const
{
	return QString();
}


bool CEventItemBase::IsMetaInfoWritable(int metaInfoType) const
{
	return false;
}


// protected methods

void CEventItemBase::UpdateToolTip()
{
	QString toolTip;

	for (MetaInfoItem metaInfoItem : m_metaInfo){
		toolTip += metaInfoItem.key + ": " + metaInfoItem.value + "\n";
	}

	if (!toolTip.isEmpty()){
		toolTip.chop(1);
	}

	setToolTip(toolTip);
}


} // namespace imtloggui


