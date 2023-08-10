#include <imtstyle/CDesignManagerCompBase.h>


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QDebug>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iqt/CDesignThemeEvent.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtstyle
{


// public methods

CDesignManagerCompBase::CDesignManagerCompBase()
{
	EnableLocalization(true);

	m_designs.SetParent(this);
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
			retVal = retVal && ApplyDesignScheme(m_designs.GetOptionId(index));
		}

		return retVal;
	}

	return true;
}


// reimplemented (iser::ISerializable)

bool CDesignManagerCompBase::Serialize(iser::IArchive& archive)
{
	bool retVal = BaseClass2::Serialize(archive);

	return retVal;
}


// protected methods

void CDesignManagerCompBase::OnSystemStarting()
{
	m_designs.UpdateDesignList();

	SetSelectionConstraints(&m_designs);

	if (m_designs.GetOptionsCount() > 0){
		QByteArray themeId;
		int selectedIndex = BaseClass2::GetSelectedOptionIndex();
		if (selectedIndex < 0){
			SetSelectedOptionIndex(0);
		}

		themeId = m_designs.GetOptionId(selectedIndex);

		ApplyDesignScheme(themeId);
	}
}


// reimplemented (icomp::CComponentBase)

void CDesignManagerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CDesignManagerCompBase::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// reimplemented (ibase::TLocalizableWrap)

void CDesignManagerCompBase::OnLanguageChanged()
{
	m_designs.UpdateDesignList();
}


// private methods

bool CDesignManagerCompBase::ApplyDesignScheme(const QByteArray& themeId)
{
	bool retVal = true;

	qDebug(qPrintable(QString("%1: Start applying color scheme index: %2").arg(qPrintable(GetComponentId(GetComponentContext()))).arg(qPrintable(themeId))));

	imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);

	if (m_paletteProviderCompPtr.IsValid()){
		IColorPaletteProvider::ColorSchema colorShema;
		if (m_paletteProviderCompPtr->GetColorPalette(themeId, colorShema)){
			imtStylePtr->SetActiveDesignSchema(colorShema);
		}
	}
	else{
		retVal = false;
	}

	QCoreApplication::instance()->setProperty("ThemeId", themeId);
	QCoreApplication::postEvent(QCoreApplication::instance(), new iqt::CDesignThemeEvent(themeId));

	qDebug(qPrintable(QString("%1: Finished applying color scheme index: %2").arg(qPrintable(GetComponentId(GetComponentContext()))).arg(qPrintable(themeId))));

	return retVal;
}


// public methods of the embedded class DesignList

CDesignManagerCompBase::DesignList::DesignList()
	:m_parentPtr(nullptr)
{
}


void CDesignManagerCompBase::DesignList::SetParent(CDesignManagerCompBase * parentPtr)
{
	m_parentPtr = parentPtr;
}

void CDesignManagerCompBase::DesignList::UpdateDesignList()
{
	istd::CChangeNotifier changeNotifier(this);

	m_designList.clear();

	if (m_parentPtr->m_paletteProviderCompPtr.IsValid()){
		const imtbase::ICollectionInfo& themeList = m_parentPtr->m_paletteProviderCompPtr->GetDesignSchemaList();

		for (const QByteArray& themeId : themeList.GetElementIds()) {
			DesignInfo designInfo;
			designInfo.id = themeId;
			designInfo.name = themeList.GetElementInfo(themeId, imtbase::ICollectionInfo::EIT_NAME).toString();
			designInfo.description = themeList.GetElementInfo(themeId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

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
	if (index >= 0 && index < m_designList.count()){
		return m_designList[index].name;
	}

	return ("Default");
}


QString CDesignManagerCompBase::DesignList::GetOptionDescription(int index) const
{
	if (index >= 0 && index < m_designList.count()){
		return m_designList[index].description;
	}

	return QString();
}


QByteArray CDesignManagerCompBase::DesignList::GetOptionId(int index) const
{
	if (index >= 0 && index < m_designList.count()){
		return m_designList[index].id;
	}

	return QByteArray();
}


bool CDesignManagerCompBase::DesignList::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtstyle


