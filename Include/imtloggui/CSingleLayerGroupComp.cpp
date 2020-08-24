#include <imtloggui/CSingleLayerGroupComp.h>


// Qt includes
#include <QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

// ImtCore includes
#include <imtloggui/CClusterItem.h>


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


IEventItem* CSingleLayerGroupComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	if (m_eventItemFactoryCompPtr.IsValid() && m_graphicsItemPtr){
		IEventItem* eventItemPtr = m_eventItemFactoryCompPtr->CreateInstance(messagePtr);
		if (eventItemPtr == nullptr){
			return nullptr;
		}

		QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(eventItemPtr);
		Q_ASSERT(graphicsItemPtr != nullptr);

		QPointF origin(m_timeAxisPtr->GetScenePositionFromTime(messagePtr->GetInformationTimeStamp()), 0);
		origin = m_itemGroupPtr->mapFromScene(origin);
		origin.setY(m_graphicsItemPtr->rect().height() / 2);

		graphicsItemPtr->setParentItem(m_itemGroupPtr);
		graphicsItemPtr->setPos(origin);
		graphicsItemPtr->setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIsSelectable);
		graphicsItemPtr->setVisible(false);

		m_events.insert(messagePtr->GetInformationTimeStamp(), eventItemPtr);
		m_eventCount[messagePtr->GetInformationCategory()]++;

		CreateClusters();
		//ArrangeEvents();

		return eventItemPtr;
	}

	return nullptr;
}


