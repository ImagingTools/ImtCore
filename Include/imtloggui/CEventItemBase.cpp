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

CEventItemBase::CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent)
	:BaseClass(parent)
{
	m_messagePtr = message;
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


const QColor& CEventItemBase::GetBackgroundColor(const QColor& color)
{
	return m_backgroundColor;
}


void CEventItemBase::SetBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
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


