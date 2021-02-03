#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// public methods

CLayerViewComp::CLayerViewComp()
	:m_updateBridge(this),
	m_isEnabled(false)
{
}


// reimplemented (iprm::IEnableableParam)

bool CLayerViewComp::IsEnabled() const
{
	return m_isEnabled;
}


bool CLayerViewComp::IsEnablingAllowed() const
{
	return true;
}


bool CLayerViewComp::SetEnabled(bool isEnabled)
{
	if (m_isEnabled == isEnabled){
		return true;
	}

	m_isEnabled = isEnabled;
	m_updateBridge.EnsureModelsDetached();

	QByteArray selectedRepresentationViewId = GetSelectedRepresentationViewId();

	ICollectionInfo::Ids ids = GetElementIds();
	for (QByteArray& objectId : ids){
		IScenographer* scenographerPtr = dynamic_cast<IScenographer*>(
					const_cast<istd::IChangeable*>(BaseClass2::GetObjectPtr(objectId)));
		Q_ASSERT(scenographerPtr != nullptr);

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(scenographerPtr);
		Q_ASSERT(modelPtr != nullptr);

		if (objectId == selectedRepresentationViewId && m_isEnabled){
			scenographerPtr->AddItemsToScene();
			modelPtr->AttachObserver(&m_updateBridge);
		}
		else{
			scenographerPtr->RemoveItemsFromScene();
		}
	}

	return true;
}


// reimplemented (iprm::IIdParam)

QByteArray CLayerViewComp::GetId() const
{
	return m_groupId;
}


void CLayerViewComp::SetId(const QByteArray& id)
{
	m_groupId = id;

	ICollectionInfo::Ids ids = GetElementIds();
	for (QByteArray& objectId : ids){
		iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(
			const_cast<istd::IChangeable*>(BaseClass2::GetObjectPtr(objectId)));
		Q_ASSERT(idParamPtr != nullptr);

		idParamPtr->SetId(id);
	}
}


// protected methods

QByteArray CLayerViewComp::GetSelectedRepresentationViewId()
{
	if (IsModelAttached()){
		const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
		QByteArray selectedRepresentationViewId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

		if (GetElementIds().contains(selectedRepresentationViewId)){
			return selectedRepresentationViewId;
		}
	}

	return QByteArray();
}


// reimplemented (imod::CSingleModelObserverBase)

void CLayerViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (changeSet.Contains(iprm::ISelectionParam::CF_SELECTION_CHANGED) && m_isEnabled){
		m_updateBridge.EnsureModelsDetached();

		QByteArray selectedRepresentationViewId = GetSelectedRepresentationViewId();

		ICollectionInfo::Ids ids = GetElementIds();
		for (QByteArray& objectId : ids){
			IScenographer* scenographerPtr = dynamic_cast<IScenographer*>(
						const_cast<istd::IChangeable*>(BaseClass2::GetObjectPtr(objectId)));
			Q_ASSERT(scenographerPtr != nullptr);

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(scenographerPtr);
			Q_ASSERT(modelPtr != nullptr);

			if (objectId == selectedRepresentationViewId){
				scenographerPtr->AddItemsToScene();
				modelPtr->AttachObserver(&m_updateBridge);
			}
			else{
				scenographerPtr->RemoveItemsFromScene();
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CLayerViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_idAttrPtr[i].isEmpty());
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));

		istd::IChangeable* objectPtr = dynamic_cast<istd::IChangeable*>(m_representationViewCompPtr[i]);
		if (objectPtr != nullptr){
			RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", objectPtr);
		}
	}
}


} // namespace imtloggui


