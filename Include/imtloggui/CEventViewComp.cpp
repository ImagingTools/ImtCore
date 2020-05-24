#include <imtloggui/CEventViewComp.h>


// ACF includes
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>
#include <iser/IObject.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtloggui/CEventGraphicsView.h>


namespace imtloggui
{


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
	m_timeAxisPtr->setRect(0, 0, 10, 40);
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
			modelPtr->AttachObserver(this);
		}
	}
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(this, &CEventViewComp::AxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);
	m_viewPtr->setTimeAxis(nullptr);

	delete m_timeAxisPtr;
	delete m_viewPtr;
	delete m_scenePtr;

	BaseClass::OnGuiDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CEventViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	istd::CRange range = GetObservedObject()->GetNumericValueUnitInfo(0)->GetValueRange();
	m_viewPtr->OnMinimumVerticalScaleChanged(range.GetMinValue());
}


// reimplemented (icomp::CComponentBase)

void CEventViewComp::OnViewPortChanged()
{
	m_groupControllerCompPtr->OnViewPortChanged();
}


} // namespace imtloggui


