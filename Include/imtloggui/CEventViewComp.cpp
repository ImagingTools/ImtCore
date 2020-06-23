#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QtCore/QDebug>
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
	:m_scenePtr(nullptr),
	m_viewPtr(nullptr),
	m_timeAxisPtr(nullptr),
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
		m_timeAxisPtr->EnsureTimeRange(message->GetInformationTimeStamp());
		m_groupControllerCompPtr->AddEvent(message);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_scenePtr = new QGraphicsScene(GetQtWidget());
	connect(m_scenePtr, &QGraphicsScene::selectionChanged, this, &CEventViewComp::OnSelectionChanged);
	connect(this, &CEventViewComp::UpdateSceneRect, this, &CEventViewComp::OnUpdateSceneRect);

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->SetColor(Qt::green);
	m_timeAxisPtr->setRect(0, 0, 100, 40);
	m_timeAxisPtr->setZValue(101);

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	m_viewPtr->setScene(m_scenePtr);
	m_viewPtr->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_viewPtr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_viewPtr->setMouseTracking(false);
	m_viewPtr->SetTimeAxis(m_timeAxisPtr);
	m_viewPtr->SetMargins(QMargins(70, 0, 70, 40));

	QHBoxLayout* layoutPtr = dynamic_cast<QHBoxLayout*>(GetQtWidget()->layout());//->addWidget(m_viewPtr);
	layoutPtr->insertWidget(0, m_viewPtr);
	layoutPtr->setStretch(0, 90);
	layoutPtr->setStretch(1, 10);
	MetaInfoPanel->hide();
	if (MetaInfoPanel->layout() == nullptr){
		MetaInfoPanel->setLayout(new QGridLayout());
	}

	m_scenePtr->addItem(m_timeAxisPtr);

	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisPosChanged, this, &CEventViewComp::OnAxisPosChanged);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CEventViewComp::OnAxisBeginTimeChanged);
	connect(m_timeAxisPtr, &CTimeAxis::EmitAxisEndTimeChanged, this, &CEventViewComp::OnAxisEndTimeChanged);
	connect(m_viewPtr, &CEventGraphicsView::EmitViewPortChanged, this, &CEventViewComp::OnViewPortChanged);

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->SetScene(m_scenePtr);
		m_groupControllerCompPtr->SetView(m_viewPtr);
		m_groupControllerCompPtr->SetTimeAxis(m_timeAxisPtr);
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

	for (const ilog::IMessageConsumer::MessagePtr& message : m_messageList)
	{	
		AddMessage(message);
	}
	m_messageList.clear();

	m_timeAxisPtr->EnsureTimeRange(QDateTime::currentDateTime());
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(m_timeAxisPtr, &CTimeAxis::EmitAxisPosChanged, this, &CEventViewComp::OnAxisPosChanged);
	disconnect(m_timeAxisPtr, &CTimeAxis::EmitAxisBeginTimeChanged, this, &CEventViewComp::OnAxisBeginTimeChanged);
	disconnect(m_timeAxisPtr, &CTimeAxis::EmitAxisEndTimeChanged, this, &CEventViewComp::OnAxisEndTimeChanged);
	disconnect(m_viewPtr, &CEventGraphicsView::EmitViewPortChanged, this, &CEventViewComp::OnViewPortChanged);

	m_viewPtr->SetTimeAxis(nullptr);

	if (m_timeAxisPtr != nullptr){
		delete m_timeAxisPtr;
	}

	if (m_viewPtr != nullptr){
		delete m_viewPtr;
	}

	if (m_scenePtr != nullptr){
		delete m_scenePtr;
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
	if (m_timeAxisPtr != nullptr){
		QRectF visibleRect = m_viewPtr->GetSceneVisibleRect();
		
		m_timeAxisPtr->setPos(0, visibleRect.bottom() - m_timeAxisPtr->rect().height() / m_viewPtr->GetScaleY());
		//m_timeAxisPtr->OnViewPortChanged();

		//QRectF rect = m_viewPtr->GetSceneRect();

		//if (m_timeAxisPtr != nullptr){
		//	rect.setLeft(m_timeAxisPtr->rect().left() - 100 / m_viewPtr->GetScaleX());
		//	rect.setRight(m_timeAxisPtr->rect().right() + 100 / m_viewPtr->GetScaleX());
		//	m_viewPtr->SetSceneRect(rect);
		//}
	}

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnViewPortChanged();
	}

	if (userAction){
		m_currentCommandTime = QDateTime();
	}
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

		rect.setLeft(m_timeAxisPtr->rect().left());
		rect.setRight(m_timeAxisPtr->rect().right());
		m_viewPtr->SetSceneRect(rect);
		rect.adjust(0, 0, 0, 40);
		m_viewPtr->SetViewRect(rect);
		m_currentCommandTime = QDateTime();
	}
	 
	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->OnAxisBeginTimeChanged(oldTime, newTime);
	}

	if (m_viewPtr != nullptr){
		QRectF rect = m_viewPtr->GetSceneRect();
		rect.setLeft(m_timeAxisPtr->rect().left());
		rect.setRight(m_timeAxisPtr->rect().right());
		m_viewPtr->SetSceneRect(rect);

		double shift = (oldTime.toMSecsSinceEpoch() - newTime.toMSecsSinceEpoch()) / 1000.;
		QRectF viewRect = m_viewPtr->GetViewRect().translated(shift, 0);
		m_viewPtr->SetViewRect(viewRect);
	}
}


void CEventViewComp::OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
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
}


void CEventViewComp::OnMoveToPreviousCommand()
{
	if (!m_groupControllerCompPtr.IsValid() || m_timeAxisPtr == nullptr){
		return;
	}

	QDateTime currentTime;
	if (m_currentCommandTime.isValid()){
		currentTime = m_currentCommandTime;
	}
	else{
		currentTime = m_timeAxisPtr->GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
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
}


void CEventViewComp::OnMoveToNextCommand()
{
	if (!m_groupControllerCompPtr.IsValid() || m_timeAxisPtr == nullptr){
		return;
	}

	QDateTime currentTime;
	if (m_currentCommandTime.isValid()){
		currentTime = m_currentCommandTime;
	}
	else{
		currentTime = m_timeAxisPtr->GetTimeFromScenePosition(GetSceneVisibleRect().center().x());
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
}


void CEventViewComp::OnSelectionChanged()
{
	QList<QGraphicsItem*> items = m_scenePtr->selectedItems();
	MetaInfoPanel->hide();
	if (!items.isEmpty()){
		CEventItemBase* itemPtr = dynamic_cast<CEventItemBase*>(items[0]);
		if (itemPtr != nullptr){
			if (UpdateMetaInfoPanel(itemPtr)){
				MetaInfoPanel->show();
			}
		}
	}
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
		return m_viewPtr->viewportTransform().m11();
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
	static istd::IChangeable::ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);

	istd::CChangeNotifier changeNotifier(&m_commands, &changes);
}


void CEventViewComp::MoveToTime(const QDateTime& time)
{
	double beginTime = m_timeAxisPtr->GetBeginTime().toMSecsSinceEpoch() / 1000.0;
	double endTime = m_timeAxisPtr->GetEndTime().toMSecsSinceEpoch() / 1000.0;
	double currentTime = time.toMSecsSinceEpoch() / 1000.0;

	if (currentTime <  beginTime || currentTime > endTime){
		return;
	}

	double visibleTime = m_viewPtr->viewport()->rect().width() / GetCurrentScaleX();

	if ((currentTime - beginTime > visibleTime / 2) && (endTime - currentTime > visibleTime / 2)){
		QRectF rect = GetSceneVisibleRect();
		double center = rect.center().x();
		double newCenter = m_timeAxisPtr->GetScenePositionFromTime(time);

		rect.translate(newCenter - center, 0);
		m_viewPtr->SetViewRect(rect);
	}
	else{
		double delta = qMin(currentTime - beginTime, endTime - currentTime);	

		m_viewPtr->scale(GetSceneVisibleRect().width() / (2 * delta), 1);

		QRectF rect = GetSceneVisibleRect();
		double center = rect.center().x();
		double newCenter = m_timeAxisPtr->GetScenePositionFromTime(time);

		rect.setWidth(2 * delta);
		rect.translate(newCenter - center, 0);
		m_viewPtr->SetViewRect(rect);
	}
}


bool CEventViewComp::UpdateMetaInfoPanel(const CEventItemBase* eventItem)
{
	iwidgets::ClearLayout(MetaInfoPanel->layout());

	if (eventItem == nullptr){
		return false;
	}

	QGridLayout* layoutPtr = dynamic_cast<QGridLayout*>(MetaInfoPanel->layout());
	if (layoutPtr != nullptr){
		CEventItemBase::MetaInfo metaInfo = eventItem->GetMetaInfo();
		if (metaInfo.isEmpty()){
			return false;
		}

		for (CEventItemBase::MetaInfoItem metaInfoItem : metaInfo){
			QLabel* labelKeyPtr = new QLabel(metaInfoItem.key, MetaInfoPanel);
			labelKeyPtr->setStyleSheet("font-size: 12px; font: bold; color: #88b8e3");

			QLabel* labelValuePtr = new QLabel(metaInfoItem.value, MetaInfoPanel);
			labelValuePtr->setStyleSheet("font-size: 9px; color: gray");
			labelValuePtr->setWordWrap(true);

			layoutPtr->addWidget(labelKeyPtr, layoutPtr->rowCount(), 0, 1, 1);
			layoutPtr->addWidget(labelValuePtr, layoutPtr->rowCount(), 0, 1, 1);

			QSpacerItem* delimeter = new QSpacerItem(10, 5);
			layoutPtr->addItem(delimeter, layoutPtr->rowCount(), 0, 1, 1);
		}

		layoutPtr->setColumnMinimumWidth(0, 10);
		layoutPtr->setColumnStretch(0, 1);
		layoutPtr->setColumnStretch(1, 100);
		layoutPtr->setColumnStretch(1, 100);
		layoutPtr->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), layoutPtr->rowCount(), 0);

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


