#include <imtloggui/CGraphicsControllerComp.h>


// Qt includes
#include <QtGui/QPen>

// ImtCore includes
#include <imtloggui/IGroupVisualSettings.h>
#include <imtloggui/IScenographer.h>


namespace imtloggui
{


// public methods

CGraphicsControllerComp::CGraphicsControllerComp()
	:m_groupDispatcher(this)
{
	m_viewportObserver.SetParent(this);
	m_timeRangeObserver.SetParent(this);
}


// reimplemented (imtloggui::IEventNavigationController)

void CGraphicsControllerComp::JumpToFirstEvent() const
{
}


void CGraphicsControllerComp::JumpToPreviousEvent() const
{
}


void CGraphicsControllerComp::JumpToNextEvent() const
{
}


void CGraphicsControllerComp::JumpToLastEvent() const
{
}


// reimplemented (imtloggui::IGraphicsController)

void CGraphicsControllerComp::InitScene()
{
	if (!m_graphicsSceneProviderCompPtr.IsValid()){
		return;
	}

	QGraphicsScene* scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
	if (scenePtr == nullptr){
		return;
	}

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->setPos(0, 0);
	m_timeAxisPtr->SetColor(Qt::green);
	m_timeAxisPtr->setRect(0, 0, 100, 40);
	m_timeAxisPtr->setZValue(10);
	scenePtr->addItem(m_timeAxisPtr);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CGraphicsControllerComp::OnAxisBeginTimeChanged);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisEndTimeChanged, this, &CGraphicsControllerComp::OnAxisEndTimeChanged);

	int sceneHeight = 0;

	if (m_groupViewProviderCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids ids = m_groupViewProviderCompPtr->GetElementIds();

		for (int i = 0; i < ids.count(); i++){
			QString name = m_groupViewProviderCompPtr->GetElementInfo(ids[i], imtbase::ICollectionInfo::EIT_NAME).toString();
			//QColor color(Qt::GlobalColor(Qt::magenta + i));
			QColor color(Qt::gray);
			int height = 300;
			if (m_groupViewVisualSettingsCompPtr.IsValid()){
				const imtloggui::IGroupVisualSettings* groupSettingsPtr = dynamic_cast<const imtloggui::IGroupVisualSettings*>(m_groupViewVisualSettingsCompPtr->GetObjectPtr(ids[i]));
				if (groupSettingsPtr != nullptr){
					color = groupSettingsPtr->GetBackgroundColor();
					height = groupSettingsPtr->GetHeight();
				}
			}

			QGraphicsRectItem* rectPtr = new QGraphicsRectItem();
			rectPtr->setRect(0, 0, 100, height);
			rectPtr->setBrush(QBrush(color));
			rectPtr->setPen(QPen(Qt::transparent));
			rectPtr->setPos(0, -height - i * height);
			rectPtr->setZValue(0);

			CEventGroupLabelItem* labelPtr = new CEventGroupLabelItem();
			labelPtr->SetBackgroundColor(color);
			labelPtr->SetGroupName(name);
			labelPtr->SetHeight(300);
			sceneHeight += 300;
			labelPtr->setPos(0, -height / 2 - i * height);
			labelPtr->setZValue(9);

			GroupItem groupItem;
			groupItem.backgroundPtr = rectPtr;
			groupItem.labelPtr = labelPtr;
			m_groupStaticItems[ids[i]] = groupItem;

			scenePtr->addItem(rectPtr);
			scenePtr->addItem(labelPtr);

			imod::IModel* groupModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<istd::IChangeable*>(m_groupViewProviderCompPtr->GetObjectPtr(ids[i])));
			if (groupModelPtr != nullptr){
				m_groupDispatcher.RegisterModel(groupModelPtr, i);
			}
		}
	}

	if (m_viewportGeometryManagerModelCompPtr.IsValid()){
		m_viewportGeometryManagerModelCompPtr->AttachObserver(&m_viewportObserver);
	}

	if (m_viewportGeometryManagerCompPtr.IsValid()){
		m_viewportGeometryManagerCompPtr->SetSceneRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewportGeometryManagerCompPtr->SetViewRect(QRectF(0, -sceneHeight, 3600, sceneHeight));
		m_viewportGeometryManagerCompPtr->SetMargins(QMargins(70, 0, 200, 40));
		m_viewportGeometryManagerCompPtr->SetScaleRangeX(istd::CRange(0, 1000));
		m_viewportGeometryManagerCompPtr->SetScaleRangeY(istd::CRange(0, 1000));
	}

	if (m_timeRangeModelCompPtr.IsValid()){
		m_timeRangeModelCompPtr->AttachObserver(&m_timeRangeObserver);
	}

	ConnectObserversToModels();
}


