#include <imtloggui/CEventGroupControllerComp.h>


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

void CEventGroupControllerComp::SetScene(QGraphicsScene* scenePtr)
{
	m_scenePtr = scenePtr;
}


void CEventGroupControllerComp::SetView(CEventGraphicsView* viewPtr)
{
	m_viewPtr = viewPtr;
}


void CEventGroupControllerComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


bool CEventGroupControllerComp::CreateGraphicsItem()
{
	if (m_itemGroupPtr == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_itemGroupPtr = new CGraphicsItemGroup();

		m_scenePtr->addItem(m_itemGroupPtr);
	}

	return false;
}


bool CEventGroupControllerComp::DestroyGraphicsItem()
{
	if (m_itemGroupPtr != nullptr && m_scenePtr != nullptr){
	

		for (IEventItemController* itemPtr : m_groups){
			RemoveGroup(itemPtr->GetGroupId());
		}
		m_groups.clear();

		m_scenePtr->removeItem(m_itemGroupPtr);
		delete m_itemGroupPtr;
		m_itemGroupPtr = nullptr;

		return true;
	}

	return false;
}


QGraphicsItem* CEventGroupControllerComp::GetGraphicsItem()
{
	return m_itemGroupPtr;
}


IEventItemController* CEventGroupControllerComp::AddGroup(const QByteArray& groupId)
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId];
	}

	IEventItemController* eventItemController = nullptr;

	for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
		if (m_groupRefsCompPtr[i]->GetGroupId() == groupId){
			eventItemController = m_groupRefsCompPtr[i];
			break;
		}
	}

	if (eventItemController == nullptr){
		if (m_generalGroupRefCompPtr.IsValid()){
			if (m_generalGroupRefCompPtr->GetGroupId() == groupId){
				eventItemController = m_generalGroupRefCompPtr.GetPtr();
			}
		}
	}

	if (eventItemController != nullptr){
		eventItemController->SetScene(m_scenePtr);
		eventItemController->SetTimeAxis(m_timeAxisPtr);
		eventItemController->CreateGraphicsItem();

		int totalHeight = 0;
		for (IEventItemController* item : m_groups){
			totalHeight += item->GetGroupHeight();
		}

		eventItemController->GetGraphicsItem()->setPos(0, -totalHeight);
		eventItemController->GetGraphicsItem()->setParentItem(m_itemGroupPtr);

		m_groups[groupId] = eventItemController;

		QRectF sceneRect = m_viewPtr->GetSceneRect();

		totalHeight = 0;
		for (IEventItemController* item : m_groups){
			totalHeight += item->GetGroupHeight();
		}

		sceneRect.setBottom(0);
		sceneRect.setTop(-totalHeight);

		m_viewPtr->SetSceneRect(sceneRect);

		m_eventStatisticsProvider.InsertGroup(
					groupId,
					eventItemController->GetGroupName(),
					m_groups.count());
	}

	return eventItemController;
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


IEventItemController* CEventGroupControllerComp::GetGroup(const QByteArray& groupId) const
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId];
	}

	return nullptr;
}


IEventItem* CEventGroupControllerComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	for (QByteArray groupId : m_messageIdMap.keys()){
		if (m_messageIdMap[groupId].contains(messagePtr->GetInformationId())){
			IEventItemController* groupPtr = GetGroup(groupId);
			if (groupPtr != nullptr){
				IEventItem* eventItemPtr = groupPtr->AddEvent(messagePtr);
				if (eventItemPtr != nullptr){
					m_eventStatisticsProvider.IncrementCategoryCounter(groupId, messagePtr->GetInformationCategory());

					return eventItemPtr;
				}

				break;
			}
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		IEventItemController* groupPtr = GetGroup(m_generalGroupRefCompPtr->GetGroupId());
		if (groupPtr != nullptr){
			IEventItem* eventItemPtr = groupPtr->AddEvent(messagePtr);
			if (eventItemPtr != nullptr){
				m_eventStatisticsProvider.IncrementCategoryCounter(m_generalGroupRefCompPtr->GetGroupId(), messagePtr->GetInformationCategory());
			}

			return eventItemPtr;
		}
	}

	return nullptr;
}


void CEventGroupControllerComp::OnAxisPosChanged(const QPointF& oldPos, const QPointF& newPos)
{
	QDateTime beginTime = m_timeAxisPtr->GetBeginTime();
	QPointF pos = m_itemGroupPtr->pos();
	pos.setX(m_timeAxisPtr->GetScenePositionFromTime(beginTime));

	m_itemGroupPtr->setPos(pos);
}


void CEventGroupControllerComp::OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->OnAxisBeginTimeChanged(oldTime, newTime);
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		m_generalGroupRefCompPtr->OnAxisBeginTimeChanged(oldTime, newTime);
	}
}


void CEventGroupControllerComp::OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime)
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->OnAxisEndTimeChanged(oldTime, newTime);
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		m_generalGroupRefCompPtr->OnAxisEndTimeChanged(oldTime, newTime);
	}
}


void CEventGroupControllerComp::OnViewPortChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->OnViewPortChanged();
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		m_generalGroupRefCompPtr->OnViewPortChanged();
	}

	int totalHeight = 0;
	for (IEventItemController* item : m_groups){
		totalHeight += item->GetGroupHeight();
	}

	QMargins viewMargins = m_viewPtr->GetMargins();
	double minimumVerticalScale = m_viewPtr->viewport()->rect().height() / (m_viewPtr->GetSceneRect().height() + viewMargins.bottom() + viewMargins.top());
	if (minimumVerticalScale < 1){
		minimumVerticalScale = 1;
	}
	
	if (totalHeight != 0 && m_minimumVerticalScale != minimumVerticalScale){
		m_minimumVerticalScale = minimumVerticalScale;
		m_verticalScaleConstraints.SetMinScale(minimumVerticalScale);
	}
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


// public methods of the embedded class VerticalScaleConstraints

CEventGroupControllerComp::VerticalScaleConstraints::VerticalScaleConstraints()
	:m_parentPtr(nullptr),
	m_minVerticalScale(1)
{
}


void CEventGroupControllerComp::VerticalScaleConstraints::SetParent(CEventGroupControllerComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


void CEventGroupControllerComp::VerticalScaleConstraints::SetMinScale(double scale)
{
	istd::CChangeNotifier changeNotifier(this);
	m_minVerticalScale = scale;
}


bool CEventGroupControllerComp::VerticalScaleConstraints::IsEqual(const IChangeable& object) const
{
	return false;
}


// reimplemented (imeas::INumericConstraints)

const iprm::IOptionsList& CEventGroupControllerComp::VerticalScaleConstraints::GetValueListInfo() const
{
	return *this;
}


const imath::IUnitInfo* CEventGroupControllerComp::VerticalScaleConstraints::GetNumericValueUnitInfo(int index) const
{
	if (index == 0){
		return this;
	}

	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CEventGroupControllerComp::VerticalScaleConstraints::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CEventGroupControllerComp::VerticalScaleConstraints::GetOptionsCount() const
{
	return 1;
}


QString CEventGroupControllerComp::VerticalScaleConstraints::GetOptionName(int index) const
{
	if (index == 0){
		return QObject::tr("MinimumVerticalScale");
	}

	return QString();
}


QString CEventGroupControllerComp::VerticalScaleConstraints::GetOptionDescription(int index) const
{
	return QString();
}


QByteArray CEventGroupControllerComp::VerticalScaleConstraints::GetOptionId(int index) const
{
	if (index == 0){
		return "MinimumVerticalScale";
	}

	return QByteArray();
}


bool CEventGroupControllerComp::VerticalScaleConstraints::IsOptionEnabled(int index) const
{
	return (index == 0);
}


// reimplemented (imath::IUnitInfo)

int CEventGroupControllerComp::VerticalScaleConstraints::GetUnitType() const
{
	return UT_RELATIVE;
}


QString CEventGroupControllerComp::VerticalScaleConstraints::GetUnitName() const
{
	return QObject::tr("Scale");
}


double CEventGroupControllerComp::VerticalScaleConstraints::GetDisplayMultiplicationFactor() const
{
	return 1;
}


istd::CRange CEventGroupControllerComp::VerticalScaleConstraints::GetValueRange() const
{
	istd::CRange range(m_minVerticalScale, DBL_MAX);
	return range;
}


const imath::IDoubleManip& CEventGroupControllerComp::VerticalScaleConstraints::GetValueManip() const
{
	return *this;
}


// reimplemented (imath::IDoubleManip)

int CEventGroupControllerComp::VerticalScaleConstraints::GetPrecision() const
{
	return 15;
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


