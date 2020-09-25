#include <imtloggui/CSingleLayerGroupComp.h>


//ACF includes
#include <ilog/CMessageContainer.h>
#include <imod/TModelWrap.h>

// Qt includes
#include <QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

// ImtCore includes
#include <imtloggui/CClusterItem.h>


namespace imtloggui
{


CSingleLayerGroupComp::CSingleLayerGroupComp()
	:m_timeAxisPtr(nullptr)
{
	m_groupName = "Unnamed";

	m_representationsInfo.InsertItem("default", QObject::tr("default"), QObject::tr("default"));
}


// reimplemented (imtloggui::IEventItemController)

void CSingleLayerGroupComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	if (m_timeAxisPtr == nullptr){
		m_timeAxisPtr = timeAxisPtr;
	}
}


QByteArray CSingleLayerGroupComp::GetGroupId() const
{
	if (m_groupIdAttrPtr.IsValid()){
		return *m_groupIdAttrPtr;
	}

	return QByteArray();
}


int CSingleLayerGroupComp::GetGroupHeight() const
{
	if (m_groupHeightAttrPtr.IsValid()){
		return *m_groupHeightAttrPtr;
	}

	return 150;
}


QString CSingleLayerGroupComp::GetGroupName() const
{
	return m_groupName;
}


void CSingleLayerGroupComp::SetGroupName(const QString& name)
{
	m_groupName = name;
}


QVector<int> CSingleLayerGroupComp::GetSupportedMessageIds() const
{
	QVector<int> ids;

	if (m_eventItemFactoryCompPtr.IsValid()){
		ids.append(m_eventItemFactoryCompPtr->GetSupportedMessageIds());
	}

	return ids;
}


const IEventItemController::EventMap* CSingleLayerGroupComp::GetEvents() const
{
	return &m_events;
}


int CSingleLayerGroupComp::GetEventCount(istd::IInformationProvider::InformationCategory category)
{
	return m_eventCount[category];
}


bool CSingleLayerGroupComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	//if (GetSupportedMessageIds().contains(messagePtr->GetInformationId())){
		//m_events.insert(messagePtr->GetInformationTimeStamp(), messagePtr);
		//m_eventCount[messagePtr->GetInformationCategory()]++;

		//return true;
	//}

	return false;
}


void CSingleLayerGroupComp::ClearEvents()
{
	m_events.clear();
}


// reimplemented (icomp::CComponentBase)

void CSingleLayerGroupComp::OnComponentCreated()
{
	if (m_groupNameAttrPtr.IsValid()){
		SetGroupName(*m_groupNameAttrPtr);
	}
}


} // namespace imtloggui