// reimplemented (icomp::CComponentBase)

void CGraphicsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CGraphicsControllerComp::OnComponentDestroyed()
{
	m_groupDispatcher.UnregisterAllModels();

	if (m_groupProviderCompPtr.IsValid()){
		DisconnectObserversFromModels();
	}

	BaseClass::OnComponentDestroyed();
}


// private slots

void CGraphicsControllerComp::OnAxisBeginTimeChanged(const QDateTime& /*oldTime*/, const QDateTime& /*newTime*/)
{
}


void CGraphicsControllerComp::OnAxisEndTimeChanged(const QDateTime& /*oldTime*/, const QDateTime& /*newTime*/)
{
}


// private methods

void CGraphicsControllerComp::OnViewportGeometryUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QRectF viewRect = propertyPtr->GetViewRect();
	m_timeAxisPtr->setPos(0, viewRect.bottom() - m_timeAxisPtr->rect().height() / propertyPtr->GetScaleY());

	m_visibleTimeRangeProvider.SetTimeRange(imtlog::CTimeRange(m_timeAxisPtr->GetVisibleBeginTime(), m_timeAxisPtr->GetVisibleEndTime()));

	for (const QByteArray& key : m_groupStaticItems.keys()){
		GroupItem& groupItem = m_groupStaticItems[key];
		QRectF rect = groupItem.backgroundPtr->rect();
		rect.setWidth(viewRect.width());
		groupItem.backgroundPtr->setRect(rect);
		groupItem.backgroundPtr->setPos(viewRect.x(), groupItem.backgroundPtr->pos().y());
		groupItem.labelPtr->setPos(viewRect.x(), groupItem.labelPtr->pos().y());
	}
}


