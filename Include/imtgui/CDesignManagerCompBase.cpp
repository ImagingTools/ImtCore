#include <imtgui/CDesignManagerCompBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtgui
{


// public methods

CDesignManagerCompBase::CDesignManagerCompBase()
{
	EnableLocalization(true);
}


CDesignManagerCompBase::~CDesignManagerCompBase()
{
	EnableLocalization(false);
}


// reimplemented (iprm:ISelectionParam)

bool CDesignManagerCompBase::SetSelectedOptionIndex(int index)
{
	bool retVal = BaseClass2::SetSelectedOptionIndex(index);

	if (retVal){
		retVal = retVal && ApplyDesignScheme(index);
	}

	return retVal;
}


// reimplemented (iser::ISerializable)

bool CDesignManagerCompBase::Serialize(iser::IArchive& archive)
{
	bool retVal = BaseClass2::Serialize(archive);

	if (retVal && !archive.IsStoring()){
		int index = GetSelectedOptionIndex();

		ApplyDesignScheme(index);
	}

	return retVal;
}


// protected methods

void CDesignManagerCompBase::SetDesignResourcesFunctions(
			imtstyle::CImtStyle::DesignSchema designSchema,
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

	m_designs.UpdateDesignList();

	SetSelectionConstraints(&m_designs);

	RegisterResourcesFunctions();

	if (m_designs.GetOptionsCount() > 0){
		SetSelectedOptionIndex(0);
	}
}


void CDesignManagerCompBase::OnComponentDestroyed()
{
	for (ResourceFunctionPtr cleanupFunctionPtr : m_cleanupResources){
		if (cleanupFunctionPtr != nullptr){
			cleanupFunctionPtr();
		}
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (ibase::TLocalizableWrap)

void CDesignManagerCompBase::OnLanguageChanged()
{
	m_designs.UpdateDesignList();
}


// private methods

bool CDesignManagerCompBase::ApplyDesignScheme(int index)
{
	bool retVal = true;

	imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);

	imtstyle::CImtStyle::DesignSchema designSchema = imtStylePtr->GetDesignSchemaFromIndex(index);
	if (designSchema != imtstyle::CImtStyle::DS_INVALID){
		if (index < imtStylePtr->GetDesignSchemaCount()){
			if (m_initResources.contains(designSchema)){
				if (m_initResources[designSchema] != nullptr){
					for (ResourceFunctionPtr cleanupFunctionPtr : m_cleanupResources){
						if (cleanupFunctionPtr != nullptr){
							cleanupFunctionPtr();
						}
					}

					{
						imtstyle::CImtStyle::DesignSchemaSetterBlocker blocker(*imtStylePtr);

						if (m_slaveCompPtr.IsValid()){
							retVal = retVal && m_slaveCompPtr->SetSelectedOptionIndex(index);
						}
					}

					m_initResources[designSchema]();
					imtStylePtr->SetDesignSchema(designSchema);
				}
			}
		}
	}
	else{
		retVal = false;
	}

	return retVal;
}


// public methods of the embedded class DesignList

void CDesignManagerCompBase::DesignList::UpdateDesignList()
{
	istd::CChangeNotifier changeNotifier(this);

	m_designList.clear();

	imtstyle::CImtStyle* imtStyle = imtstyle::CImtStyle::GetInstance();
	if (imtStyle != nullptr){
		for (int i = 0; i < imtStyle->GetDesignSchemaCount(); i++){
			DesignInfo designInfo;
			designInfo.id = imtStyle->GetDesignSchemaId((imtstyle::CImtStyle::DesignSchema)i);
			designInfo.name = imtStyle->GetDesignSchemaName((imtstyle::CImtStyle::DesignSchema)i);

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


