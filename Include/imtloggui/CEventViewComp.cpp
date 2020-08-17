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
	: m_viewPtr(nullptr),
	m_splitterPtr(nullptr),
	m_panelsStackPtr(nullptr),
	m_metaInfoPanelPtr(nullptr),
	m_scaleConstraintsObserver(*this),
	m_rootCommands("", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_moveToFirstCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToPreviousCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToNextCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToLastCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_isNavigationIteratorValid(false)
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
	m_messageProcessingTimer.callOnTimeout(this, &CEventViewComp::OnMessageProcessingTimer);
}


// reimplemented (ilog::IMessageConsumer)

bool CEventViewComp::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	return true;
}


void CEventViewComp::AddMessage(const IMessageConsumer::MessagePtr& message)
{
	BaseClass::AddMessage(message);

	QMutexLocker locker(&m_messageListMutex);
	m_messageList.append(message);
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

	m_splitterPtr = new QSplitter();
	layoutPtr->insertWidget(0, m_splitterPtr);

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	m_viewPtr->setScene(&m_scene);
	m_viewPtr->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_viewPtr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_viewPtr->SetMargins(QMargins(70, 0, 70, 40));
	m_splitterPtr->addWidget(m_viewPtr);

	m_panelsStackPtr = new QStackedWidget(GetQtWidget());
	m_splitterPtr->addWidget(m_panelsStackPtr);
	m_splitterPtr->setStretchFactor(0, 9);
	m_splitterPtr->setStretchFactor(0, 1);
	m_splitterPtr->setCollapsible(0, false);

	if (m_statisticsViewCompPtr.IsValid()){
		m_statisticsPanelPtr = new QWidget(GetQtWidget());
		m_statisticsViewCompPtr->CreateGui(m_statisticsPanelPtr);
		m_panelsStackPtr->addWidget(m_statisticsViewCompPtr->GetWidget());
	}

	m_metaInfoPanelPtr = new QWidget();
	if (m_metaInfoPanelPtr->layout() == nullptr){
		m_metaInfoPanelPtr->setLayout(new QVBoxLayout());
	}
	m_panelsStackPtr->addWidget(m_metaInfoPanelPtr);

	m_panelsStackPtr->setCurrentIndex(0);

	m_timeAxis.SetColor(Qt::green);
	m_timeAxis.setRect(0, 0, 100, 40);
	m_timeAxis.setZValue(101);
	m_scene.addItem(&m_timeAxis);

	connect(&m_scene, &QGraphicsScene::selectionChanged, this, &CEventViewComp::OnSelectionChanged);
	connect(&m_timeAxis, &CTimeAxis::EmitAxisPosChanged, this, &CEventViewComp::OnAxisPosChanged);
	connect(&m_timeAxis, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CEventViewComp::OnAxisBeginTimeChanged);
	connect(&m_timeAxis, &CTimeAxis::EmitAxisEndTimeChanged, this, &CEventViewComp::OnAxisEndTimeChanged);
	connect(m_viewPtr, &CEventGraphicsView::EmitViewPortChanged, this, &CEventViewComp::OnViewPortChanged);
	connect(this, &CEventViewComp::EmitShowAll, m_viewPtr, &CEventGraphicsView::OnShowAll, Qt::QueuedConnection);

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->SetScene(&m_scene);
		m_groupControllerCompPtr->SetView(m_viewPtr);
		m_groupControllerCompPtr->SetTimeAxis(&m_timeAxis);
		m_groupControllerCompPtr->CreateGraphicsItem();

		QByteArrayList groupIds = m_groupControllerCompPtr->GetAvailableGroupList();
		for (QByteArray id : groupIds){
			m_groupControllerCompPtr->AddGroup(id);
		}

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_scaleConstraintsCompPtr.GetPtr());
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_scaleConstraintsObserver);
		}
	}

	m_timeAxis.EnsureTimeRange(QDateTime::currentDateTime());

	UpdateCommands();

	m_messageProcessingTimer.setInterval(100);
	m_messageProcessingTimer.start();
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(&m_scene, &QGraphicsScene::selectionChanged, this, &CEventViewComp::OnSelectionChanged);
	disconnect(&m_timeAxis, &CTimeAxis::EmitAxisPosChanged, this, &CEventViewComp::OnAxisPosChanged);
	disconnect(&m_timeAxis, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CEventViewComp::OnAxisBeginTimeChanged);
	disconnect(&m_timeAxis, &CTimeAxis::EmitAxisEndTimeChanged, this, &CEventViewComp::OnAxisEndTimeChanged);
	disconnect(m_viewPtr, &CEventGraphicsView::EmitViewPortChanged, this, &CEventViewComp::OnViewPortChanged);
	disconnect(this, &CEventViewComp::EmitShowAll, m_viewPtr, &CEventGraphicsView::OnShowAll);

	if (m_viewPtr != nullptr){
		delete m_viewPtr;
	}

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_scaleConstraintsCompPtr.GetPtr());
	if (modelPtr != nullptr){
		modelPtr->DetachObserver(&m_scaleConstraintsObserver);
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

	m_slaveMessageConsumerCompPtr.EnsureInitialized();
}