void CGraphicsControllerComp::ConnectObserversToModels()
{
	// Iteration over groups
	QVector<QByteArray> groupIds = m_groupProviderCompPtr->GetElementIds();
	QVector<QByteArray> groupViewIds = m_groupViewProviderCompPtr->GetElementIds();
	for (int groupIndex = 0; groupIndex < groupIds.count(); groupIndex++){
		QByteArray groupId = groupIds[groupIndex];

		if (!groupViewIds.contains(groupId)){
			continue;
		}

		imod::IModel* groupModelPtr = dynamic_cast<imod::IModel*>(
			const_cast<istd::IChangeable*>(m_groupProviderCompPtr->GetObjectPtr(groupId)));
		imod::IObserver* groupObserverPtr = dynamic_cast<imod::IObserver*>(
			const_cast<istd::IChangeable*>(m_groupViewProviderCompPtr->GetObjectPtr(groupId)));

		if (groupModelPtr == nullptr || groupObserverPtr == nullptr){
			continue;
		}

		groupModelPtr->AttachObserver(groupObserverPtr);

		// Iteration over layers
		const imtbase::IObjectCollection* layerCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
			m_groupProviderCompPtr->GetObjectPtr(groupId));
		const imtbase::IObjectCollection* layerViewCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
			m_groupViewProviderCompPtr->GetObjectPtr(groupId));

		if (layerCollectionPtr == nullptr || layerViewCollectionPtr == nullptr){
			continue;
		}

		QVector<QByteArray> layerIds = layerCollectionPtr->GetElementIds();
		QVector<QByteArray> layerViewIds = layerViewCollectionPtr->GetElementIds();
		for (int layerIndex = 0; layerIndex < layerIds.count(); layerIndex++){
			QByteArray layerId = layerIds[layerIndex];

			if (!layerViewIds.contains(layerId)){
				continue;
			}

			imod::IModel* layerModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<istd::IChangeable*>(layerCollectionPtr->GetObjectPtr(layerId)));
			imod::IObserver* layerObserverPtr = dynamic_cast<imod::IObserver*>(
				const_cast<istd::IChangeable*>(layerViewCollectionPtr->GetObjectPtr(layerId)));

			if (layerModelPtr == nullptr || layerObserverPtr == nullptr){
				continue;
			}

			layerModelPtr->AttachObserver(layerObserverPtr);

			// Iteration over representations
			const imtbase::IObjectCollection* representationCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
				layerCollectionPtr->GetObjectPtr(layerId));
			const imtbase::IObjectCollection* representationViewCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
				layerViewCollectionPtr->GetObjectPtr(layerId));

			if (representationCollectionPtr == nullptr || representationViewCollectionPtr == nullptr){
				continue;
			}

			QVector<QByteArray> representationIds = representationCollectionPtr->GetElementIds();
			QVector<QByteArray> representationViewIds = representationViewCollectionPtr->GetElementIds();
			for (int representationIndex = 0; representationIndex < representationIds.count(); representationIndex++){
				QByteArray representationId = representationIds[representationIndex];

				if (!representationViewIds.contains(representationId)){
					continue;
				}

				imod::IModel* representationModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<istd::IChangeable*>(representationCollectionPtr->GetObjectPtr(representationId)));
				imod::IObserver* representationObserverPtr = dynamic_cast<imod::IObserver*>(
					const_cast<istd::IChangeable*>(representationViewCollectionPtr->GetObjectPtr(representationId)));

				if (representationModelPtr == nullptr || representationObserverPtr == nullptr){
					continue;
				}

				representationModelPtr->AttachObserver(representationObserverPtr);
			}
		}
	}
}


void CGraphicsControllerComp::DisconnectObserversFromModels()
{
	// Iteration over groups
	QVector<QByteArray> groupIds = m_groupProviderCompPtr->GetElementIds();
	for (int groupIndex = 0; groupIndex < groupIds.count(); groupIndex++){
		QByteArray groupId = groupIds[groupIndex];

		imod::IModel* groupModelPtr = dynamic_cast<imod::IModel*>(
			const_cast<istd::IChangeable*>(m_groupProviderCompPtr->GetObjectPtr(groupId)));

		if (groupModelPtr != nullptr){
			groupModelPtr->DetachAllObservers();
		}

		// Iteration over layers
		const imtbase::IObjectCollection* layerCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
			m_groupProviderCompPtr->GetObjectPtr(groupId));

		if (layerCollectionPtr == nullptr){
			continue;
		}

		QVector<QByteArray> layerIds = layerCollectionPtr->GetElementIds();
		for (int layerIndex = 0; layerIndex < layerIds.count(); layerIndex++){
			QByteArray layerId = layerIds[layerIndex];

			imod::IModel* layerModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<istd::IChangeable*>(layerCollectionPtr->GetObjectPtr(layerId)));

			if (layerModelPtr != nullptr){
				layerModelPtr->DetachAllObservers();
			}

			// Iteration over representations
			const imtbase::IObjectCollection* representationCollectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(
				layerCollectionPtr->GetObjectPtr(layerId));

			if (representationCollectionPtr == nullptr){
				continue;
			}

			QVector<QByteArray> representationIds = representationCollectionPtr->GetElementIds();
			for (int representationIndex = 0; representationIndex < representationIds.count(); representationIndex++){
				QByteArray representationId = representationIds[representationIndex];

				imod::IModel* representationModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<istd::IChangeable*>(representationCollectionPtr->GetObjectPtr(representationId)));

				if (representationModelPtr != nullptr){
					representationModelPtr->DetachAllObservers();
				}
			}
		}
	}
}


