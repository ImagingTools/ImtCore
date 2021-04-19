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
	RegisterResourcesFunctions();
}


// protected methods

void CDesignManagerCompBase::RegisterResourcesFunctions()
{
}


void CDesignManagerCompBase::SetDesignResourcesFunctions(
			imtwidgets::CImtStyle::DesignSchema designSchema,
			ResourceFunctionPtr initResourceFunctionPtr,
			ResourceFunctionPtr cleanupResourceFunctionPtr)
{
	if (initResourceFunctionPtr != nullptr && cleanupResourceFunctionPtr != nullptr){
		m_initResourceFunctions[designSchema] = initResourceFunctionPtr;
		m_cleanupResourceFunctions[designSchema] = cleanupResourceFunctionPtr;
	}
}


// reimplemented (icomp::CComponentBase)

void CDesignManagerCompBase::OnComponentCreated()
{
	if (m_selectionParamCompPtr.IsValid() && m_selectionParamModelPtr.IsValid()){
		m_selectionObserver.RegisterObject(
					m_selectionParamCompPtr.GetPtr(),
					&CDesignManagerCompBase::OnDesignSelectionUpdated);
	}
}


void CDesignManagerCompBase::OnComponentDestroyed()
{
	m_selectionObserver.UnregisterAllObjects();
}


// private methods

void CDesignManagerCompBase::OnDesignSelectionUpdated(const istd::IChangeable::ChangeSet& changeSet, const iprm::ISelectionParam* selectionParamPtr)
{
	int index = selectionParamPtr->GetSelectedOptionIndex();
	if (index < imtwidgets::CImtStyle::Instance().GetDesignSchemaCount()){
		if (m_initResourceFunctions.contains(index)){
			if (m_initResourceFunctions[index] != nullptr){
				for (ResourceFunctionPtr functionPtr : m_cleanupResourceFunctions){
					if (functionPtr != nullptr){
						functionPtr();
					}
				}

				imtwidgets::CImtStyle::Instance().SetDesignSchema((imtwidgets::CImtStyle::DesignSchema)index);
				m_initResourceFunctions[index]();
			}
		}
	}
}


} // namespace imtgui


