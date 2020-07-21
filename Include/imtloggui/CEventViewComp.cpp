#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QtWidgets/QLabel>

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
	m_summaryInfoPanelPtr(nullptr),
	m_metaInfoPanelPtr(nullptr),
	m_scaleConstraintsObserver(*this),
	m_rootCommands("", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_moveToFirstCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToPreviousCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToNextCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR),
	m_moveToLastCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
	m_commands.SetParent(this);
	m_rootCommands.InsertChild(&m_moveToFirstCommand);
	m_rootCommands.InsertChild(&m_moveToPreviousCommand);
	m_rootCommands.InsertChild(&m_moveToNextCommand);
	m_rootCommands.InsertChild(&m_moveToLastCommand);

	m_moveToFirstCommand.SetVisuals(tr("First event"), tr("First"), tr("Move to first event"), QIcon(":/Icons/Undo"));
	m_moveToPreviousCommand.SetVisuals(tr("Previous event"), tr("Previous"), tr("Move to previous event"), QIcon(":/Icons/Up"));
	m_moveToNextCommand.SetVisuals(tr("Next event"), tr("Next"), tr("Move to next event"), QIcon(":/Icons/Down"));
	m_moveToLastCommand.SetVisuals(tr("Last event"), tr("Last"), tr("Move to last event"), QIcon(":/Icons/Redo"));

	connect(&m_moveToFirstCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToFirstCommand);
	connect(&m_moveToPreviousCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToPreviousCommand);
	connect(&m_moveToNextCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToNextCommand);
	connect(&m_moveToLastCommand, &QAction::triggered, this, &CEventViewComp::OnMoveToLastCommand);
}


// reimplemented (ilog::IMessageConsumer)

bool CEventViewComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CEventViewComp::AddMessage(const IMessageConsumer::MessagePtr& message)
{
	if (!IsGuiCreated()){
		m_messageList.append(message);
		return;
	}

	if (m_groupControllerCompPtr.IsValid()){
		m_timeAxis.EnsureTimeRange(message->GetInformationTimeStamp());
		m_groupControllerCompPtr->AddEvent(message);
		UpdateSummaryInfoPanel();
	}

	UpdateCommands();
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

	m_summaryInfoPanelPtr = new QWidget();
	if (m_summaryInfoPanelPtr->layout() == nullptr){
		m_summaryInfoPanelPtr->setLayout(new QVBoxLayout());
	}
	m_panelsStackPtr->addWidget(m_summaryInfoPanelPtr);

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

	UpdateSummaryInfoPanel();

	if (m_messageList.isEmpty()){
		m_timeAxis.EnsureTimeRange(QDateTime::currentDateTime());
	}
	else{
		for (const ilog::IMessageConsumer::MessagePtr& message : m_messageList){
			AddMessage(message);
		}

		m_messageList.clear();
	}

	UpdateCommands();
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
	if (!m_groupControllerCompPtr.IsValid()){
		return;
	}

	QDateTime time;
	for (QByteArray id : m_groupControllerCompPtr->GetActiveGroupList()){
		IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

		const IEventItemController::EventMap* eventMap = groupPtr->GetEvents();

		if (eventMap->isEmpty()){
			continue;
		}

		if (!time.isValid() || time > eventMap->firstKey()){
			time = eventMap->firstKey();
		}
	}

	if (!time.isValid()){
		return;
	}

	m_currentCommandTime = time;
	MoveToTime(time);

	UpdateCommands();
}


void CEventViewComp::OnMoveToPreviousCommand()
{
	if (!m_groupControllerCompPtr.IsValid()){
		return;
	}

	QDateTime currentTime;
	if (m_currentCommandTime.isValid()){
		currentTime = m_currentCommandTime;
	}
	else{
		currentTime = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
	}

	QDateTime time;
	for (QByteArray id : m_groupControllerCompPtr->GetActiveGroupList()){
		IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

		const IEventItemController::EventMap* eventMap = groupPtr->GetEvents();

		if (eventMap->isEmpty()){
			continue;
		}

		IEventItemController::EventMap::const_iterator it = eventMap->lowerBound(currentTime);
		if (it == eventMap->begin()){
			continue;
		}

		it--;

		if (!time.isValid() || time < it.key()){
			time = it.key();
		}
	}

	if (!time.isValid()){
		return;
	}

	m_currentCommandTime = time;
	MoveToTime(time);

	UpdateCommands();
}


void CEventViewComp::OnMoveToNextCommand()
{
	if (!m_groupControllerCompPtr.IsValid()){
		return;
	}

	QDateTime currentTime;
	if (m_currentCommandTime.isValid()){
		currentTime = m_currentCommandTime;
	}
	else{
		currentTime = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
	}

	QDateTime time;
	for (QByteArray id : m_groupControllerCompPtr->GetActiveGroupList()){
		IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

		const IEventItemController::EventMap* eventMap = groupPtr->GetEvents();

		if (eventMap->isEmpty()){
			continue;
		}

		IEventItemController::EventMap::const_iterator it = eventMap->upperBound(currentTime);
		if (it == eventMap->end()){
			continue;
		}

		if (!time.isValid() || time > it.key()){
			time = it.key();
		}
	}

	if (!time.isValid()){
		return;
	}

	m_currentCommandTime = time;
	MoveToTime(time);

	UpdateCommands();
}