void CGraphicsControllerComp::OnGroupChanged(int modelId)
{
	if (!m_graphicsSceneProviderCompPtr.IsValid()){
		return;
	}

	QGraphicsScene* scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
	if (scenePtr == nullptr){
		return;
	}

	qint64 span = m_timeAxisPtr->GetVisibleBeginTime().msecsTo(m_timeAxisPtr->GetVisibleEndTime());

	if (m_groupViewProviderCompPtr.IsValid() && span > 0){
		imtbase::ICollectionInfo::Ids groupIds = m_groupViewProviderCompPtr->GetElementIds();
		QByteArray groupId = groupIds[modelId];

		for (QGraphicsItem* itemPtr : scenePtr->items()){
			QVariant data = itemPtr->data(0);
			if (data.type() != QVariant::ByteArray){
				continue;
			}

			if (data.toByteArray() == groupId){
				QPointF newPos = itemPtr->pos();
				newPos.ry() = m_groupStaticItems[groupId].backgroundPtr->pos().y() + m_groupStaticItems[groupId].backgroundPtr->rect().height() / 2;
				itemPtr->setPos(newPos);
				itemPtr->update();
			}
		}
	}
}


// public methods of the embedded class ViewportObserver

CGraphicsControllerComp::ViewportObserver::ViewportObserver()
	:m_parent(nullptr)
{
}

void CGraphicsControllerComp::ViewportObserver::SetParent(CGraphicsControllerComp* parent)
{
	m_parent = parent;
}


// protected methods of the embedded class ViewportObserver

// reimplemented (imod::CSingleModelObserverBase)

void CGraphicsControllerComp::ViewportObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent->OnViewportGeometryUpdate(GetObservedObject(), changeSet);
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


// protected methods of the embedded class TimeRangeObserver

// reimplemented (imod::CSingleModelObserverBase)

void CGraphicsControllerComp::TimeRangeObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();

	if (timeRange.GetBeginTime().isValid()){
		m_parent->m_timeAxisPtr->EnsureTimeRange(timeRange.GetBeginTime());
		m_parent->m_timeAxisPtr->EnsureTimeRange(timeRange.GetEndTime());

		if (m_parent->m_viewportGeometryProviderCompPtr.IsValid()){
			QRectF rect = m_parent->m_viewportGeometryProviderCompPtr->GetSceneRect();
			rect.setWidth(m_parent->m_timeAxisPtr->rect().width());
			m_parent->m_viewportGeometryManagerCompPtr->SetSceneRect(rect);
		}
	}
}


// public methods of the embedded class TimeRangeProvider

CGraphicsControllerComp::TimeRangeProvider::TimeRangeProvider()
	:m_parent(nullptr)
{
}

void CGraphicsControllerComp::TimeRangeProvider::SetParent(CGraphicsControllerComp* parent)
{
	m_parent = parent;
}


void CGraphicsControllerComp::TimeRangeProvider::SetTimeRange(const imtlog::CTimeRange& timeRange)
{
	if (m_timeRange != timeRange){
		istd::CChangeNotifier notifier(this);
		m_timeRange = timeRange;
	}
}


// reimplemented (imtlog::ITimeRangeProvider)

imtlog::CTimeRange CGraphicsControllerComp::TimeRangeProvider::GetTimeRange() const
{
	return m_timeRange;
}


// protected methods of the embedded class GroupsDispatcher

// reimplemented (imod::CMultiModelDispatcherBase)

void CGraphicsControllerComp::GroupsDispatcher::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_parentPtr != nullptr){
		m_parentPtr->OnGroupChanged(modelId);
	}
}



} // namespace imtloggui