// private slots

void CEventViewComp::OnViewPortChanged(bool userAction)
{
	QRectF visibleRect = m_viewPtr->GetSceneVisibleRect();

	m_timeAxis.setPos(0, visibleRect.bottom() - m_timeAxis.rect().height() / m_viewPtr->GetScaleY());

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnViewPortChanged();
	}

	if (userAction){
		m_currentCommandTime = QDateTime();
		m_isNavigationIteratorValid = false;
	}

	UpdateCommands();
}


void CEventViewComp::OnAxisPosChanged(const QPointF& oldPos, const QPointF& newPos)
{
	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnAxisPosChanged(oldPos, newPos);
	}
}


void CEventViewComp::OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	if(!oldTime.isValid()){
		QRectF rect = m_viewPtr->GetSceneRect();
		rect.setLeft(m_timeAxis.rect().left());
		rect.setRight(m_timeAxis.rect().right());
		m_viewPtr->SetSceneRect(rect);
		m_viewPtr->SetViewRect(rect);
		m_currentCommandTime = QDateTime();

		Q_EMIT EmitShowAll();
	}
	else{
		QRectF rect = m_viewPtr->GetSceneRect();
		rect.setLeft(m_timeAxis.rect().left());
		rect.setRight(m_timeAxis.rect().right());
		m_viewPtr->SetSceneRect(rect);

		double shift = (oldTime.toMSecsSinceEpoch() - newTime.toMSecsSinceEpoch()) / 1000.;
		QRectF viewRect = m_viewPtr->GetViewRect().translated(shift, 0);
		m_viewPtr->SetViewRect(viewRect);
	}

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnAxisBeginTimeChanged(oldTime, newTime);
	}
}


void CEventViewComp::OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	QRectF rect = m_viewPtr->GetSceneRect();
	rect.setLeft(m_timeAxis.rect().left());
	rect.setRight(m_timeAxis.rect().right());
	m_viewPtr->SetSceneRect(rect);

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnAxisEndTimeChanged(oldTime, newTime);
	}
}


void CEventViewComp::OnMoveToFirstCommand()
{
	if (!m_eventMap.isEmpty()){
		m_navigationIterator = m_eventMap.cbegin();
		m_isNavigationIteratorValid = true;

		QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
		if (graphicsItem != nullptr){
			MoveToTime(m_navigationIterator.key());
			m_scene.clearSelection();
			graphicsItem->setSelected(true);
		}

		UpdateCommands();
	}
}


