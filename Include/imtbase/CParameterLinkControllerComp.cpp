#include <imtbase/CParameterLinkControllerComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// protected methods

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

				int targetOptionsCount = m_paramsManagerCompPtr->GetParamsSetsCount();
				for (int i = 0; i < targetOptionsCount; i++){
					const iprm::ISelectionParam* targetSelectionParamPtr = FindReferenceSelection(i);
					if (targetSelectionParamPtr != nullptr){
						int selectedIndex = targetSelectionParamPtr->GetSelectedOptionIndex();
						if (selectedIndex == -1){
							removeTargetOptions.append(i);
							continue;
						}

						const iprm::IOptionsList* targetOptionsPtr = targetSelectionParamPtr->GetSelectionConstraints();
						if (targetOptionsPtr != nullptr){
							QByteArray targetId = targetOptionsPtr->GetOptionId(selectedIndex);

							if (!availableIds.contains(targetId)){
								removeTargetOptions.append(i);

								continue;
							}
						}
					}
				}

				if (removeTargetOptions.count() > 0){
					istd::CChangeNotifier changeNotifier(m_paramsManagerCompPtr.GetPtr());

					for (int i = removeTargetOptions.count() - 1; i >= 0; i--){
						m_paramsManagerCompPtr->RemoveParamsSet(removeTargetOptions[i]);
					}

					changeNotifier.Abort();
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


