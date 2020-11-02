#include <imtloggui/CGraphicsControllerComp.h>


// Qt includes
#include <QtGui/QPen>


namespace imtloggui
{


// public methods

CGraphicsControllerComp::CGraphicsControllerComp()
{
	m_viewPropertyObserver.SetParent(this);
	m_timeRangeObserver.SetParent(this);
}

// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGraphicsControllerComp::GetGraphicsItems() const
{
	GraphicsItemList retVal;

	if (m_groupProviderCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids groupIds = m_groupProviderCompPtr->GetElementIds();
		QList<int> allGroupsMessageIds;
		for (int i = 0; i < groupIds.count(); i++){
			ILayerProvider* layerProvider = m_groupProviderCompPtr->GetLayerProvider(groupIds[i]);
			imtbase::ICollectionInfo::Ids layerIds = layerProvider->GetElementIds();
			IRepresentationFactoryProvider* representationProvider = layerProvider->GetRepresentationFactoryProvider(layerIds[0]);
			imtbase::ICollectionInfo::Ids factoryIds = representationProvider->GetElementIds();
			IRepresentationFactory* representation = representationProvider->GetRepresentationFactory(factoryIds[0]);

			QDateTime begin = m_timeAxis.GetVisibleBeginTime();
			QDateTime end = m_timeAxis.GetVisibleEndTime();

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

				IGraphicsItemProvider::GraphicsItemList items = m_representationViewFactoryCompPtr->CreateGraphicItems(representationObject.GetPtr());

				for (int j = 0; j < items.count(); j++){
					items[j]->setPos(items[j]->pos().x(), -150 - i*300);
				}

				retVal.append(items);
			}
		}
	}

	return retVal;


}


// reimplemented (icomp::CComponentBase)

void CGraphicsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeAxis.setPos(0, 0);
	m_timeAxis.SetColor(Qt::green);
	m_timeAxis.setRect(0, 0, 100, 40);
	m_timeAxis.setZValue(101);
	m_staticItemsProvider.AddItem(&m_timeAxis);

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
			m_staticItemsProvider.AddItem(rectPtr);

			CEventGroupLabelItem* labelPtr = new CEventGroupLabelItem();
			labelPtr->SetBackgroundColor(color);
			labelPtr->SetGroupName(name);
			labelPtr->SetHeight(300);
			sceneHeight += 300;
			labelPtr->setPos(0, -150 - i * 300);
			m_staticItemsProvider.AddItem(labelPtr);

			GroupItem groupItem;
			groupItem.backgroundPtr = rectPtr;
			groupItem.labelPtr = labelPtr;
			m_groupItemList.append(groupItem);
		}
	}

	if (m_viewPropertyModelCompPtr.IsValid()){
		m_viewPropertyModelCompPtr->AttachObserver(&m_viewPropertyObserver);
	}

	if (m_viewPropertyManagerCompPtr.IsValid()){
		m_viewPropertyManagerCompPtr->SetSceneRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewPropertyManagerCompPtr->SetViewRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewPropertyManagerCompPtr->SetMargins(QMargins(70, 0, 70, 40));
		m_viewPropertyManagerCompPtr->SetScaleRangeX(istd::CRange(0, 1000));
		m_viewPropertyManagerCompPtr->SetScaleRangeY(istd::CRange(0, 1000));
	}

	if (m_timeRangeModelCompPtr.IsValid()){
		m_timeRangeModelCompPtr->AttachObserver(&m_timeRangeObserver);
	}
}


// private methods

void CGraphicsControllerComp::OnViewPropertyUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	QRectF viewRect = propertyPtr->GetViewRect();
	m_timeAxis.setPos(0, viewRect.bottom() - m_timeAxis.rect().height() / propertyPtr->GetScaleY());

	for (int i = 0; i < m_groupItemList.count(); i++){
		QRectF rect = m_groupItemList[i].backgroundPtr->rect();
		rect.setWidth(viewRect.width());
		m_groupItemList[i].backgroundPtr->setRect(rect);
		m_groupItemList[i].backgroundPtr->setPos(viewRect.x(), m_groupItemList[i].backgroundPtr->pos().y());
		m_groupItemList[i].labelPtr->setPos(viewRect.x(), m_groupItemList[i].labelPtr->pos().y());
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

void CGraphicsControllerComp::TimeRangeObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();

	if (timeRange.GetBeginTime().isValid()){
		m_parent->m_timeAxis.EnsureTimeRange(timeRange.GetBeginTime());
		m_parent->m_timeAxis.EnsureTimeRange(timeRange.GetEndTime());

		if (m_parent->m_viewPropertyProviderCompPtr.IsValid()){
			QRectF rect = m_parent->m_viewPropertyProviderCompPtr->GetSceneRect();
			rect.setWidth(m_parent->m_timeAxis.rect().width());
			m_parent->m_viewPropertyManagerCompPtr->SetSceneRect(rect);
		}
	}
}


// public methods of the embedded class StaticItemsProvider

bool CGraphicsControllerComp::StaticItemsProvider::AddItem(QGraphicsItem* itemPtr)
{
	if (!m_items.contains(itemPtr)){
		istd::CChangeNotifier notifier(this);

		m_items.append(itemPtr);

		return true;
	}

	return false;
}


bool CGraphicsControllerComp::StaticItemsProvider::RemoveItem(QGraphicsItem* itemPtr)
{
	if (m_items.contains(itemPtr)){
		istd::CChangeNotifier notifier(this);

		m_items.removeOne(itemPtr);

		return true;
	}

	return false;
}


// protected methods of the embedded class StaticItemsProvider

// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGraphicsControllerComp::StaticItemsProvider::GetGraphicsItems() const
{
	return m_items;
}


} // namespace imtloggui


