#include <imtloggui/CSingleLayerGroupComp.h>


// Qt includes
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


CSingleLayerGroupComp::CSingleLayerGroupComp()
	:m_scenePtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_graphicsItemPtr(nullptr),
	m_itemGroupPtr(nullptr)
{
	m_groupName = "Unnamed";
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
	if (m_graphicsItemPtr == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_graphicsItemPtr = new CEventGroupItem();

		m_graphicsItemPtr->SetGroupName(m_groupName);

		if (m_groupColorAttrPtr.IsValid()){
			m_graphicsItemPtr->SetBackgroundColor(QString(*m_groupColorAttrPtr));
		}

		if (m_groupHeightAttrPtr.IsValid()){
			m_graphicsItemPtr->setRect(QRectF(0, 0, 0, -*m_groupHeightAttrPtr));
		}

		m_scenePtr->addItem(m_graphicsItemPtr);


		m_itemGroupPtr = new CGraphicsItemGroup(m_graphicsItemPtr);
	}

	return false;
}


bool CSingleLayerGroupComp::DestroyGraphicsItem()
{
	if (m_graphicsItemPtr != nullptr && m_scenePtr != nullptr){
		ClearEvents();

		m_scenePtr->removeItem(m_graphicsItemPtr);
		delete m_graphicsItemPtr;
		m_graphicsItemPtr = nullptr;

		return true;
	}

	return false;
}


QGraphicsItem* CSingleLayerGroupComp::GetGraphicsItem()
{
	return m_graphicsItemPtr;
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
	if (m_graphicsItemPtr != nullptr){
		m_graphicsItemPtr->SetGroupName(name);
	}
}


QVector<int> CSingleLayerGroupComp::GetSupportedMessageIds() const
{
	QVector<int> ids;

	if (m_eventItemFactoryCompPtr.IsValid()){
		ids.append(m_eventItemFactoryCompPtr->GetSupportedMessageIds());
	}

	return ids;
}


const IEventItemController::EventMap* CSingleLayerGroupComp::GetEvents() const
{
	return &m_events;
}


int CSingleLayerGroupComp::GetEventCount(istd::IInformationProvider::InformationCategory category)
{
	return m_eventCount[category];
}


CEventItemBase* CSingleLayerGroupComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	if (m_eventItemFactoryCompPtr.IsValid() && m_graphicsItemPtr){
		CEventItemBase* itemPtr = m_eventItemFactoryCompPtr->CreateInstance(messagePtr);

		if (itemPtr == nullptr){
			return nullptr;
		}

		QPointF origin(m_timeAxisPtr->GetScenePositionFromTime(messagePtr->GetInformationTimeStamp()), 0);
		origin = m_itemGroupPtr->mapFromScene(origin);
		origin.setY(m_graphicsItemPtr->rect().height() / 2);

		itemPtr->setParentItem(m_itemGroupPtr);
		itemPtr->setPos(origin);
		itemPtr->setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIsSelectable);

		m_events.insert(messagePtr->GetInformationTimeStamp() ,itemPtr);
		m_eventCount[messagePtr->GetInformationCategory()]++;

		ArrangeEvents();

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
	if (m_graphicsItemPtr != nullptr){
		m_graphicsItemPtr->setVisible(isVisible);
	}
}


void CSingleLayerGroupComp::OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	double width = (m_timeAxisPtr->GetEndTime().toMSecsSinceEpoch() - m_timeAxisPtr->GetBeginTime().toMSecsSinceEpoch()) / 1000.;
	if (m_graphicsItemPtr != nullptr){
		m_graphicsItemPtr->setRect(QRectF(0, 0, width, -*m_groupHeightAttrPtr));
	}

	if (oldTime.isValid()){
		double shift = (oldTime.toMSecsSinceEpoch() - newTime.toMSecsSinceEpoch()) / 1000.;
		m_itemGroupPtr->setPos(m_itemGroupPtr->pos().x() + shift, 0);
	}
}


