#include <imtloggui/CGraphicsControllerComp.h>


// Qt includes
#include <QtGui/QPen>


namespace imtloggui
{


// public methods

CGraphicsControllerComp::CGraphicsControllerComp()
	:m_rootCommands("", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_moveToFirstCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToPreviousCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToNextCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToLastCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
	m_viewPropertyObserver.SetParent(this);
	m_timeRangeObserver.SetParent(this);

	m_moveToFirstCommand.SetVisuals(tr("First event"), tr("First"), tr("Move to first event"), QIcon(":/Icons/MoveFirst"));
	m_moveToPreviousCommand.SetVisuals(tr("Previous event"), tr("Previous"), tr("Move to previous event"), QIcon(":/Icons/Left"));
	m_moveToNextCommand.SetVisuals(tr("Next event"), tr("Next"), tr("Move to next event"), QIcon(":/Icons/Right"));
	m_moveToLastCommand.SetVisuals(tr("Last event"), tr("Last"), tr("Move to last event"), QIcon(":/Icons/MoveLast"));

	m_commands.SetParent(this);
	m_rootCommands.InsertChild(&m_moveToFirstCommand);
	m_rootCommands.InsertChild(&m_moveToPreviousCommand);
	m_rootCommands.InsertChild(&m_moveToNextCommand);
	m_rootCommands.InsertChild(&m_moveToLastCommand);

	connect(&m_moveToFirstCommand, &QAction::triggered, this, &CGraphicsControllerComp::OnMoveToFirstCommand);
	connect(&m_moveToPreviousCommand, &QAction::triggered, this, &CGraphicsControllerComp::OnMoveToPreviousCommand);
	connect(&m_moveToNextCommand, &QAction::triggered, this, &CGraphicsControllerComp::OnMoveToNextCommand);
	connect(&m_moveToLastCommand, &QAction::triggered, this, &CGraphicsControllerComp::OnMoveToLastCommand);
}


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGraphicsControllerComp::GetAddedItems() const
{
	GraphicsItemList items = m_addedItems;
	m_addedItems.clear();
	return items;
}


IGraphicsItemProvider::GraphicsItemList CGraphicsControllerComp::GetRemovedItems() const
{
	GraphicsItemList items = m_removedItems;
	m_removedItems.clear();
	return items;
}


// reimplemented (ibase::TLocalizableWrap)

void CGraphicsControllerComp::OnLanguageChanged()
{
	m_moveToFirstCommand.SetVisuals(tr("First event"), tr("First"), tr("Move to first event"), QIcon(":/Icons/MoveFirst"));
	m_moveToPreviousCommand.SetVisuals(tr("Previous event"), tr("Previous"), tr("Move to previous event"), QIcon(":/Icons/Left"));
	m_moveToNextCommand.SetVisuals(tr("Next event"), tr("Next"), tr("Move to next event"), QIcon(":/Icons/Right"));
	m_moveToLastCommand.SetVisuals(tr("Last event"), tr("Last"), tr("Move to last event"), QIcon(":/Icons/MoveLast"));
}


// reimplemented (icomp::CComponentBase)

void CGraphicsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	istd::CChangeNotifier notifier(this);

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->setPos(0, 0);
	m_timeAxisPtr->SetColor(Qt::green);
	m_timeAxisPtr->setRect(0, 0, 100, 40);
	m_timeAxisPtr->setZValue(10);
	//m_timeAxisPtr->EnsureTimeRange(QDateTime::currentDateTime());
	m_timeAxisSPtr.SetPtr(m_timeAxisPtr);
	m_addedItems.append(m_timeAxisSPtr);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CGraphicsControllerComp::OnAxisBeginTimeChanged);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisEndTimeChanged, this, &CGraphicsControllerComp::OnAxisEndTimeChanged);

	int sceneHeight = 0;

	if (m_groupProviderCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids ids = m_groupProviderCompPtr->GetElementIds();

		for (int i = 0; i < ids.count(); i++){
			QString name = m_groupProviderCompPtr->GetElementInfo(ids[i], imtbase::ICollectionInfo::EIT_NAME).toString();
			QColor color(m_groupProviderCompPtr->GetElementInfo(ids[i], IGroupProvider::EIT_COLOR).toString());

			QGraphicsRectItem* rectPtr = new QGraphicsRectItem();
			rectPtr->setRect(0, 0, 100, 300);
			rectPtr->setBrush(QBrush(color));
			rectPtr->setPen(QPen(Qt::transparent));
			rectPtr->setPos(0, -300 - i * 300);
			rectPtr->setZValue(0);

			CEventGroupLabelItem* labelPtr = new CEventGroupLabelItem();
			labelPtr->SetBackgroundColor(color);
			labelPtr->SetGroupName(name);
			labelPtr->SetHeight(300);
			sceneHeight += 300;
			labelPtr->setPos(0, -150 - i * 300);
			labelPtr->setZValue(9);

			GroupItem groupItem;
			groupItem.backgroundPtr = rectPtr;
			groupItem.labelPtr = labelPtr;
			groupItem.sptrs.append(GraphicsItem(rectPtr));
			groupItem.sptrs.append(GraphicsItem(labelPtr));
			m_groupItemList.append(groupItem);

			m_addedItems.append(groupItem.sptrs);
		}
	}

	if (m_viewPropertyModelCompPtr.IsValid()){
		m_viewPropertyModelCompPtr->AttachObserver(&m_viewPropertyObserver);
	}

	if (m_viewPropertyManagerCompPtr.IsValid()){
		m_viewPropertyManagerCompPtr->SetSceneRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewPropertyManagerCompPtr->SetViewRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewPropertyManagerCompPtr->SetMargins(QMargins(70, 0, 200, 40));
		m_viewPropertyManagerCompPtr->SetScaleRangeX(istd::CRange(0, 1000));
		m_viewPropertyManagerCompPtr->SetScaleRangeY(istd::CRange(0, 1000));
	}

	if (m_timeRangeModelCompPtr.IsValid()){
		m_timeRangeModelCompPtr->AttachObserver(&m_timeRangeObserver);
	}
}


