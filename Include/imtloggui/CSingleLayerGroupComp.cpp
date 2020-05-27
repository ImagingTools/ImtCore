#include <imtloggui/CSingleLayerGroupComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


CSingleLayerGroupComp::CSingleLayerGroupComp()
	:m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_graphicsItem(nullptr)
{
}


// reimplemented (imtloggui::IEventGroupController)

void CSingleLayerGroupComp::SetScene(QGraphicsScene* scenePtr)
{
	if (m_scenePtr != nullptr){
		DestroyGraphicsItem();
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


bool CSingleLayerGroupComp::CreateGraphicsItem()
{
	if (m_graphicsItem == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_graphicsItem = new CEventGroupItem();

		m_graphicsItem->SetGroupName(m_groupName);

		if (m_groupColorAttrPtr.IsValid()){
			m_graphicsItem->SetBackgroundColor(QString(*m_groupColorAttrPtr));
		}

		if (m_groupHeightAttrPtr.IsValid()){
			m_graphicsItem->setRect(QRectF(0, 0, 0, -*m_groupHeightAttrPtr));
		}

		m_scenePtr->addItem(m_graphicsItem);
	}

	return false;
}


bool CSingleLayerGroupComp::DestroyGraphicsItem()
{
	if (m_graphicsItem != nullptr && m_scenePtr != nullptr){
		ClearEvents();

		m_scenePtr->removeItem(m_graphicsItem);
		delete m_graphicsItem;
		m_graphicsItem = nullptr;

		return true;
	}

	return false;
}


QGraphicsItem* CSingleLayerGroupComp::GetGraphicsItem()
{
	return m_graphicsItem;
}


QByteArray CSingleLayerGroupComp::GetGroupId() const
{
	if (m_groupIdAttrPtr.IsValid()){
		return *m_groupIdAttrPtr;
	}

	return QByteArray();
}


int CSingleLayerGroupComp::GetGroupHeight() const
{
	if (m_groupHeightAttrPtr.IsValid()){
		return *m_groupHeightAttrPtr;
	}

	return 150;
}


QString CSingleLayerGroupComp::GetGroupName() const
{
	return m_groupName;
}


void CSingleLayerGroupComp::SetGroupName(const QString& name)
{
	m_groupName = name;
	if (m_graphicsItem != nullptr){
		m_graphicsItem->SetGroupName(name);
	}
}


const IEventItemController::EventMap* CSingleLayerGroupComp::GetEvents() const
{
	return &m_events;
}


CEventItemBase* CSingleLayerGroupComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	if (m_eventItemFactoryCompPtr.IsValid() && m_graphicsItem){
		CEventItemBase* itemPtr = nullptr;

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
		origin = m_graphicsItem->mapFromScene(origin);
		origin.setY(m_graphicsItem->rect().height() / 2);

		itemPtr->setParentItem(m_graphicsItem);
		itemPtr->setPos(origin);
		itemPtr->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

		m_events.insert(messagePtr->GetInformationTimeStamp() ,itemPtr);

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
	if (m_graphicsItem != nullptr){
		m_graphicsItem->setVisible(isVisible);
	}
}


void CSingleLayerGroupComp::OnTimeAxisChanged()
{
	// TODO: Synchronize group with time axis
}


void CSingleLayerGroupComp::OnViewPortChanged()
{
	if (m_graphicsItem != nullptr){
		m_graphicsItem->OnViewPortChanged();
	}

	ArrangeEvents();
}


// private methods

double CSingleLayerGroupComp::GetCurrentScaleX() const
{
	if (m_scenePtr != nullptr){
		return m_scenePtr->views().first()->viewportTransform().m11();
	}

	return 0;
}


double CSingleLayerGroupComp::GetCurrentScaleY() const
{
	if (m_scenePtr != nullptr){
		return m_scenePtr->views().first()->viewportTransform().m22();
	}

	return 0;
}


void CSingleLayerGroupComp::ArrangeEvents()
{
	//if (m_events.isEmpty()){
	//	return;
	//}

	//double scaleX = GetCurrentScaleX();
	//double scaleY = GetCurrentScaleX();

	//if (!(scaleX && scaleY)){
	//	return;
	//}

	//double groupHeight = GetGroupHeight() * scaleY;

	//QDateTime beginTime = m_timeAxisPtr->GetVisibleBeginTime();
	//QDateTime endTime = m_timeAxisPtr->GetVisibleEndTime();

	//EventMap::iterator beginIt = m_events.lowerBound(beginTime);
	//EventMap::iterator endIt = m_events.upperBound(endTime);

	//QRectF itemRect = m_events.first()->boundingRect();

	//int rowCount = groupHeight / itemRect.height();
	////double 
	//
	//for (EventMap::iterator it = beginIt; it != endIt; it++){
	//	//qDebug() << it.key();
	//}
}


} // namespace imtloggui


