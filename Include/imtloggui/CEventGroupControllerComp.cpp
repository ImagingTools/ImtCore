#include <imtloggui/CEventGroupControllerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QScrollBar>


namespace imtloggui
{


CEventGroupControllerComp::CEventGroupControllerComp()
	: m_scenePtr(nullptr),
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
		IEventItemController* eventItemControllerPtr = m_groupRefsCompPtr[i];
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

	if (m_groupRefsCompPtr.IsValid()){
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


bool CEventGroupControllerComp::SetVisible(QByteArray groupId, bool isVisible) const
{
	if (m_groups.contains(groupId)){
		m_groups[groupId]->SetVisible(isVisible);
		return true;
	}

	return false;
}


void CEventGroupControllerComp::TimeAxisChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->TimeAxisChanged();
		}
	}
}


double CEventGroupControllerComp::ViewPortChanged()
{
	if (m_groupRefsCompPtr.IsValid()){
		for (int i = 0; i < m_groupRefsCompPtr.GetCount(); i++){
			m_groupRefsCompPtr[i]->ViewPortChanged();
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
	
	if (totalHeight != 0 && m_minimumVerticalScale != minimumVerticalScale){
		m_minimumVerticalScale = minimumVerticalScale;
		Q_EMIT MinimumVerticalScaleChanged(minimumVerticalScale);
		return minimumVerticalScale;
	}

	return 0;
}


} // namespace imtloggui