void CEventViewComp::OnMoveToPreviousCommand()
{
	if (!m_eventMap.isEmpty()){
		if (m_isNavigationIteratorValid){
			if (m_navigationIterator == m_eventMap.cbegin()){
				return;
			}

			m_navigationIterator--;
		}
		else{
			QDateTime time = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
			m_navigationIterator = --m_eventMap.lowerBound(time);
			m_isNavigationIteratorValid = true;
		}

		QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
		if (graphicsItem != nullptr){
			MoveToTime(m_navigationIterator.key());
			m_scene.clearSelection();
			graphicsItem->setSelected(true);
		}

		UpdateCommands();
	}
}


void CEventViewComp::OnMoveToNextCommand()
{
	if (!m_eventMap.isEmpty()){
		if (m_isNavigationIteratorValid){
			if (m_navigationIterator == --m_eventMap.cend()){
				return;
			}

			m_navigationIterator++;
		}
		else{
			QDateTime time = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
			m_navigationIterator = m_eventMap.lowerBound(time);
			m_isNavigationIteratorValid = true;
		}

		QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
		if (graphicsItem != nullptr){
			MoveToTime(m_navigationIterator.key());
			m_scene.clearSelection();
			graphicsItem->setSelected(true);
		}

		UpdateCommands();
	}
}


void CEventViewComp::OnMoveToLastCommand()
{
	if (!m_eventMap.isEmpty()){
		m_navigationIterator = --m_eventMap.cend();
		m_isNavigationIteratorValid = true;

		QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(m_navigationIterator.value());
		if (graphicsItem != nullptr){
			MoveToTime(m_navigationIterator.key());
			m_scene.clearSelection();
			graphicsItem->setSelected(true);
		}

		UpdateCommands();
	}
}


void CEventViewComp::OnSelectionChanged()
{
	QList<QGraphicsItem*> items = m_scene.selectedItems();
	m_panelsStackPtr->setCurrentIndex(0);
	if (!items.isEmpty()){
		IEventItem* itemPtr = dynamic_cast<IEventItem*>(items[0]);
		if (itemPtr != nullptr){
			if (UpdateMetaInfoPanel(itemPtr)){
				m_panelsStackPtr->setCurrentIndex(1);
			}
		}
	}

	m_scene.update(m_viewPtr->GetViewRect().toRect());
}


void CEventViewComp::OnUpdateSceneRect()
{
}


void CEventViewComp::OnMessageProcessingTimer()
{
	if (IsGuiCreated()){
		if (m_groupControllerCompPtr.IsValid()){
			QMutexLocker locker(&m_messageListMutex);
			while (m_messageList.count()){
				MessagePtr message = m_messageList.takeFirst();
				locker.unlock();

				m_timeAxis.EnsureTimeRange(message->GetInformationTimeStamp());

				IEventItem* eventItemPtr = m_groupControllerCompPtr->AddEvent(message);
				if (eventItemPtr != nullptr){
					m_eventMap.insert(message->GetInformationTimeStamp(), eventItemPtr);
				}
			}

			UpdateCommands();
		}
		else{
			QMutexLocker locker(&m_messageListMutex);
			m_messageList.clear();
		}
	}
}


// private methods

QRectF CEventViewComp::GetSceneVisibleRect() const
{
	if (m_viewPtr == nullptr){
		return QRectF();
	}

	QRect viewportRect = m_viewPtr->viewport()->rect();

	QRectF visibleSceneRect = m_viewPtr->mapToScene(viewportRect).boundingRect();

	return visibleSceneRect;
}


double CEventViewComp::GetCurrentScaleX() const
{
	if (m_viewPtr != nullptr){
		return m_viewPtr->GetScaleX();
	}

	return 0;
}


void CEventViewComp::UpdateVerticalRangeScale(const istd::CRange & range) const
{
	if (m_viewPtr != nullptr){
		m_viewPtr->OnMinimumVerticalScaleChanged(range.GetMinValue());
	}
}


