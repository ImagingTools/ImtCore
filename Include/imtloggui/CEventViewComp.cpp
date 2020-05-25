#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QtCore/QDebug>

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

	qDebug() << connect(&m_moveToFirstCommand, &QAction::toggled, this, &CEventViewComp::OnMoveToFirstToggled);
	qDebug() << connect(&m_moveToPreviousCommand, &QAction::toggled, this, &CEventViewComp::OnMoveToPreviousData);
	qDebug() << connect(&m_moveToNextCommand, &QAction::toggled, this, &CEventViewComp::OnMoveToNextToggled);
	qDebug() << connect(&m_moveToLastCommand, &QAction::toggled, this, &CEventViewComp::OnMoveToLastToggled);
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
	
			Q_EMIT AxisPositionChanged();
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
	m_viewPtr->setTimeAxis(m_timeAxisPtr);
	GetQtWidget()->layout()->addWidget(m_viewPtr);

	m_scenePtr->addItem(m_timeAxisPtr);

	connect(this, &CEventViewComp::AxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);

	if (m_groupControllerCompPtr.IsValid()){
		m_groupControllerCompPtr->SetScene(m_scenePtr);
		m_groupControllerCompPtr->SetView(m_viewPtr);
		m_groupControllerCompPtr->SetTimeAxis(m_timeAxisPtr);
		m_groupControllerCompPtr->CreateGraphicsItem();
		connect(m_viewPtr, &CEventGraphicsView::ViewPortChanged, this, &CEventViewComp::OnViewPortChanged);

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
	disconnect(this, &CEventViewComp::AxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);
	m_viewPtr->setTimeAxis(nullptr);

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
}


// private slots

void CEventViewComp::OnMoveToFirstToggled()
{
	qDebug() << "First";
}


void CEventViewComp::OnMoveToPreviousData()
{
	qDebug() << "Previous";
}


void CEventViewComp::OnMoveToNextToggled()
{
	qDebug() << "Next";
}


void CEventViewComp::OnMoveToLastToggled()
{
	qDebug() << "Last";
}


// private methods

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


