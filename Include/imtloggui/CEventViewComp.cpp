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
#include <imtloggui/CEventGroup.h>
#include <imtloggui/ÑLoginEventItem.h>


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
	imtbase::IMessageGroupInfoProvider::GroupInfo groupInfo;
	if (m_messageGroupInfoProviderCompPtr.IsValid()){
		groupInfo = m_messageGroupInfoProviderCompPtr->GetMessageGroupInfo(message.GetPtr());
	}

	CEventGroup* groupPtr = m_groupManagerPtr->GetGroup(groupInfo.groupId);
	if (groupPtr == nullptr){
		groupPtr = m_groupManagerPtr->GetGroup(QByteArray());
	}

	if (groupPtr != nullptr){
		if (m_timeAxisPtr != nullptr){
			QDateTime eventTime = message->GetInformationTimeStamp();

			m_timeAxisPtr->EnsureTimeRange(eventTime);

			CLoginEventItem* eventPtr = new CLoginEventItem(message);
			eventPtr->SetIcons(QIcon(":/Icons/Lock"), QIcon(":/Icons/Unlock"));
			eventPtr->SetIconSize(QSize(40, 40));

			QPointF origin(m_timeAxisPtr->GetScenePositionFromTime(message->GetInformationTimeStamp()), -150);
			eventPtr->setPos(origin);
			eventPtr->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

			groupPtr->AddEvent(eventPtr);

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

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	m_viewPtr->setScene(m_scenePtr);
	m_viewPtr->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
	m_viewPtr->setTimeAxis(m_timeAxisPtr);
	GetQtWidget()->layout()->addWidget(m_viewPtr);
	
	m_groupManagerPtr = new CEventGroupManager();

	m_scenePtr->addItem(m_timeAxisPtr);
	m_scenePtr->addItem(m_groupManagerPtr);

	CEventGroup* generalGroupPtr = new CEventGroup();
	generalGroupPtr->SetGroupId(QByteArray());
	generalGroupPtr->SetTimeAxis(m_timeAxisPtr);
	m_groupManagerPtr->AddGroup(generalGroupPtr);

	connect(this, &CEventViewComp::AxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);
}


void CEventViewComp::OnGuiDestroyed()
{
	disconnect(this, &CEventViewComp::AxisPositionChanged, m_viewPtr, &CEventGraphicsView::OnAxisPositionChanged);
	m_viewPtr->setTimeAxis(nullptr);

	delete m_groupManagerPtr;
	delete m_viewPtr;
	delete m_scenePtr;

	BaseClass::OnGuiDestroyed();
}


void CEventViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// reimplemented (icomp::CComponentBase)

void CEventViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CEventViewComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtloggui


