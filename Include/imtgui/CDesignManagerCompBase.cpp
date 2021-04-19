#include <imtgui/CDesignManagerCompBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtgui
{


// protected methods

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


// reimplemented (iprm:ISelectionParam)

bool CDesignManagerCompBase::SetSelectedOptionIndex(int index)
{
	bool retVal = BaseClass2::SetSelectedOptionIndex(index);

	if (retVal){
		imtwidgets::CImtStyle* imtStylePtr = imtwidgets::CImtStyle::GetInstance();
		Q_ASSERT(imtStylePtr != nullptr);

		imtwidgets::CImtStyle::DesignSchema designSchema = imtStylePtr->GetDesignSchemaFromIndex(index);
		if (designSchema != imtwidgets::CImtStyle::DS_INVALID){
			if (index < imtStylePtr->GetDesignSchemaCount()){
				if (m_initResources.contains(designSchema)){
					if (m_initResources[designSchema] != nullptr){
						for (ResourceFunctionPtr cleanupFunctionPtr : m_cleanupResources){
							if (cleanupFunctionPtr != nullptr){
								cleanupFunctionPtr();
							}
						}

						m_initResources[designSchema]();
						imtStylePtr->SetDesignSchema(designSchema);

						if (m_slaveCompPtr.IsValid()){
							retVal = retVal && m_slaveCompPtr->SetSelectedOptionIndex(index);
						}
					}
				}
			}
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CDesignManagerCompBase::OnComponentCreated()
{
	m_designs.UpdateDesignList();

	SetSelectionConstraints(&m_designs);

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


// public methods of the embedded class DesignList

void CDesignManagerCompBase::DesignList::UpdateDesignList()
{
	istd::CChangeNotifier changeNotifier(this);

	m_designList.clear();

	imtwidgets::CImtStyle* imtStyle = imtwidgets::CImtStyle::GetInstance();
	if (imtStyle != nullptr){
		for (int i = 0; i < imtStyle->GetDesignSchemaCount(); i++){
			DesignInfo designInfo;
			designInfo.id = imtStyle->GetDesignSchemaId((imtwidgets::CImtStyle::DesignSchema)i);
			designInfo.name = imtStyle->GetDesignSchemaName((imtwidgets::CImtStyle::DesignSchema)i);

			m_designList.append(designInfo);
		}
	}
}


// reimplemented (iprm::IOptionsList)

int CDesignManagerCompBase::DesignList::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CDesignManagerCompBase::DesignList::GetOptionsCount() const
{
	return m_designList.count();
}


QString CDesignManagerCompBase::DesignList::GetOptionName(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_designList.count()){
		return m_designList[index].name;
	}

	return QString();
}


QString CDesignManagerCompBase::DesignList::GetOptionDescription(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_designList.count()){
		return m_designList[index].description;
	}

	return QString();
}


QByteArray CDesignManagerCompBase::DesignList::GetOptionId(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_designList.count()){
		return m_designList[index].id;
	}

	return QByteArray();
}


bool CDesignManagerCompBase::DesignList::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtgui


