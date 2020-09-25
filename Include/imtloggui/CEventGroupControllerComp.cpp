#include <imtloggui/CEventGroupControllerComp.h>


// STD includes
#include <float.h>

// Qt includes
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QScrollBar>

// Acf includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


CEventGroupControllerComp::CEventGroupControllerComp()
	:m_scenePtr(nullptr),
	m_viewPtr(nullptr),
	m_timeAxisPtr(nullptr),
	m_itemGroupPtr(nullptr),
	m_minimumVerticalScale(1)
{
}


// reimplemented (imtloggui::IEventGroupController)

void CEventGroupControllerComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


IEventItemController* CEventGroupControllerComp::AddGroup(const QByteArray& groupId)
{
	//if (m_groups.contains(groupId)){
	//	return dynamic_cast<IEventItemController*>(m_groups[groupId]);
	//}

	//IEventItemController* eventItemController = nullptr;

	//for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
	//	if (m_groupRefsCompPtr[i]->GetGroupId() == groupId){
	//		eventItemController = m_groupRefsCompPtr[i];
	//		break;
	//	}
	//}

	//if (eventItemController == nullptr){
	//	if (m_generalGroupRefCompPtr.IsValid()){
	//		if (m_generalGroupRefCompPtr->GetGroupId() == groupId){
	//			eventItemController = m_generalGroupRefCompPtr.GetPtr();
	//		}
	//	}
	//}

	//if (eventItemController != nullptr){
	//	eventItemController->SetTimeAxis(m_timeAxisPtr);

	//	int totalHeight = 0;
	//	for (IEventItemController* item : m_groups){
	//		totalHeight += item->GetGroupHeight();
	//	}

	//	m_groups[groupId] = eventItemController;

	//	QRectF sceneRect = m_viewPtr->GetSceneRect();

	//	totalHeight = 0;
	//	for (IEventItemController* item : m_groups){
	//		totalHeight += item->GetGroupHeight();
	//	}

	//	sceneRect.setBottom(0);
	//	sceneRect.setTop(-totalHeight);

	//	m_viewPtr->SetSceneRect(sceneRect);

	//	m_eventStatisticsProvider.InsertGroup(
	//				groupId,
	//				eventItemController->GetGroupName(),
	//				m_groups.count());
	//}

	return nullptr; //eventItemController;
}


bool CEventGroupControllerComp::RemoveGroup(const QByteArray& groupId)
{
	if (m_groups.contains(groupId)){
		delete m_groups[groupId];
		m_groups.remove(groupId);

		m_eventStatisticsProvider.RemoveGroup(groupId);

		return true;
	}

	return false;
}


QByteArrayList CEventGroupControllerComp::GetAvailableGroupList() const
{
	QByteArrayList result;

	for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
		IEventItemController* eventItemControllerPtr = dynamic_cast<IEventItemController*>(m_groupRefsCompPtr[i]);
		if (eventItemControllerPtr != nullptr){
			result.append(m_groupRefsCompPtr[i]->GetGroupId());
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		result.append(m_generalGroupRefCompPtr->GetGroupId());
	}

	return result;
}


QByteArrayList CEventGroupControllerComp::GetActiveGroupList() const
{
	return m_groups.keys();
}


IGroup* CEventGroupControllerComp::GetGroup(const QByteArray& groupId) const
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId];
	}

	return nullptr;
}


IEventItem* CEventGroupControllerComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	//for (QByteArray groupId : m_messageIdMap.keys()){
	//	if (m_messageIdMap[groupId].contains(messagePtr->GetInformationId())){
	//		IEventItemController* groupPtr = GetGroup(groupId);
	//		if (groupPtr != nullptr){
	//			if (groupPtr->AddEvent(messagePtr)){
	//				m_eventStatisticsProvider.IncrementCategoryCounter(groupId, messagePtr->GetInformationCategory());

	//				return nullptr;
	//			}

	//			break;
	//		}
	//	}
	//}

	//if (m_generalGroupRefCompPtr.IsValid()){
	//	IEventItemController* groupPtr = GetGroup(m_generalGroupRefCompPtr->GetGroupId());
	//	if (groupPtr != nullptr){
	//		if (groupPtr->AddEvent(messagePtr)){
	//			m_eventStatisticsProvider.IncrementCategoryCounter(m_generalGroupRefCompPtr->GetGroupId(), messagePtr->GetInformationCategory());
	//		}

	//		return nullptr;
	//	}
	//}

	return nullptr;
}


// reimplemented (icomp::CComponentBase)

void CEventGroupControllerComp::OnComponentCreated()
{
	for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
		IEventItemController* eventItemControllerPtr = dynamic_cast<IEventItemController*>(m_groupRefsCompPtr[i]);
		if (eventItemControllerPtr != nullptr){
			m_messageIdMap[eventItemControllerPtr->GetGroupId()] = eventItemControllerPtr->GetSupportedMessageIds();
		}
	}
}


// public methods of the embedded class EventStatisticsProvider

bool CEventGroupControllerComp::EventStatisticsProvider::InsertGroup(const QByteArray& groupId, const QString& groupName, int position)
{
	int index = GetGroupIndex(groupId);

	if (index < 0){
		istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_GROUPS, IEventStatisticsProvider::CF_COUNTERS);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		GroupListItem group;
		group.id = groupId;
		group.name = groupName;
		group.enabled = true;
		m_groups.insert(position, group);

		return true;
	}

	return false;
}


bool CEventGroupControllerComp::EventStatisticsProvider::RemoveGroup(const QByteArray& groupId)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_GROUPS);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		m_groups.removeAt(index);

		return true;
	}

	return false;
}


bool CEventGroupControllerComp::EventStatisticsProvider::SetGroupName(const QByteArray& groupId, const QString& groupName)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		if (m_groups[index].name != groupName){
			istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_GROUPS);
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_groups[index].name = groupName;
			
			return true;
		}
	}

	return false;
}


bool CEventGroupControllerComp::EventStatisticsProvider::SetGroupPosition(const QByteArray& groupId, int position)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		if (index != position){
			istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_GROUPS);
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_groups.move(index, position);

			return true;
		}
	}

	return false;
}

bool CEventGroupControllerComp::EventStatisticsProvider::SetGroupEnabled(const QByteArray& groupId, bool enabled)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		if (m_groups[index].enabled != enabled){
			istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_GROUPS);
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_groups[index].enabled = enabled;

			return true;
		}
	}

	return false;
}


bool CEventGroupControllerComp::EventStatisticsProvider::IncrementCategoryCounter(
	const QByteArray& groupId,
	istd::IInformationProvider::InformationCategory category)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_COUNTERS);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		m_groups[index].counters[category]++;

		return true;
	}

	return false;
}



bool CEventGroupControllerComp::EventStatisticsProvider::SetCategoryCounter(
			const QByteArray& groupId,
			istd::IInformationProvider::InformationCategory category,
			qint64 counter)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		istd::IChangeable::ChangeSet changeSet(IEventStatisticsProvider::CF_COUNTERS);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		m_groups[index].counters[category] = counter;

		return true;
	}

	return false;
}


// reimplemented (imtloggui::IEventGroupStatisticsProvider)

qint64 CEventGroupControllerComp::EventStatisticsProvider::GetCategoryCounter(const QByteArray& groupId, istd::IInformationProvider::InformationCategory category)
{
	int index = GetGroupIndex(groupId);

	if (index >= 0){
		return m_groups[index].counters[category];
	}

	return -1;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CEventGroupControllerComp::EventStatisticsProvider::GetElementIds() const
{
	Ids retVal;

	for (int i = 0; i < m_groups.count(); i++){
		retVal.append(m_groups[i].id);
	}

	return retVal;
}


QVariant CEventGroupControllerComp::EventStatisticsProvider::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QVariant retVal;

	int index = GetGroupIndex(elementId);
	if (index < 0){
		return retVal;
	}

	switch (infoType){
	case EIT_NAME:
		retVal = m_groups[index].name;
		break;
	case EIT_ENABLED:
		retVal = m_groups[index].enabled;
		break;
	}

	return retVal;
}


// private methods of the embedded class EventStatisticsProvider

int CEventGroupControllerComp::EventStatisticsProvider::GetGroupIndex(const QByteArray& groupId) const
{
	for (int i = 0; i < m_groups.count(); i++){
		if (m_groups[i].id == groupId){
			return i;
		}
	}

	return -1;
}


} // namespace imtloggui


