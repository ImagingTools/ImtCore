#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QDebug>
#include <QGraphicsLineItem>

// ACF includes
#include <iser/IObject.h>
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CEventItemBase.h>
#include <imtloggui/CEventGroup.h>


namespace imtloggui
{


// public methods

CEventViewComp::CEventViewComp()
{
	// General group
	//imtbase::IMessageGroupInfoProvider::GroupInfo info;
	//info.name == tr("General");
	//m_eventGroupManager.AddGroup(new CEventGroup(&m_eventGroupManager), info);
}


// reimplemented (ilog::IMessageConsumer)

bool CEventViewComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CEventViewComp::AddMessage(const IMessageConsumer::MessagePtr& messagePtr)
{
	imtbase::IMessageGroupInfoProvider::GroupInfo groupInfo;
	if (m_messageGroupInfoProviderCompPtr.IsValid()) {
		groupInfo = m_messageGroupInfoProviderCompPtr->GetMessageGroupInfo(messagePtr.GetPtr());
	}

	CEventItemBase* eventPtr = new CEventItemBase();
	eventPtr->setPos(m_timeAxisPtr->GetScenePosition(messagePtr->GetInformationTimeStamp()), -150);
	eventPtr->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	eventPtr->SetMessage(messagePtr);

	if (messagePtr->GetInformationCategory() == istd::IInformationProvider::IC_NONE){
		eventPtr->SetColor(Qt::darkGray);
	}
	else if (messagePtr->GetInformationCategory() == istd::IInformationProvider::IC_INFO){
		eventPtr->SetColor(Qt::green);
	}
	else if (messagePtr->GetInformationCategory() == istd::IInformationProvider::IC_WARNING){
		eventPtr->SetColor(Qt::yellow);
	}
	else if (messagePtr->GetInformationCategory() == istd::IInformationProvider::IC_ERROR){
		eventPtr->SetColor(Qt::red);
	}
	else if (messagePtr->GetInformationCategory() == istd::IInformationProvider::IC_CRITICAL){
		eventPtr->SetColor(Qt::black);
	}

	CEventGroup* groupPtr = m_groupManagerPtr->GetGroup(groupInfo.groupId);
	if (groupPtr == nullptr){
		groupPtr = m_groupManagerPtr->GetGroup(QByteArray());
	}

	if (groupPtr != nullptr){
		groupPtr->AddEvent(eventPtr);
	}
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CEventViewComp::OnRestoreSettings(const QSettings& settings)
{
}


void CEventViewComp::OnSaveSettings(QSettings& settings) const
{
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_scenePtr = new QGraphicsScene(GetQtWidget());

	QDateTime begin = QDateTime::currentDateTime();
	QDateTime end;
	end.setSecsSinceEpoch(begin.toSecsSinceEpoch() + 3600);

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->setColor(Qt::green);
	m_timeAxisPtr->setTimeRange(begin, end);
	m_timeAxisPtr->setMinorTickCount(12);

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

	//m_viewPtr->setSceneRect(m_scenePtr->sceneRect());
}


void CEventViewComp::OnGuiDestroyed()
{
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


