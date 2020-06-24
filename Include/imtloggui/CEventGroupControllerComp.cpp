#include <imtloggui/CEventGroupControllerComp.h>


// Qt includes
#include <QDebug>
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
		qDebug() << "-------------------";
		qDebug() << eventItemController->GetGraphicsItem()->boundingRect();
		qDebug() << m_itemGroupPtr->childrenBoundingRect();
		eventItemController->GetGraphicsItem()->setParentItem(m_itemGroupPtr);
		qDebug() << m_itemGroupPtr->childrenBoundingRect();

		m_groups[groupId] = eventItemController;

		QRectF sceneRect = m_viewPtr->GetSceneRect();

		totalHeight = 0;
		for (IEventItemController* item : m_groups){
			totalHeight += item->GetGroupHeight();
		}

		sceneRect.setBottom(0);
		sceneRect.setTop(-totalHeight);

		m_viewPtr->SetSceneRect(sceneRect);
	}

	return eventItemController;
}


bool CEventGroupControllerComp::RemoveGroup(const QByteArray& groupId)
{
	if (m_groups.contains(groupId)){
		delete m_groups[groupId];
		m_groups.remove(groupId);
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


CEventItemBase* CEventGroupControllerComp::AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr)
{
	for (QByteArray groupId : m_messageIdMap.keys()){
		if (m_messageIdMap[groupId].contains(messagePtr->GetInformationId())){
			IEventItemController* groupPtr = GetGroup(groupId);
			if (groupPtr != nullptr){
				CEventItemBase* eventPtr = groupPtr->AddEvent(messagePtr);
				if (eventPtr != nullptr){
					return eventPtr;
				}

				break;
			}
		}
	}

	if (m_generalGroupRefCompPtr.IsValid()){
		IEventItemController* groupPtr = GetGroup(m_generalGroupRefCompPtr->GetGroupId());
		if (groupPtr != nullptr){
			return groupPtr->AddEvent(messagePtr);
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

	//QRectF newSceneRect = m_viewPtr->GetSceneRect();

	int totalHeight = 0;
	for (IEventItemController* item : m_groups){
		totalHeight += item->GetGroupHeight();
	}

	//newSceneRect.setBottom(40 / m_viewPtr->viewportTransform().m22());
	//newSceneRect.setTop(-totalHeight);

	//if (m_viewPtr->GetSceneRect() != newSceneRect){
	//	m_viewPtr->SetSceneRect(newSceneRect);
	//}

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
	istd::CChangeNotifier notifier(this);
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


} // namespace imtloggui