void CSingleLayerGroupComp::ClearEvents()
{
	for (IItemBase* eventItemPtr : m_events){
		QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(eventItemPtr);
		Q_ASSERT(graphicsItemPtr != nullptr);

		m_scenePtr->removeItem(graphicsItemPtr);
		delete graphicsItemPtr;
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

	CreateClusters();
	//ArrangeEvents();
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

	QList<IItemBase*> arrangedItems;
	double arrangedHeight = 0;

	for (EventMap::iterator it = beginIt; it != endIt; it++){
		QGraphicsItem* currentPtr = dynamic_cast<QGraphicsItem*>(it.value());
		Q_ASSERT(currentPtr != nullptr);

		QRectF currentRect = currentPtr->boundingRect();
		QPointF currentPos = currentPtr->pos();

		// Current stairway is empty
		if (arrangedItems.isEmpty()){
			arrangedItems.append(it.value());
			currentPos.ry() = -GetGroupHeight() / 2;
			currentPtr->setPos(currentPos);
			arrangedHeight = currentRect.height();
			continue;
		}

		QGraphicsItem* lastPtr = dynamic_cast<QGraphicsItem*>(arrangedItems.last());
		Q_ASSERT(lastPtr != nullptr);

		// Distances in pixels
		double posDist = (currentPos.x() - lastPtr->pos().x()) * scaleX;
		double widthHalfSum = (currentRect.width() + lastPtr->boundingRect().width()) / 2;

		// Ñurrent and previous items don't intersect along the x axis. Begin new stairway
		if (posDist > widthHalfSum){
			currentPos.ry() = (-groupHeight / 2) / scaleY;
			currentPtr->setPos(currentPos);
			arrangedHeight = currentRect.height();
			arrangedItems.clear();
			arrangedItems.append(it.value());
			continue;
		}

		// Current stairway can grow up
		if (arrangedHeight + (currentRect.height() + 3 * (*m_verticalSpaceingAttrPtr)) < groupHeight){
			// Shift up stairway
			for (QList<IItemBase*>::iterator arrangedIt = arrangedItems.begin(); arrangedIt != arrangedItems.end(); arrangedIt++){
				QGraphicsItem* arrangedPtr = dynamic_cast<QGraphicsItem*>(*arrangedIt);
				Q_ASSERT(arrangedPtr != nullptr);

				QPointF pos = arrangedPtr->pos();
				pos.ry() -= ((currentRect.height() + *m_verticalSpaceingAttrPtr) / 2) / scaleY;
				arrangedPtr->setPos(pos);
			}

			currentPos.ry() = lastPtr->pos().y() + (lastPtr->boundingRect().height() / 2 + *m_verticalSpaceingAttrPtr + currentRect.height() / 2) / scaleY;
			currentPtr->setPos(currentPos);
			arrangedHeight += *m_verticalSpaceingAttrPtr + currentRect.height();
			arrangedItems.append(it.value());
			continue;
		}

		// Current stairway can't grow up. Begin new stairway
		currentPos.ry() = -GetGroupHeight() / 2;
		currentPtr->setPos(currentPos);
		arrangedHeight = currentRect.height();
		arrangedItems.clear();
		arrangedItems.append(it.value());
	}
}


void CSingleLayerGroupComp::CreateClusters()
{
	QTransform transform  = m_scenePtr->views()[0]->transform();

	QDateTime beginView = m_timeAxisPtr->GetVisibleBeginTime();
	QDateTime endView = m_timeAxisPtr->GetVisibleEndTime();
	qint64 msecs = endView.toMSecsSinceEpoch() - beginView.toMSecsSinceEpoch();
	QDateTime begin = beginView.addMSecs(-msecs);
	QDateTime end = endView.addMSecs(msecs);


	EventMap::iterator it;
	EventMap::const_iterator itEnd;
	QList<IItemBase*>::iterator itVisible;
	QList<IItemBase*>::const_iterator itVisibleEnd;


	//CClusterItem* cp = new CClusterItem;
	//cp->setPos(0, 0);
	//cp->setParentItem(m_graphicsItemPtr);

	//if (m_lastTransform.m11() != transform.m11()){
		for (IItemBase* clusterPtr : m_visibleClusters){
			QGraphicsItem* giClusterPtr = dynamic_cast<QGraphicsItem*>(clusterPtr);

			for(QGraphicsItem* giItemPtr : giClusterPtr->childItems()){
				IEventItem* itemPtr = dynamic_cast<IEventItem*>(giItemPtr);
				m_events.insert(itemPtr->GetInformationProvider()->GetInformationTimeStamp(), itemPtr);
			}

			dynamic_cast<IClusterItem*>(clusterPtr)->DetachAll();

			delete giClusterPtr;
		}

		for (IItemBase* itemPtr : m_visibleItems){
			QGraphicsItem* graphicsItemPtr = dynamic_cast<QGraphicsItem*>(itemPtr);
			graphicsItemPtr->setVisible(false);
		}

		m_visibleClusters.clear();
		m_visibleItems.clear();

	//}
	//else {
	//	itVisible = m_visibleClusters.begin();
	//	while(itVisible != m_visibleClusters.end()){
	//		if (itVisible.key() < begin || itVisible.key() > end){
	//			QGraphicsItem* giClusterPtr = dynamic_cast<QGraphicsItem*>(itVisible.value());
	//			m_scenePtr->removeItem(giClusterPtr);
	//			delete giClusterPtr;

	//			itVisible = m_visibleClusters.erase(itVisible);
	//			continue;
	//		}

	//		itVisible++;
	//	}

	//	itVisible = m_visibleItems.begin();
	//	while (itVisible != m_visibleItems.end()){
	//		if (itVisible.key() < begin || itVisible.key() > end){
	//			QGraphicsItem* giClusterPtr = dynamic_cast<QGraphicsItem*>(itVisible.value());
	//			giClusterPtr->setVisible(false);

	//			itVisible = m_visibleItems.erase(itVisible);
	//			continue;
	//		}

	//		itVisible++;
	//	}
	//}

	it = m_events.lowerBound(begin);
	itEnd = m_events.upperBound(end);

	double scale = transform.m11();

	while (it != itEnd){
		bool processNextIt = false;

		itVisible = m_visibleClusters.begin();
		while (itVisible != m_visibleClusters.end()){
			if (it.value()->CollidesWithItem(*itVisible, scale)){
				CClusterItem* clusterPtr = dynamic_cast<CClusterItem*>(*itVisible);
				clusterPtr->Attach(dynamic_cast<IEventItem*>(it.value()));

				it = m_events.erase(it);

				processNextIt = true;				
				break;
			}

			itVisible++;
		}

		if (processNextIt){
			continue;
		}

		itVisible = m_visibleItems.begin();
		while (itVisible != m_visibleItems.end()){
			if (it.value()->CollidesWithItem(*itVisible, scale)){
				CClusterItem* clusterPtr = new CClusterItem();
				QGraphicsItem* giVisiblePtr = dynamic_cast<QGraphicsItem*>(*itVisible);
				m_visibleClusters.append(clusterPtr);

				clusterPtr->SetParams(QSize(100, 100));
				clusterPtr->setParentItem(m_itemGroupPtr);
				clusterPtr->setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIsSelectable);

				double giX = giVisiblePtr->pos().x();
				double giX1 = dynamic_cast<QGraphicsItem*>(it.value())->pos().x();
				double giW = giVisiblePtr->boundingRect().width();
 				clusterPtr->setPos(
					giX -
					(giW / 2) / scale +
					(100 / 2) / scale,
					giVisiblePtr->pos().y());

				qDebug() << clusterPtr->pos();

				clusterPtr->Attach(dynamic_cast<IEventItem*>(it.value()));
				clusterPtr->Attach(dynamic_cast<IEventItem*>(*itVisible));

				m_visibleItems.erase(itVisible);
				m_events.remove(dynamic_cast<IEventItem*>(*itVisible)->GetInformationProvider()->GetInformationTimeStamp(), *itVisible);
				it = m_events.erase(it);

				processNextIt = true;
				break;
			}

			itVisible++;
		}

		if (processNextIt){
			continue;
		}

		dynamic_cast<QGraphicsItem*>(it.value())->setVisible(true);
		m_visibleItems.append(it.value());
		
		it++;
	}

	m_lastTransform = m_scenePtr->views()[0]->transform();
}


} // namespace imtloggui


