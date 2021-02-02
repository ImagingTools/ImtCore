#include <imtloggui/CRepresentationViewCompBase.h>


// Qt includes
#include <QtCore/QThread>


namespace imtloggui
{


// public methods

CRepresentationViewCompBase::CRepresentationViewCompBase()
	:m_isActivated(false)
{
	connect(this, &CRepresentationViewCompBase::EmitRepresentationUpdated, this, &CRepresentationViewCompBase::OnRepresentationUpdated, Qt::QueuedConnection);
}


// reimplemented (imtloggui::IScenographer)

void CRepresentationViewCompBase::SetItemsSceneId(const QByteArray& sceneId)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	m_itemsSceneId = sceneId;

	for (GraphicsItemPtr itemPtr : m_itemsOnScene){
		itemPtr->setData(DK_GROUP_ID, m_itemsSceneId);
	}
}


void CRepresentationViewCompBase::ActivateGraphicsItems()
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	if (m_graphicsSceneProviderCompPtr.IsValid()){
		// Additional integrity check
		if (!m_isActivated){
			Q_ASSERT(m_itemsOnScene.isEmpty());
		}

		QGraphicsScene* scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
		if (scenePtr != nullptr){
			UpdateView(scenePtr);

			m_isActivated = true;
		}
	}
}


void CRepresentationViewCompBase::DeactivateGraphicsItems()
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	if (m_graphicsSceneProviderCompPtr.IsValid()){
		// Additional integrity check
		if (!m_isActivated){
			Q_ASSERT(m_itemsOnScene.isEmpty());
		}

		QGraphicsScene* scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
		if (scenePtr != nullptr){
			for (GraphicsItemPtr itemPtr : m_itemsOnScene){
				scenePtr->removeItem(itemPtr.GetPtr());
			}

			m_itemsOnScene.clear();

			m_isActivated = false;
		}
	}
}


// protected methods

void CRepresentationViewCompBase::UpdateView(QGraphicsScene* scenePtr)
{
	QMutexLocker locker(&m_generatedItemsMutex);
	GraphicsItemList generatedItems = m_generatedItems;
	locker.unlock();

	GraphicsItemList removedItems = GetRemovedItemList();
	GraphicsItemList addedItems = GetAddedItemList();

	for (GraphicsItemPtr itemPtr : removedItems){
		scenePtr->removeItem(itemPtr.GetPtr());
	}

	for (GraphicsItemPtr itemPtr : addedItems){
		scenePtr->addItem(itemPtr.GetPtr());
		itemPtr->setData(DK_GROUP_ID, m_itemsSceneId);
	}

	m_itemsOnScene = generatedItems;
}


// protected methods

CRepresentationViewCompBase::GraphicsItemList CRepresentationViewCompBase::GetAddedItemList()
{
	GraphicsItemList retVal;

	for (int i = 0; i < m_itemsOnScene.count(); i++){
		if (!m_generatedItems.contains(m_itemsOnScene[i])){
			retVal.push_back(m_itemsOnScene[i]);
		}
	}

	return retVal;
}


CRepresentationViewCompBase::GraphicsItemList CRepresentationViewCompBase::GetRemovedItemList()
{
	GraphicsItemList retVal;

	for (int i = 0; i < m_generatedItems.count(); i++){
		if (!m_itemsOnScene.contains(m_generatedItems[i])){
			retVal.push_back(m_generatedItems[i]);
		}
	}

	return retVal;
}


// protected slots:

void CRepresentationViewCompBase::OnRepresentationUpdated()
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	if (m_graphicsSceneProviderCompPtr.IsValid()){
		if (m_isActivated){
			QGraphicsScene* scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
			if (scenePtr != nullptr){
				UpdateView(scenePtr);
			}
		}
		else{
			// Additional integrity check
			Q_ASSERT(m_itemsOnScene.isEmpty());
		}
	}

	EndChanges(istd::IChangeable::GetAnyChange());
}


} // namespace imtloggui

