#include <imtbase/CParameterLinkControllerComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>
#include <istd/CChangeGroup.h>


namespace imtbase
{


// protected methods

// reimplemented (imod::IObserver)

void CParameterLinkControllerComp::BeforeUpdate(imod::IModel* modelPtr)
{
	BaseClass2::BeforeUpdate(modelPtr);

	m_referenceMap.clear();

	if (!m_paramsManagerCompPtr.IsValid()){
		return;
	}

	const iprm::IOptionsList* paramsSetListPtr = m_paramsManagerCompPtr->GetSelectionConstraints();
	if (paramsSetListPtr == nullptr){
		return;
	}

	int targetParamsSetCount = paramsSetListPtr->GetOptionsCount();
	for (int i = 0; i < targetParamsSetCount; i++){
		QByteArray targetParamsSetId = paramsSetListPtr->GetOptionId(i);

		const iprm::ISelectionParam* targetSelectionParamPtr = FindReferenceSelection(i);
		if (targetSelectionParamPtr != nullptr){
			int selectedIndex = targetSelectionParamPtr->GetSelectedOptionIndex();
			if (selectedIndex >= 0){
				const iprm::IOptionsList* referenceListPtr = targetSelectionParamPtr->GetSelectionConstraints();
				if (referenceListPtr != nullptr){
					int targetOptionsCount = referenceListPtr->GetOptionsCount();
					if (selectedIndex < targetOptionsCount){
						QByteArray referenceId = referenceListPtr->GetOptionId(selectedIndex);

						m_referenceMap[targetParamsSetId] = referenceId;
					}
				}
			}
		}
	}
}


// reimplemented (imod::CSingleModelObserverBase)

void CParameterLinkControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(iprm::IParamsManager::CF_SET_REMOVED)){
		return;
	}

	if (m_paramsManagerCompPtr.IsValid() && m_selectionParamIdAttrPtr.IsValid()){
		iprm::IParamsManager* sourceParamsManagerPtr = GetObservedObject();
		if (sourceParamsManagerPtr != nullptr){
			const iprm::IOptionsList* sourceOptionsPtr = sourceParamsManagerPtr->GetSelectionConstraints();
			if (sourceOptionsPtr != nullptr){
				QByteArrayList availableIds;

				int sourceOptionsCount = sourceOptionsPtr->GetOptionsCount();
				for (int i = 0; i < sourceOptionsCount; i++){
					availableIds.append(sourceOptionsPtr->GetOptionId(i));
				}

				QVector<int> removeTargetOptions;

				const iprm::IOptionsList* paramsSetListPtr = m_paramsManagerCompPtr->GetSelectionConstraints();
				if (paramsSetListPtr == nullptr){
					return;
				}

				int targetParamsSetCount = paramsSetListPtr->GetOptionsCount();
				for (int i = 0; i < targetParamsSetCount; i++){
					QByteArray targetParamsSetId = paramsSetListPtr->GetOptionId(i);

					Q_ASSERT(m_referenceMap.contains(targetParamsSetId));

					QByteArray referenceId = m_referenceMap[targetParamsSetId];

					if (!availableIds.contains(referenceId)){
						removeTargetOptions.append(i);
					}
				}

				if (removeTargetOptions.count() > 0){
					istd::CChangeGroup changeGroup(m_paramsManagerCompPtr.GetPtr());

					for (int i = removeTargetOptions.count() - 1; i >= 0; i--){
						m_paramsManagerCompPtr->RemoveParamsSet(removeTargetOptions[i]);
					}
				}
			}
		}
	}
}


// private methods

const iprm::ISelectionParam* CParameterLinkControllerComp::FindReferenceSelection(int paramSetIndex)const
{
	if (m_paramsManagerCompPtr.IsValid() && m_selectionParamIdAttrPtr.IsValid()){
		iprm::IParamsSet* paramSetPtr = m_paramsManagerCompPtr->GetParamsSet(paramSetIndex);
		if (paramSetPtr != nullptr){
			return dynamic_cast<const iprm::ISelectionParam*>(paramSetPtr->GetParameter(*m_selectionParamIdAttrPtr));
		}
	}

	return nullptr;
}


} // namespace imtbase


