#include <imtloggui/CSingleLayerGroupComp.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtloggui
{


CSingleLayerGroupComp::CSingleLayerGroupComp()
	: m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_groupPtr(nullptr)
{
}


void CSingleLayerGroupComp::SetScene(QGraphicsScene* scenePtr)
{
	if (m_scenePtr != nullptr){
		DestroyGroupItem();
	}

	m_scenePtr = scenePtr;
	m_timeAxisPtr = nullptr;
}


void CSingleLayerGroupComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	if (m_timeAxisPtr == nullptr){
		m_timeAxisPtr = timeAxisPtr;
	}
}


bool CSingleLayerGroupComp::CreateGroupItem()
{
	if (m_groupPtr == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_groupPtr = new CEventGroupItem();

		if (m_groupColorAttrPtr.IsValid()){
			m_groupPtr->SetBackgroundColor(QString(*m_groupColorAttrPtr));
		}

		if (m_groupHeightAttrPtr.IsValid()){
			m_groupPtr->SetRect(QRectF(0, 0, 10000000, -*m_groupHeightAttrPtr));
		}

		m_scenePtr->addItem(m_groupPtr);
	}

	return false;
}


bool CSingleLayerGroupComp::DestroyGroupItem()
{
	if (m_groupPtr != nullptr && m_scenePtr != nullptr){
		ClearEvents();

		m_scenePtr->removeItem(m_groupPtr);
		delete m_groupPtr;
		m_groupPtr = nullptr;

		return true;
	}

	return false;
}


QByteArray CSingleLayerGroupComp::GetGroupId() const
{
	if (m_groupIdAttrPtr.IsValid()){
		return *m_groupIdAttrPtr;
	}

	return QByteArray();
}


QString CSingleLayerGroupComp::GetGroupName() const
{
	return m_groupName;
}


void CSingleLayerGroupComp::SetGroupName(QString name)
{
	m_groupName = name;
}


QGraphicsItem* CSingleLayerGroupComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	if (m_eventItemFactoryCompPtr.IsValid() && m_groupPtr){
		QGraphicsItem* itemPtr = nullptr;

		for (int i = 0; i < m_eventItemFactoryCompPtr.GetCount(); i++){
			itemPtr = m_eventItemFactoryCompPtr[i]->CreateInstance(messagePtr);
			if (itemPtr != nullptr){
				break;
			}
		}

		if (itemPtr == nullptr){
			return nullptr;
		}

		QPointF origin(m_timeAxisPtr->GetScenePositionFromTime(messagePtr->GetInformationTimeStamp()), 0);
		origin = m_groupPtr->mapFromScene(origin);
		origin.setY(-100);

		m_groupPtr->addToGroup(itemPtr);
		itemPtr->setPos(origin);
		itemPtr->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

		m_scenePtr->addItem(itemPtr);

		qDebug() << m_scenePtr->items().count() << m_groupPtr->childItems().count() << m_groupPtr->boundingRect();
		for (QGraphicsItem* item : m_groupPtr->childItems()){
			qDebug() << item->pos() << item->boundingRect();
		}

		m_events.append(itemPtr);

		return itemPtr;
	}

	return nullptr;
}


void CSingleLayerGroupComp::ClearEvents()
{
	for (QGraphicsItem* item : m_events){
		m_scenePtr->removeItem(item);
		delete item;
	}

	m_events.clear();
}


void CSingleLayerGroupComp::SetVisible(bool isVisible) const
{
	if (m_groupPtr != nullptr){
		m_groupPtr->setVisible(isVisible);
	}
}


void CSingleLayerGroupComp::TimeAxisChanged()
{

}


void CSingleLayerGroupComp::ViewPortChanged()
{

}


} // namespace imtloggui


