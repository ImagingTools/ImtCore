#include <imtgui/CDesignManagerCompBase.h>


extern int qInitResources_imtguilight();
extern int qCleanupResources_imtguilight();
extern int qInitResources_imtguidark();
extern int qCleanupResources_imtguidark();


namespace imtgui
{


// public methods

// reimplemented (iprm:ISelectionParam)

bool CDesignManagerCompBase::SetSelectedOptionIndex(int index)
{
	bool retVal = BaseClass::SetSelectedOptionIndex(index);

	if (retVal){
		imtwidgets::CImtStyle* imtStylePtr = imtwidgets::CImtStyle::GetInstance();
		Q_ASSERT(imtStylePtr != nullptr);

		imtwidgets::CImtStyle::DesignSchema designSchema = imtStylePtr->GetDesignSchemaFromIndex(index);
		if (designSchema != imtwidgets::CImtStyle::DS_INVALID){
			if (index < imtStylePtr->GetDesignSchemaCount()){
				if (m_initResources.contains(designSchema)){
					if (m_initResources[designSchema] != nullptr){
						for (ResourceFunctionPtr functionPtr : m_cleanupResources){
							if (functionPtr != nullptr){
								functionPtr();
							}
						}

						m_initResources[designSchema]();
						imtStylePtr->SetDesignSchema(designSchema);

						for (int i = 0; i < m_slaveCompPtr.GetCount(); i++){
							iprm::ISelectionParam* selectionParamPtr = m_slaveCompPtr[i];
							if (selectionParamPtr != nullptr){
								retVal = retVal && selectionParamPtr->SetSelectedOptionIndex(index);
							}
						}
					}
				}
			}
		}
	}

	return retVal;
}


// protected methods

void CDesignManagerCompBase::RegisterResourcesFunctions()
{
}


void CDesignManagerCompBase::SetDesignResourcesFunctions(
			imtwidgets::CImtStyle::DesignSchema designSchema,
			ResourceFunctionPtr initResources,
			ResourceFunctionPtr cleanupResources)
{
	if (initResources != nullptr && cleanupResources != nullptr){
		m_initResources[designSchema] = initResources;
		m_cleanupResources[designSchema] = cleanupResources;
	}
}


// reimplemented (icomp::CComponentBase)

void CDesignManagerCompBase::OnComponentCreated()
{
	RegisterResourcesFunctions();

	BaseClass::OnComponentCreated();
}


void CDesignManagerCompBase::OnComponentDestroyed()
{
	for (ResourceFunctionPtr functionPtr : m_cleanupResources){
		if (functionPtr != nullptr){
			functionPtr();
		}
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtgui


