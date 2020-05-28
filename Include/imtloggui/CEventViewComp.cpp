#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtWidgets/QScrollBar>

// ACF includes
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>
#include <iser/IObject.h>
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
	imtlog::IMessageGroupInfoProvider::GroupInfo groupInfo;
	groupInfo.id = "General";
	groupInfo.name = QObject::tr("General");

	if (m_messageGroupInfoProviderCompPtr.IsValid()){
		groupInfo = m_messageGroupInfoProviderCompPtr->GetMessageGroupInfo(message.GetPtr());
	}

	if (m_groupControllerCompPtr.IsValid()){
		IEventItemController* eventItemControllerPtr = m_groupControllerCompPtr->AddGroup(groupInfo.id, groupInfo.name);

		if (eventItemControllerPtr != nullptr){
			m_timeAxisPtr->EnsureTimeRange(message->GetInformationTimeStamp());
			eventItemControllerPtr->AddEvent(message);
	
			Q_EMIT EmitAxisPositionChanged();
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_scenePtr = new QGraphicsScene(GetQtWidget());

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->SetColor(Qt::green);
	m_timeAxisPtr->setRect(0, 0, 0, 40);
	m_timeAxisPtr->setZValue(2);

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	m_viewPtr->setScene(m_scenePtr);
	m_viewPtr->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
	m_viewPtr->SetTimeAxis(m_timeAxisPtr);
	GetQtWidget()->layout()->addWidget(m_viewPtr);

	m_scenePtr->addItem(m_timeAxisPtr);

	connect(this, &CEventViewComp::EmitAxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->SetScene(m_scenePtr);
		m_groupControllerCompPtr->SetView(m_viewPtr);
		m_groupControllerCompPtr->SetTimeAxis(m_timeAxisPtr);
		m_groupControllerCompPtr->CreateGraphicsItem();
		connect(m_viewPtr, &CEventGraphicsView::EmitViewPortChanged, this, &CEventViewComp::OnViewPortChanged);

		if (m_messageGroupInfoProviderCompPtr.IsValid()){
			imtlog::IMessageGroupInfoProvider::GroupInfos groupInfos = m_messageGroupInfoProviderCompPtr->GetMessageGroupInfos();
			m_groupControllerCompPtr->AddGroups(groupInfos);
		}

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_scaleConstraintsCompPtr.GetPtr());
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_scaleConstraintsObserver);
		}
	}
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(this, &CEventViewComp::EmitAxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);
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


// public slots

void CEventViewComp::OnViewPortChanged()
{
	m_groupControllerCompPtr->OnViewPortChanged();

	m_viewPtr->viewportTransform();
}


// private slots

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

	QDateTime currentTime = m_timeAxisPtr->GetTimeFromScenePosition(GetSceneVisibleRect().center().x());

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

	QDateTime currentTime = m_timeAxisPtr->GetTimeFromScenePosition(GetSceneVisibleRect().center().x());

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


void CEventViewComp::UpdateVerticalRangeScale(const istd::CRange & range)
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
		m_viewPtr->ensureVisible(rect, 0, 0);
	}
	else{
		double delta = qMin(currentTime - beginTime, endTime - currentTime);	

		m_viewPtr->scale(GetSceneVisibleRect().width() / (2 * delta), 1);

		QRectF rect = GetSceneVisibleRect();
		double center = rect.center().x();
		double newCenter = m_timeAxisPtr->GetScenePositionFromTime(time);

		rect.setWidth(2 * delta);
		rect.translate(newCenter - center, 0);
		m_viewPtr->ensureVisible(rect, 0, 0);
	}
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


