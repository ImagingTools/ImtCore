#include <imtgui/CDesignManagerCompBase.h>


extern int qInitResources_imtguilight();
extern int qCleanupResources_imtguilight();
extern int qInitResources_imtguidark();
extern int qCleanupResources_imtguidark();


namespace imtgui
{


// public methods

CDesignManagerCompBase::CDesignManagerCompBase()
	:m_selectionObserver(*this)
{
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
	BaseClass::OnComponentCreated();

	if (m_selectionParamCompPtr.IsValid() && m_selectionParamModelPtr.IsValid()){
		m_selectionObserver.RegisterObject(
					m_selectionParamCompPtr.GetPtr(),
					&CDesignManagerCompBase::OnDesignSelectionUpdated);
	}

	RegisterResourcesFunctions();
}


void CDesignManagerCompBase::OnComponentDestroyed()
{
	m_selectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CDesignManagerCompBase::OnDesignSelectionUpdated(const istd::IChangeable::ChangeSet& changeSet, const iprm::ISelectionParam* selectionParamPtr)
{
	int index = selectionParamPtr->GetSelectedOptionIndex();
	imtwidgets::CImtStyle::DesignSchema designSchema = imtwidgets::CImtStyle::GetInstance().GetDesignSchemaFromIndex(index);

	if (index < imtwidgets::CImtStyle::GetInstance().GetDesignSchemaCount()){
		if (m_initResources.contains(designSchema)){
			if (m_initResources[designSchema] != nullptr){
				for (ResourceFunctionPtr functionPtr : m_cleanupResources){
					if (functionPtr != nullptr){
						functionPtr();
					}
				}

				m_initResources[designSchema]();
				imtwidgets::CImtStyle::GetInstance().SetDesignSchema(designSchema);
			}
		}
	}
}


} // namespace imtgui


