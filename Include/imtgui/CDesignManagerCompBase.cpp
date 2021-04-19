#include <imtgui/CDesignManagerCompBase.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>


extern int qInitResources_imtguilight();
extern int qCleanupResources_imtguilight();
extern int qInitResources_imtguidark();
extern int qCleanupResources_imtguidark();


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
	m_constraints.Update();

	SetSelectionConstraints(&m_constraints);

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


// public methods of the embedded class Constraints

void CDesignManagerCompBase::Constraints::Update()
{
	istd::CChangeGroup changeGroup(this);

	m_options.clear();

	imtwidgets::CImtStyle* imtStyle = imtwidgets::CImtStyle::GetInstance();
	if (imtStyle != nullptr){
		for (int i = 0; i < imtStyle->GetDesignSchemaCount(); i++){
			Option option;
			option.id = imtStyle->GetDesignSchemaId((imtwidgets::CImtStyle::DesignSchema)i);
			option.name = imtStyle->GetDesignSchemaName((imtwidgets::CImtStyle::DesignSchema)i);

			m_options.append(option);
		}
	}
}


// reimplemented (iprm::IOptionsList)

int CDesignManagerCompBase::Constraints::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CDesignManagerCompBase::Constraints::GetOptionsCount() const
{
	return m_options.count();
}


QString CDesignManagerCompBase::Constraints::GetOptionName(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_options.count()){
		return m_options[index].name;
	}

	return QString();
}


QString CDesignManagerCompBase::Constraints::GetOptionDescription(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_options.count()){
		return m_options[index].description;
	}

	return QString();
}


QByteArray CDesignManagerCompBase::Constraints::GetOptionId(int index) const
{
	Q_ASSERT(index >= 0);

	if (index < m_options.count()){
		return m_options[index].id;
	}

	return QByteArray();
}


bool CDesignManagerCompBase::Constraints::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtgui


