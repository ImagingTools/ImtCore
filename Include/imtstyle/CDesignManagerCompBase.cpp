#include <imtstyle/CDesignManagerCompBase.h>


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QDebug>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtstyle
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
	int currentIndex = GetSelectedOptionIndex();
	if (currentIndex != index){
		bool retVal = BaseClass2::SetSelectedOptionIndex(index);

		if (retVal){
			retVal = retVal && ApplyDesignScheme(index);
		}

		return retVal;
	}

	return true;
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

	qDebug(qPrintable(QString("%1: Start applying color scheme index: %2").arg(qPrintable(GetComponentId(GetComponentContext()))).arg(index)));

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
						else if (m_paletteProviderCompPtr.IsValid()){
							QPalette palette;
							if (m_paletteProviderCompPtr->GetColorPalette(imtStylePtr->GetDesignSchemaId(designSchema), palette)){
								imtStylePtr->SetPalette(designSchema, palette);
							}
						}
					}

					m_initResources[designSchema]();

					I_IF_DEBUG(
						qDebug() << "Start dumping of the embedded resources";
						QDirIterator it(":/Styles", QDirIterator::Subdirectories);
						while (it.hasNext()){
							qDebug() << it.next();
						}
						qDebug() << "Finished dumping of the embedded resources";
					)

					imtStylePtr->SetDesignSchema(designSchema);
				}
			}
		}
	}
	else{
		retVal = false;
	}

	qDebug(qPrintable(QString("%1: Finished applying color scheme index: %2").arg(qPrintable(GetComponentId(GetComponentContext()))).arg(index)));

	return retVal;
}


// public methods of the embedded class DesignList

void CDesignManagerCompBase::DesignList::UpdateDesignList()
{
	istd::CChangeNotifier changeNotifier(this);

	m_designList.clear();

	imtstyle::CImtStyle* imtStyle = imtstyle::CImtStyle::GetInstance();
	if (imtStyle != nullptr){
		qDebug(qPrintable(QString("Update list of color schemes: %1").arg(imtStyle->GetDesignSchemaCount())));

		for (int i = 0; i < imtStyle->GetDesignSchemaCount(); i++){
			DesignInfo designInfo;
			designInfo.id = imtStyle->GetDesignSchemaId((imtstyle::CImtStyle::DesignSchema)i);
			designInfo.name = imtStyle->GetDesignSchemaName((imtstyle::CImtStyle::DesignSchema)i);

			qDebug(qPrintable(QString("Color scheme added: %1").arg(designInfo.name)));

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


} // namespace imtstyle


