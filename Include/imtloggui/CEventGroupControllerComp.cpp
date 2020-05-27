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
	m_graphicsItem(nullptr),
	m_minimumVerticalScale(1)
{
}


void CEventGroupControllerComp::SetScene(QGraphicsScene* scenePtr)
{
	m_scenePtr = scenePtr;
}


void CEventGroupControllerComp::SetView(QGraphicsView* viewPtr)
{
	m_viewPtr = viewPtr;
}


void CEventGroupControllerComp::SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


bool CEventGroupControllerComp::CreateGraphicsItem()
{
	if (m_graphicsItem == nullptr && m_scenePtr != nullptr && m_timeAxisPtr != nullptr){
		m_graphicsItem = new QGraphicsItemGroup();

		m_scenePtr->addItem(m_graphicsItem);
	}

	return false;
}


bool CEventGroupControllerComp::DestroyGraphicsItem()
{
	if (m_graphicsItem != nullptr && m_scenePtr != nullptr){
	

		for (IEventItemController* itemPtr : m_groups){
			RemoveGroup(itemPtr->GetGroupId());
		}
		m_groups.clear();

		m_scenePtr->removeItem(m_graphicsItem);
		delete m_graphicsItem;
		m_graphicsItem = nullptr;

		return true;
	}

	return false;
}


QGraphicsItem* CEventGroupControllerComp::GetGraphicsItem()
{
	return m_graphicsItem;
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


QString CEventGroupControllerComp::GetGroupName(const QByteArray& groupId) const
{
	if (m_groups.contains(groupId)){
		return m_groups[groupId]->GetGroupName();
	}

	return QString();
}


IEventItemController* CEventGroupControllerComp::AddGroup(const QByteArray& groupId, const QString& groupName)
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

	if (eventItemController != nullptr){
		eventItemController->SetGroupName(groupName);
		eventItemController->SetScene(m_scenePtr);
		eventItemController->SetTimeAxis(m_timeAxisPtr);
		eventItemController->CreateGraphicsItem();

		int totalHeight = 0;
		for (IEventItemController* item : m_groups){
			totalHeight += item->GetGroupHeight();
		}

		eventItemController->GetGraphicsItem()->setParentItem(m_graphicsItem);
		eventItemController->GetGraphicsItem()->setPos(0, -totalHeight);

		m_groups[groupId] = eventItemController;

		QRectF sceneRect = m_scenePtr->sceneRect();

		totalHeight = 0;
		for (IEventItemController* item : m_groups){
			totalHeight += item->GetGroupHeight();
		}

		sceneRect.setBottom(40 / m_viewPtr->viewportTransform().m22());
		sceneRect.setTop(-totalHeight);

		m_viewPtr->setSceneRect(sceneRect);
	}

	return eventItemController;
}


void CEventGroupControllerComp::AddGroups(const imtlog::IMessageGroupInfoProvider::GroupInfos& groupInfos)
{
	for (const imtlog::IMessageGroupInfoProvider::GroupInfo groupInfo : groupInfos){
		AddGroup(groupInfo.id, groupInfo.name);
	}
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


bool CEventGroupControllerComp::SetVisible(const QByteArray& groupId, bool isVisible) const
{
	if (m_groups.contains(groupId)){
		m_groups[groupId]->SetVisible(isVisible);
		return true;
	}

	return false;
}


void CEventGroupControllerComp::OnTimeAxisChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->OnTimeAxisChanged();
		}
	}
}


void CEventGroupControllerComp::OnViewPortChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->OnViewPortChanged();
		}
	}

	QRectF newSceneRect = m_viewPtr->sceneRect();

	int totalHeight = 0;
	for (IEventItemController* item : m_groups){
		totalHeight += item->GetGroupHeight();
	}

	newSceneRect.setBottom(40 / m_viewPtr->viewportTransform().m22());
	newSceneRect.setTop(-totalHeight);

	if (m_viewPtr->sceneRect() != newSceneRect){
		m_viewPtr->setSceneRect(newSceneRect);
	}

	double minimumVerticalScale = m_viewPtr->viewport()->rect().height() / newSceneRect.height();
	if (minimumVerticalScale < 1){
		minimumVerticalScale = 1;
	}
	
	if (totalHeight != 0 && m_minimumVerticalScale != minimumVerticalScale){
		m_minimumVerticalScale = minimumVerticalScale;
		m_verticalScaleConstraints.SetMinScale(minimumVerticalScale);
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


