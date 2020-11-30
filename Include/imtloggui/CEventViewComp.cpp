#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>

// ACF includes
#include <iser/IObject.h>
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtloggui/CEventGraphicsView.h>


namespace imtloggui
{


// public methods

CEventViewComp::CEventViewComp()
	:m_splitterPtr(nullptr),
	m_panelsStackPtr(nullptr),
	m_statisticsPanelPtr(nullptr),
	m_metaInfoPanelPtr(nullptr),
	m_rootCommands("", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_moveToFirstCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToPreviousCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToNextCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToLastCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_graphicsItemObserver(this)
	//m_isNavigationIteratorValid(false)
{
	m_commands.SetParent(this);
	m_rootCommands.InsertChild(&m_moveToFirstCommand);
	m_rootCommands.InsertChild(&m_moveToPreviousCommand);
	m_rootCommands.InsertChild(&m_moveToNextCommand);
	m_rootCommands.InsertChild(&m_moveToLastCommand);

	connect(&m_moveToFirstCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToFirstCommand);
	connect(&m_moveToPreviousCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToPreviousCommand);
	connect(&m_moveToNextCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToNextCommand);
	connect(&m_moveToLastCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToLastCommand);
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CEventViewComp::OnRestoreSettings(const QSettings& settings)
{
	int viewWidth = 0;
	int rightPanelWidth = 0;

	QByteArray settingsKey("EventViewSettings/EventViewWidth");
	if (settings.contains(settingsKey)){
		QVariant settingsValue = settings.value(settingsKey);
		if (settingsValue.type() == QVariant::Int){
			viewWidth = settingsValue.toInt();
		}
	}

	settingsKey = "EventViewSettings/RightPanelWidth";
	if (settings.contains(settingsKey)){
		QVariant settingsValue = settings.value(settingsKey);
		if (settingsValue.type() == QVariant::Int){
			rightPanelWidth = settingsValue.toInt();
		}
	}

	if (viewWidth > 0 && rightPanelWidth >= 0){
		QList<int> sizes = {viewWidth, rightPanelWidth};
		m_splitterPtr->setSizes(sizes);
	}
}


void CEventViewComp::OnSaveSettings(QSettings& settings) const
{
	settings.beginGroup("EventViewSettings");
	settings.setValue("EventViewWidth", m_splitterPtr->sizes()[0]);
	settings.setValue("RightPanelWidth", m_splitterPtr->sizes()[1]);
	settings.endGroup();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	QHBoxLayout* layoutPtr = dynamic_cast<QHBoxLayout*>(GetQtWidget()->layout());

	m_splitterPtr = new QSplitter(GetQtWidget());
	layoutPtr->insertWidget(0, m_splitterPtr);

	m_viewPtr = new imod::TModelWrap<CEventGraphicsView>();
	m_viewPtr->setParent(GetQtWidget());
	m_viewPtr->setScene(&m_scene);
	m_viewPtr->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_viewPtr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_splitterPtr->addWidget(m_viewPtr);

	m_panelsStackPtr = new QStackedWidget(GetQtWidget());
	m_splitterPtr->addWidget(m_panelsStackPtr);
	m_splitterPtr->setStretchFactor(0, 100);
	m_splitterPtr->setCollapsible(0, false);

	if (m_statisticsViewCompPtr.IsValid()){
		m_statisticsPanelPtr = new QWidget(GetQtWidget());
		m_statisticsViewCompPtr->CreateGui(m_statisticsPanelPtr);
		m_panelsStackPtr->addWidget(m_statisticsViewCompPtr->GetWidget());
	}

	if (m_metainfoViewCompPtr.IsValid()){
		m_metaInfoPanelPtr = new QWidget(GetQtWidget());
		m_metainfoViewCompPtr->CreateGui(m_metaInfoPanelPtr);
		m_panelsStackPtr->addWidget(m_metainfoViewCompPtr->GetWidget());

		imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(m_metainfoViewCompPtr.GetPtr());
		if (observerPtr != nullptr){
			m_modelProxy.AttachObserver(observerPtr);
		}
	}

	m_panelsStackPtr->setCurrentIndex(0);

	connect(&m_scene, &QGraphicsScene::selectionChanged, this, &CEventViewComp::OnSelectionChanged);

	if (m_itemProviderModelCompPtr.IsValid()){
		m_itemProviderModelCompPtr->AttachObserver(&m_graphicsItemObserver);
	}

	UpdateCommands();
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(&m_scene, &QGraphicsScene::selectionChanged, this, &CEventViewComp::OnSelectionChanged);

	m_graphicsItemObserver.EnsureModelDetached();
	for (QGraphicsItem* itemPtr : m_scene.items()){
		m_scene.removeItem(itemPtr);
	}

	if (m_statisticsViewCompPtr.IsValid() && m_statisticsViewCompPtr->IsGuiCreated()){
		m_statisticsViewCompPtr->DestroyGui();
	}

	if (m_metainfoViewCompPtr.IsValid() && m_metainfoViewCompPtr->IsGuiCreated()){
		m_metainfoViewCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


void CEventViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_moveToFirstCommand.SetVisuals(tr("First event"), tr("First"), tr("Move to first event"), QIcon(":/Icons/MoveFirst"));
	m_moveToPreviousCommand.SetVisuals(tr("Previous event"), tr("Previous"), tr("Move to previous event"), QIcon(":/Icons/Left"));
	m_moveToNextCommand.SetVisuals(tr("Next event"), tr("Next"), tr("Move to next event"), QIcon(":/Icons/Right"));
	m_moveToLastCommand.SetVisuals(tr("Last event"), tr("Last"), tr("Move to last event"), QIcon(":/Icons/MoveLast"));
}


// reimplemented (icomp::CComponentBase)

void CEventViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


// private slots

void CEventViewComp::OnMoveToFirstCommand()
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


void CEventViewComp::OnMoveToPreviousCommand()
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


void CEventViewComp::OnMoveToNextCommand()
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


void CEventViewComp::OnMoveToLastCommand()
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


void CEventViewComp::OnSelectionChanged()
{
	QList<QGraphicsItem*> items = m_scene.selectedItems();
	if (!items.isEmpty()){
		IItemBase* itemPtr = dynamic_cast<IItemBase*>(items[0]);
		if (itemPtr != nullptr){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(itemPtr);
			if (modelPtr != nullptr){
				m_modelProxy.SetModelPtr(modelPtr);
				m_panelsStackPtr->setCurrentIndex(1);
				m_scene.update(m_viewPtr->GetViewRect().toRect());
				return;
			}
		}
	}

	m_panelsStackPtr->setCurrentIndex(0);
	m_modelProxy.SetModelPtr(nullptr);
	m_scene.update(m_viewPtr->GetViewRect().toRect());
}


// private methods

QRectF CEventViewComp::GetSceneVisibleRect() const
{
	QRect viewportRect = m_viewPtr->viewport()->rect();

	QRectF visibleSceneRect = m_viewPtr->mapToScene(viewportRect).boundingRect();

	return visibleSceneRect;
}


double CEventViewComp::GetCurrentScaleX() const
{
	return m_viewPtr->GetScaleX();
}


void CEventViewComp::UpdateCommands()
{
	//QDateTime currentTime;
	//currentTime = m_timeAxisPtr.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());

	//bool enablePrev = false;
	//bool enableNext = false;

	//if (!m_eventMap.isEmpty()){
	//	const IEventItemController::EventMap::const_iterator itBegin = m_eventMap.cbegin();
	//	const IEventItemController::EventMap::const_iterator itEnd = --m_eventMap.cend();

	//	if (m_isNavigationIteratorValid){
	//		if (m_navigationIterator != itBegin){
	//			enablePrev = true;
	//		}
	//		
	//		if (m_navigationIterator != itEnd){
	//			enableNext = true;
	//		}
	//	}
	//	else{
	//		if (currentTime >= itBegin.key()){
	//			enablePrev = true;
	//		}

	//		if (currentTime <= itEnd.key()){
	//			enableNext = true;
	//		}
	//	}

	//	m_moveToFirstCommand.SetEnabled(true);
	//	m_moveToLastCommand.SetEnabled(true);
	//}
	//else{
	//	m_moveToFirstCommand.SetEnabled(false);
	//	m_moveToLastCommand.SetEnabled(false);
	//}

	//m_moveToPreviousCommand.SetEnabled(enablePrev);
	//m_moveToNextCommand.SetEnabled(enableNext);
}


void CEventViewComp::MoveToTime(const QDateTime& time)
{
	//double beginTime = m_timeAxisPtr.GetBeginTime().toMSecsSinceEpoch() / 1000.0;
	//double endTime = m_timeAxisPtr.GetEndTime().toMSecsSinceEpoch() / 1000.0;
	//double currentTime = time.toMSecsSinceEpoch() / 1000.0;

	//if (currentTime <  beginTime || currentTime > endTime){
	//	return;
	//}

	//double visibleTime = m_viewPtr->viewport()->rect().width() / GetCurrentScaleX();

	//if ((currentTime - beginTime > visibleTime / 2) && (endTime - currentTime > visibleTime / 2)){
	//	QRectF rect = GetSceneVisibleRect();
	//	double center = rect.center().x();
	//	double newCenter = m_timeAxisPtr.GetScenePositionFromTime(time);

	//	rect.translate(newCenter - center, 0);
	//	m_viewPtr->SetViewRect(rect);
	//}
	//else{
	//	double delta = qMin(currentTime - beginTime, endTime - currentTime);	

	//	m_viewPtr->scale(GetSceneVisibleRect().width() / (2 * delta), 1);

	//	QRectF rect = GetSceneVisibleRect();
	//	double center = rect.center().x();
	//	double newCenter = m_timeAxisPtr.GetScenePositionFromTime(time);

	//	rect.setWidth(2 * delta);
	//	rect.translate(newCenter - center, 0);
	//	m_viewPtr->SetViewRect(rect);
	//}
}


bool CEventViewComp::UpdateMetaInfoPanel(const IEventItem* eventItem)
{
	iwidgets::ClearLayout(m_metaInfoPanelPtr->layout());

	if (eventItem == nullptr){
		return false;
	}

	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(m_metaInfoPanelPtr->layout());
	if (layoutPtr != nullptr){
		idoc::IDocumentMetaInfo::MetaInfoTypes types = eventItem->GetMetaInfoTypes();
		for (int type : types){
			QLabel* labelKeyPtr = new QLabel(eventItem->GetMetaInfoName(type), m_metaInfoPanelPtr);
			QFont keyLabelFont;
			keyLabelFont.setPixelSize(12);
			keyLabelFont.setBold(true);
			labelKeyPtr->setFont(keyLabelFont);
			labelKeyPtr->setStyleSheet("color: #88b8e3");

			QFont valueLabelFont;
			valueLabelFont.setPixelSize(9);
			QLabel* labelValuePtr = new QLabel(eventItem->GetMetaInfo(type).toString(), m_metaInfoPanelPtr);
			labelValuePtr->setFont(valueLabelFont);
			labelValuePtr->setStyleSheet("color: gray");
			labelValuePtr->setWordWrap(true);

			layoutPtr->addWidget(labelKeyPtr);
			layoutPtr->addWidget(labelValuePtr);

			layoutPtr->addSpacing(10);
		}

		layoutPtr->addStretch();

		return true;
	}

	return false;
}


// public methods of the embedded class StaticItemsProvider

CEventViewComp::ItemsObserver::ItemsObserver()
	:m_parent(nullptr)
{
}


void CEventViewComp::ItemsObserver::SetParent(CEventViewComp* parent)
{
	m_parent = parent;
}


// protected methods of the embedded class StaticItemsProvider

// reimplemented (imtloggui::IGraphicsItemProvider)

void CEventViewComp::ItemsObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{

}


// public methods of the embedded class StaticItemsProvider

CEventViewComp::GraphicsItemsObserver::GraphicsItemsObserver(CEventViewComp* parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class StaticItemsProvider

// reimplemented (imtloggui::IGraphicsItemProvider)

void CEventViewComp::GraphicsItemsObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_parent != nullptr){
		for (IGraphicsItemProvider::GraphicsItem item : GetObservedObject()->GetRemovedItems()){
			if (m_items.contains(item)){
				m_parent->m_scene.removeItem(item.GetPtr());
				m_items.removeOne(item);
			}
		}

		for (IGraphicsItemProvider::GraphicsItem item : GetObservedObject()->GetAddedItems()){
			if (!m_items.contains(item)){
				m_parent->m_scene.addItem(item.GetPtr());
				m_items.append(item);
			}
		}

		qDebug() << "Items on scene: " << m_items.count();
	}
}


// public methods of the embedded class Commands

CEventViewComp::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CEventViewComp::Commands::SetParent(CEventViewComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CEventViewComp::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	return &m_parentPtr->m_rootCommands;
}


} // namespace imtloggui