void CEventViewComp::UpdateCommands()
{
	QDateTime currentTime;
	currentTime = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());

	bool enablePrev = false;
	bool enableNext = false;

	if (!m_eventMap.isEmpty()){
		const IEventItemController::EventMap::const_iterator itBegin = m_eventMap.cbegin();
		const IEventItemController::EventMap::const_iterator itEnd = --m_eventMap.cend();

		if (m_isNavigationIteratorValid){
			if (m_navigationIterator != itBegin){
				enablePrev = true;
			}
			
			if (m_navigationIterator != itEnd){
				enableNext = true;
			}
		}
		else{
			if (currentTime >= itBegin.key()){
				enablePrev = true;
			}

			if (currentTime <= itEnd.key()){
				enableNext = true;
			}
		}

		m_moveToFirstCommand.SetEnabled(true);
		m_moveToLastCommand.SetEnabled(true);
	}
	else{
		m_moveToFirstCommand.SetEnabled(false);
		m_moveToLastCommand.SetEnabled(false);
	}

	m_moveToPreviousCommand.SetEnabled(enablePrev);
	m_moveToNextCommand.SetEnabled(enableNext);
}


void CEventViewComp::MoveToTime(const QDateTime& time)
{
	double beginTime = m_timeAxis.GetBeginTime().toMSecsSinceEpoch() / 1000.0;
	double endTime = m_timeAxis.GetEndTime().toMSecsSinceEpoch() / 1000.0;
	double currentTime = time.toMSecsSinceEpoch() / 1000.0;

	if (currentTime <  beginTime || currentTime > endTime){
		return;
	}

	double visibleTime = m_viewPtr->viewport()->rect().width() / GetCurrentScaleX();

	if ((currentTime - beginTime > visibleTime / 2) && (endTime - currentTime > visibleTime / 2)){
		QRectF rect = GetSceneVisibleRect();
		double center = rect.center().x();
		double newCenter = m_timeAxis.GetScenePositionFromTime(time);

		rect.translate(newCenter - center, 0);
		m_viewPtr->SetViewRect(rect);
	}
	else{
		double delta = qMin(currentTime - beginTime, endTime - currentTime);	

		m_viewPtr->scale(GetSceneVisibleRect().width() / (2 * delta), 1);

		QRectF rect = GetSceneVisibleRect();
		double center = rect.center().x();
		double newCenter = m_timeAxis.GetScenePositionFromTime(time);

		rect.setWidth(2 * delta);
		rect.translate(newCenter - center, 0);
		m_viewPtr->SetViewRect(rect);
	}
}


bool CEventViewComp::UpdateMetaInfoPanel(const IEventItem* eventItem)
{
	iwidgets::ClearLayout(m_metaInfoPanelPtr->layout());

	if (eventItem == nullptr){
		return false;
	}

	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(m_metaInfoPanelPtr->layout());
	if (layoutPtr != nullptr){
		IEventItem::MetaInfo metaInfo = eventItem->GetMetaInfo();
		if (metaInfo.isEmpty()){
			return false;
		}

		for (IEventItem::MetaInfoItem metaInfoItem : metaInfo){
			QLabel* labelKeyPtr = new QLabel(metaInfoItem.key, m_metaInfoPanelPtr);
			QFont keyLabelFont;
			keyLabelFont.setPixelSize(12);
			keyLabelFont.setBold(true);
			labelKeyPtr->setFont(keyLabelFont);
			labelKeyPtr->setStyleSheet("color: #88b8e3");

			QFont valueLabelFont;
			valueLabelFont.setPixelSize(9);
			QLabel* labelValuePtr = new QLabel(metaInfoItem.value, m_metaInfoPanelPtr);
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


// protected methods of the embedded class ScaleConstraintsObserver

// reimplemented (imod::CSingleModelObserverBase)

CEventViewComp::ScaleConstraintsObserver::ScaleConstraintsObserver(CEventViewComp & parent)
	:m_parent(parent)
{
}


void CEventViewComp::ScaleConstraintsObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imeas::INumericConstraints* constraintsPtr = GetObservedObject();
	Q_ASSERT(constraintsPtr != nullptr);

	istd::CRange range = constraintsPtr->GetNumericValueUnitInfo(0)->GetValueRange();

	m_parent.UpdateVerticalRangeScale(range);
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