// private slots

void CGraphicsControllerComp::OnAxisBeginTimeChanged(const QDateTime& /*oldTime*/, const QDateTime& /*newTime*/)
{

}


void CGraphicsControllerComp::OnAxisEndTimeChanged(const QDateTime& /*oldTime*/, const QDateTime& /*newTime*/)
{

}


void CGraphicsControllerComp::OnMoveToFirstCommand()
{
	//if (!m_eventMap.isEmpty()){
	//	m_navigationIterator = m_eventMap.cbegin();
	//	m_isNavigationIteratorValid = true;

	//	QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
	//	if (graphicsItem != nullptr){
	//		MoveToTime(m_navigationIterator.key());
	//		m_scene.clearSelection();
	//		graphicsItem->setSelected(true);
	//	}

	//	UpdateCommands();
	//}
}


void CGraphicsControllerComp::OnMoveToPreviousCommand()
{
	//if (!m_eventMap.isEmpty()){
	//	if (m_isNavigationIteratorValid){
	//		if (m_navigationIterator == m_eventMap.cbegin()){
	//			return;
	//		}

	//		m_navigationIterator--;
	//	}
	//	else{
	//		QDateTime time = m_timeAxisPtr.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
	//		m_navigationIterator = --m_eventMap.lowerBound(time);
	//		m_isNavigationIteratorValid = true;
	//	}

	//	QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
	//	if (graphicsItem != nullptr){
	//		MoveToTime(m_navigationIterator.key());
	//		m_scene.clearSelection();
	//		graphicsItem->setSelected(true);
	//	}

	//	UpdateCommands();
	//}
}


void CGraphicsControllerComp::OnMoveToNextCommand()
{
	//if (!m_eventMap.isEmpty()){
	//	if (m_isNavigationIteratorValid){
	//		if (m_navigationIterator == --m_eventMap.cend()){
	//			return;
	//		}

	//		m_navigationIterator++;
	//	}
	//	else{
	//		QDateTime time = m_timeAxisPtr.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
	//		m_navigationIterator = m_eventMap.lowerBound(time);
	//		m_isNavigationIteratorValid = true;
	//	}

	//	QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
	//	if (graphicsItem != nullptr){
	//		MoveToTime(m_navigationIterator.key());
	//		m_scene.clearSelection();
	//		graphicsItem->setSelected(true);
	//	}

	//	UpdateCommands();
	//}
}


void CGraphicsControllerComp::OnMoveToLastCommand()
{
	//if (!m_eventMap.isEmpty()){
	//	m_navigationIterator = --m_eventMap.cend();
	//	m_isNavigationIteratorValid = true;

	//	QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
	//	if (graphicsItem != nullptr){
	//		MoveToTime(m_navigationIterator.key());
	//		m_scene.clearSelection();
	//		graphicsItem->setSelected(true);
	//	}

	//	UpdateCommands();
	//}
}


// private methods