void CSingleLayerGroupComp::OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	double width = (m_timeAxisPtr->GetEndTime().toMSecsSinceEpoch() - m_timeAxisPtr->GetBeginTime().toMSecsSinceEpoch()) / 1000.;
	if (m_graphicsItemPtr != nullptr){
		m_graphicsItemPtr->setRect(QRectF(0, 0, width, -*m_groupHeightAttrPtr));
	}
}


void CSingleLayerGroupComp::OnViewPortChanged()
{
	if (m_graphicsItemPtr != nullptr){
		m_graphicsItemPtr->OnViewPortChanged();
	}

	ArrangeEvents();
}


// reimplemented (icomp::CComponentBase)

void CSingleLayerGroupComp::OnComponentCreated()
{
	if (m_groupNameAttrPtr.IsValid()){
		SetGroupName(*m_groupNameAttrPtr);
	}
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
	double scaleX = GetCurrentScaleX();
	double scaleY = GetCurrentScaleY();

	if (!(scaleX && scaleY)){
		return;
	}

	double groupHeight = GetGroupHeight() * scaleY; // in pixels
	
	QDateTime beginTime = m_timeAxisPtr->GetVisibleBeginTime();
	QDateTime endTime = m_timeAxisPtr->GetVisibleEndTime();

	EventMap::iterator beginIt = m_events.lowerBound(beginTime);
	EventMap::iterator endIt = m_events.upperBound(endTime);

	QList<CEventItemBase*> arrangedItems;
	double arrangedHeight = 0;

	for (EventMap::iterator it = beginIt; it != endIt; it++){
		CEventItemBase* currentPtr = it.value();
		QRectF currentRect = currentPtr->boundingRect();
		QPointF currentPos = currentPtr->pos();

		// Current stairway is empty
		if (arrangedItems.isEmpty()){
			arrangedItems.append(currentPtr);
			currentPos.ry() = -GetGroupHeight() / 2;
			currentPtr->setPos(currentPos);
			arrangedHeight = currentRect.height();
			continue;
		}

		CEventItemBase* lastPtr = arrangedItems.last();

		// Distances in pixels
		double posDist = (currentPos.x() - lastPtr->pos().x()) * scaleX;
		double widthHalfSum = (currentRect.width() + lastPtr->boundingRect().width()) / 2;

		// Ñurrent and previous items don't intersect along the x axis. Begin new stairway
		if (posDist > widthHalfSum){
			currentPos.ry() = (-groupHeight / 2) / scaleY;
			currentPtr->setPos(currentPos);
			arrangedHeight = currentRect.height();
			arrangedItems.clear();
			arrangedItems.append(currentPtr);
			continue;
		}

		// Current stairway can grow up
		if (arrangedHeight + (currentRect.height() + 3 * (*m_verticalSpaceingAttrPtr)) < groupHeight){
			// Shift up stairway
			for (CEventItemBase* itemPtr : arrangedItems){
				QPointF pos = itemPtr->pos();
				pos.ry() -= ((currentRect.height() + *m_verticalSpaceingAttrPtr) / 2) / scaleY;
				itemPtr->setPos(pos);
			}

			currentPos.ry() = lastPtr->pos().y() + (lastPtr->boundingRect().height() / 2 + *m_verticalSpaceingAttrPtr + currentRect.height() / 2) / scaleY;
			currentPtr->setPos(currentPos);
			arrangedHeight += *m_verticalSpaceingAttrPtr + currentRect.height();
			arrangedItems.append(currentPtr);
			continue;
		}

		// Current stairway can't grow up. Begin new stairway
		currentPos.ry() = -GetGroupHeight() / 2;
		currentPtr->setPos(currentPos);
		arrangedHeight = currentRect.height();
		arrangedItems.clear();
		arrangedItems.append(currentPtr);
	}
}


} // namespace imtloggui


