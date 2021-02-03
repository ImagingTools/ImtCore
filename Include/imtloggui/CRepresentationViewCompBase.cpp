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

void CRepresentationViewCompBase::AddItemsToScene()
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	QGraphicsScene* scenePtr = nullptr;
	if (m_graphicsSceneProviderCompPtr.IsValid()){
		scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
	}

	if (scenePtr != nullptr){
		if (!m_isActivated){
			Q_ASSERT(m_itemsOnScene.isEmpty());
		}

		if (!m_isActivated){
			m_isActivated = true;

			UpdateItemsOnScene(scenePtr);

			EndChanges(istd::IChangeable::GetAnyChange());
		}
	}
}


void CRepresentationViewCompBase::RemoveItemsFromScene()
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	QGraphicsScene* scenePtr = nullptr;
	if (m_graphicsSceneProviderCompPtr.IsValid()){
		scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
	}

	if (scenePtr != nullptr){
		if (!m_isActivated){
			Q_ASSERT(m_itemsOnScene.isEmpty());
		}

		if (m_isActivated){
			m_isActivated = false;

			for (GraphicsItemPtr itemPtr : m_itemsOnScene){
				scenePtr->removeItem(itemPtr.GetPtr());
			}

			m_itemsOnScene.clear();
		}
	}
}


// reimplemented (iprm::IIdParam)

QByteArray CRepresentationViewCompBase::GetId() const
{
	return m_groupId;
}


void CRepresentationViewCompBase::SetId(const QByteArray& id)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());

	m_groupId = id;

	for (GraphicsItemPtr itemPtr : m_itemsOnScene){
		itemPtr->setData(0, m_groupId);
	}
}


// reimplemented (iser::ISerializable)

bool CRepresentationViewCompBase::Serialize(iser::IArchive& archive)
{
	return false;
}


// protected methods

void CRepresentationViewCompBase::UpdateItemsOnScene(QGraphicsScene* scenePtr)
{
	QMutexLocker locker(&m_generatedItemsMutex);
	GraphicsItemList generatedItems = m_generatedItems;
	locker.unlock();

	GraphicsItemList removedItems = GetRemovedItems();
	GraphicsItemList addedItems = GetAddedItems();

	for (GraphicsItemPtr itemPtr : removedItems){
		scenePtr->removeItem(itemPtr.GetPtr());
	}

	for (GraphicsItemPtr itemPtr : addedItems){
		scenePtr->addItem(itemPtr.GetPtr());
		itemPtr->setData(0, m_groupId);
	}

	m_itemsOnScene = generatedItems;
}


// protected methods

CRepresentationViewCompBase::GraphicsItemList CRepresentationViewCompBase::GetAddedItems()
{
	GraphicsItemList retVal;

	for (int i = 0; i < m_itemsOnScene.count(); i++){
		if (!m_generatedItems.contains(m_itemsOnScene[i])){
			retVal.push_back(m_itemsOnScene[i]);
		}
	}

	return retVal;
}


CRepresentationViewCompBase::GraphicsItemList CRepresentationViewCompBase::GetRemovedItems()
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

	if (m_isActivated){
		QGraphicsScene* scenePtr = nullptr;
		if (m_graphicsSceneProviderCompPtr.IsValid()){
			scenePtr = m_graphicsSceneProviderCompPtr->GetGraphicsScene();
		}

		if (scenePtr != nullptr){
			UpdateItemsOnScene(scenePtr);

			EndChanges(istd::IChangeable::GetAnyChange());
		}
	}
}


} // namespace imtloggui