void CGraphicsControllerComp::OnViewPropertyUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QRectF viewRect = propertyPtr->GetViewRect();
	m_timeAxisPtr->setPos(0, viewRect.bottom() - m_timeAxisPtr->rect().height() / propertyPtr->GetScaleY());

	for (int i = 0; i < m_groupItemList.count(); i++){
		QRectF rect = m_groupItemList[i].backgroundPtr->rect();
		rect.setWidth(viewRect.width());
		m_groupItemList[i].backgroundPtr->setRect(rect);
		m_groupItemList[i].backgroundPtr->setPos(viewRect.x(), m_groupItemList[i].backgroundPtr->pos().y());
		m_groupItemList[i].labelPtr->setPos(viewRect.x(), m_groupItemList[i].labelPtr->pos().y());
	}

	qint64 span = m_timeAxisPtr->GetVisibleBeginTime().msecsTo(m_timeAxisPtr->GetVisibleEndTime());

	if (m_groupProviderCompPtr.IsValid() && span > 0){
		imtbase::ICollectionInfo::Ids groupIds = m_groupProviderCompPtr->GetElementIds();
		QList<int> allGroupsMessageIds;

		GraphicsItemList items;

		for (int i = 0; i < groupIds.count(); i++){
			ILayerProvider* layerProvider = m_groupProviderCompPtr->GetLayerProvider(groupIds[i]);
			imtbase::ICollectionInfo::Ids layerIds = layerProvider->GetElementIds();
			QByteArray id = layerProvider->GetIdForTimeSpan(span);
			IRepresentationFactoryProvider* representationProvider = layerProvider->GetRepresentationFactoryProvider(id);
			imtbase::ICollectionInfo::Ids factoryIds = representationProvider->GetElementIds();
			IRepresentationFactory* representation = representationProvider->GetRepresentationFactory(factoryIds[0]);

			QDateTime begin = m_timeAxisPtr->GetVisibleBeginTime();
			QDateTime end = m_timeAxisPtr->GetVisibleEndTime();

			if (i != groupIds.count() - 1){
				allGroupsMessageIds.append(layerProvider->GetMessageIdList());
			}

			if (m_representationViewFactoryCompPtr.IsValid()){
				istd::TSmartPtr<istd::IChangeable> representationObject;

				if (i == groupIds.count() - 1){
					representationObject = representation->CreateRepresentationObject(
								imtlog::CTimeRange(begin, end),
								allGroupsMessageIds,
								imtlog::IEventMessageIdFilter::M_EXCEPT);
				}
				else{
					representationObject = representation->CreateRepresentationObject(
								imtlog::CTimeRange(begin, end),
								layerProvider->GetMessageIdList(),
								imtlog::IEventMessageIdFilter::M_ACCEPT);
				}

			
				GraphicsItemList groupItems;
				if (representationObject.IsValid()){
					groupItems += m_representationViewFactoryCompPtr->CreateGraphicItems(representationObject, groupIds[i]);
				}

				for (int j = 0; j < groupItems.count(); j++){
					if (!m_items.contains(groupItems[j])){
						groupItems[j]->setPos(groupItems[j]->pos().x(), -150 - i*300);
						groupItems[j]->setZValue(1);
					}
				}

				items += groupItems;
			}
		}

		GraphicsItemList::iterator it = m_items.begin();
		while(it != m_items.end()){
			if (!items.contains(*it)){
				m_removedItems.append(*it);
				it = m_items.erase(it);
				continue;
			}

			it++;
		}

		for (int i = 0; i < items.count(); i++){
			if (!m_items.contains(items[i])){
				m_addedItems.append(items[i]);
				m_items.append(items[i]);
			}
		}

		if (m_addedItems.count() != 0 || m_removedItems.count() != 0){
			istd::CChangeNotifier notifier(this);
		}
	}
}


// public methods of the embedded class ViewPropertyObserver

CGraphicsControllerComp::ViewPropertyObserver::ViewPropertyObserver()
	:m_parent(nullptr)
{
}

void CGraphicsControllerComp::ViewPropertyObserver::SetParent(CGraphicsControllerComp* parent)
{
	m_parent = parent;
}


// protected methods of the embedded class ViewPropertyObserver

// reimplemented (imod::CSingleModelObserverBase)

void CGraphicsControllerComp::ViewPropertyObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent->OnViewPropertyUpdate(GetObservedObject(), changeSet);
}


// public methods of the embedded class TimeRangeObserver

CGraphicsControllerComp::TimeRangeObserver::TimeRangeObserver()
	:m_parent(nullptr)
{
}

void CGraphicsControllerComp::TimeRangeObserver::SetParent(CGraphicsControllerComp* parent)
{
	m_parent = parent;
}


// protected methods of the embedded class ViewPropertyObserver

// reimplemented (imod::CSingleModelObserverBase)

void CGraphicsControllerComp::TimeRangeObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();

	if (timeRange.GetBeginTime().isValid()){
		m_parent->m_timeAxisPtr->EnsureTimeRange(timeRange.GetBeginTime());
		m_parent->m_timeAxisPtr->EnsureTimeRange(timeRange.GetEndTime());

		if (m_parent->m_viewPropertyProviderCompPtr.IsValid()){
			QRectF rect = m_parent->m_viewPropertyProviderCompPtr->GetSceneRect();
			rect.setWidth(m_parent->m_timeAxisPtr->rect().width());
			m_parent->m_viewPropertyManagerCompPtr->SetSceneRect(rect);
		}
	}
}


// public methods of the embedded class Commands

CGraphicsControllerComp::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CGraphicsControllerComp::Commands::SetParent(CGraphicsControllerComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CGraphicsControllerComp::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	return &m_parentPtr->m_rootCommands;
}


} // namespace imtloggui