void CEventViewComp::OnMoveToLastCommand()
{
	if (!m_groupControllerCompPtr.IsValid()){
		return;
	}

	QDateTime time;
	for (QByteArray id : m_groupControllerCompPtr->GetActiveGroupList()){
		IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

		const IEventItemController::EventMap* eventMap = groupPtr->GetEvents();

		if (eventMap->isEmpty()){
			continue;
		}

		if (!time.isValid() || time < eventMap->lastKey()){
			time = eventMap->lastKey();
		}
	}

	if (!time.isValid()){
		return;
	}

	m_currentCommandTime = time;
	MoveToTime(time);

	UpdateCommands();
}


void CEventViewComp::OnSelectionChanged()
{
	QList<QGraphicsItem*> items = m_scene.selectedItems();
	m_panelsStackPtr->setCurrentIndex(0);
	if (!items.isEmpty()){
		CEventItemBase* itemPtr = dynamic_cast<CEventItemBase*>(items[0]);
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
	//static istd::IChangeable::ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);
	//istd::CChangeNotifier changeNotifier(&m_commands, &changes);

	if (!m_groupControllerCompPtr.IsValid()){
		return;
	}

	QDateTime currentTime;
	if (m_currentCommandTime.isValid()){
		currentTime = m_currentCommandTime;
	}
	else{
		currentTime = m_timeAxis.GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
	}

	bool enablePrev = false;
	bool enableNext = false;

	for (QByteArray id : m_groupControllerCompPtr->GetActiveGroupList()){
		IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

		const IEventItemController::EventMap* eventMap = groupPtr->GetEvents();

		if (eventMap->isEmpty()){
			continue;
		}

		IEventItemController::EventMap::const_iterator it;
		it = eventMap->lowerBound(currentTime);
		if (it != eventMap->begin()){
			enablePrev = true;
		}

		it = eventMap->upperBound(currentTime);
		if (it != eventMap->end()){
			enableNext = true;
		}
	}

	m_moveToFirstCommand.SetEnabled(enablePrev);
	m_moveToPreviousCommand.SetEnabled(enablePrev);
	m_moveToNextCommand.SetEnabled(enableNext);
	m_moveToLastCommand.SetEnabled(enableNext);
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


void CEventViewComp::UpdateSummaryInfoPanel()
{
	iwidgets::ClearLayout(m_summaryInfoPanelPtr->layout());

	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(m_summaryInfoPanelPtr->layout());
	if (layoutPtr != nullptr){
		if (m_groupControllerCompPtr.IsValid()){
			QByteArrayList groupIds = m_groupControllerCompPtr->GetActiveGroupList();
			for (QByteArray id : groupIds){
				IEventItemController* groupPtr = m_groupControllerCompPtr->GetGroup(id);

				QLabel* nameLabelPtr = new QLabel(groupPtr->GetGroupName());
				QFont groupNameFont;
				groupNameFont.setPixelSize(12);
				groupNameFont.setBold(true);
				nameLabelPtr->setStyleSheet("color: #88b8e3");
				nameLabelPtr->setFont(groupNameFont);
				layoutPtr->addWidget(nameLabelPtr);

				for (int i = 0; i < 5; i++){
					QHBoxLayout* categoryLayoutPtr = new QHBoxLayout();

					QIcon icon;

					switch (i){
					case istd::IInformationProvider::IC_NONE:
						icon = QIcon(":/Icons/StateUnknown");
						break;
					case istd::IInformationProvider::IC_INFO:
						icon = QIcon(":/Icons/StateOk");
						break;
					case istd::IInformationProvider::IC_WARNING:
						icon = QIcon(":/Icons/StateWarning");
						break;
					case istd::IInformationProvider::IC_ERROR:
						icon = QIcon(":/Icons/Error");
						break;
					case istd::IInformationProvider::IC_CRITICAL:
						icon = QIcon(":/Icons/StateInvalid");
						break;
					}

					QPixmap pixmap = icon.pixmap(16);
					QLabel* iconPtr = new QLabel();
					iconPtr->setPixmap(pixmap);
					categoryLayoutPtr->addWidget(iconPtr,1 );

					QLabel* countLabelPtr = new QLabel(QString("%1 ").arg(groupPtr->GetEventCount((istd::IInformationProvider::InformationCategory)i)) + tr("Events"));
					QFont countLabelFont;
					countLabelFont.setPixelSize(9);
					countLabelPtr->setStyleSheet("color: gray");
					countLabelPtr->setFont(countLabelFont);
					categoryLayoutPtr->addWidget(countLabelPtr, 1000);

					layoutPtr->addLayout(categoryLayoutPtr);
				}

				layoutPtr->addSpacing(10);
			}
		}

		layoutPtr->addStretch();
	}
}


bool CEventViewComp::UpdateMetaInfoPanel(const CEventItemBase* eventItem)
{
	iwidgets::ClearLayout(m_metaInfoPanelPtr->layout());

	if (eventItem == nullptr){
		return false;
	}

	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(m_metaInfoPanelPtr->layout());
	if (layoutPtr != nullptr){
		CEventItemBase::MetaInfo metaInfo = eventItem->GetMetaInfo();
		if (metaInfo.isEmpty()){
			return false;
		}

		for (CEventItemBase::MetaInfoItem metaInfoItem : metaInfo){
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


