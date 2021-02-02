#include <imtloggui/CScenographerBase.h>


namespace imtloggui
{


// public methods:

CScenographerBase::CScenographerBase()
	:m_updateBridge(this),
	m_isActivated(false)
{
}


// reimplemented (imtloggui::IScenographer)

void CScenographerBase::SetItemsSceneId(const QByteArray& sceneId)
{
	ICollectionInfo::Ids ids = GetElementIds();

	for (QByteArray id : ids){
		IScenographer* scenographerPtr = dynamic_cast<IScenographer*>(
					const_cast<istd::IChangeable*>(BaseClass::GetObjectPtr(id)));

		Q_ASSERT(scenographerPtr != nullptr);
		scenographerPtr->SetItemsSceneId(sceneId);
	}
}


void CScenographerBase::ActivateGraphicsItems()
{
	IScenographer* scenographerPtr = GetActiveElement();
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(GetActiveElement());

	if (scenographerPtr != nullptr && modelPtr == nullptr){
		scenographerPtr->ActivateGraphicsItems();
		modelPtr->AttachObserver(&m_updateBridge);
	}

	m_isActivated = true;
}


void CScenographerBase::DeactivateGraphicsItems()
{
	IScenographer* scenographerPtr = GetActiveElement();

	if (scenographerPtr != nullptr){
		scenographerPtr->DeactivateGraphicsItems();
		m_updateBridge.EnsureModelsDetached();
	}

	m_isActivated = false;
}


// protected methods

IScenographer* CScenographerBase::GetActiveElement()
{
	return nullptr;
}

// reimplemented (imod::CSingleModelObserverBase)

void CScenographerBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_isActivated){
		IScenographer* scenographerPtr = GetActiveElement();
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(GetActiveElement());

		if (scenographerPtr != nullptr && modelPtr == nullptr){
			scenographerPtr->ActivateGraphicsItems();
			modelPtr->AttachObserver(&m_updateBridge);
		}
	}
}

// reimplemented (imod::IObserver)

void CScenographerBase::BeforeUpdate(imod::IModel* modelPtr)
{
	IScenographer* scenographerPtr = GetActiveElement();
	if (scenographerPtr != nullptr){
		scenographerPtr->DeactivateGraphicsItems();
	}

	m_updateBridge.EnsureModelsDetached();
}


} // namespace imtloggui


