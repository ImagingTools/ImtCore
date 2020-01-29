#include <imtbase/CLocalizedHelpPathProviderComp.h>


// Qt includes
#include <QtCore/QLocale>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtbase
{


// reimplemented (idoc::IHelpFileProvider)

QString CLocalizedHelpPathProviderComp::GetHelpFilePath(const QString& /*contextText*/, const istd::IPolymorphic* /*contextObjectPtr*/) const
{
	int localeIndex = -1;

	if (m_languageSelectionCompPtr.IsValid()){
		localeIndex = m_languageSelectionCompPtr->GetSelectedOptionIndex();
	}

	return GetPathForLocale(localeIndex);
}


// reimplemented (idoc::IHelpFileInfoProvider)

double CLocalizedHelpPathProviderComp::GetHelpQuality(const QString& /*contextText*/, const istd::IPolymorphic* /*contextObjectPtr*/) const
{
	return 1.0;
}


// private methods

QString CLocalizedHelpPathProviderComp::GetPathForLocale(int languageIndex) const
{
	QString path;
	QString localeId;

	if (languageIndex == -1) {
		localeId = QLocale::system().name();
	}
	else{
		if (m_languageSelectionCompPtr.IsValid()){
			const iprm::IOptionsList* listPtr = m_languageSelectionCompPtr->GetSelectionConstraints();
			if (listPtr != nullptr){
				if (languageIndex <= listPtr->GetOptionsCount()){
					localeId = listPtr->GetOptionId(languageIndex);
				}
			}
		}
	}

	if (localeId.isEmpty()){
		return QString();
	}

	int helpPathIndex = m_localeIdsAttrPtr.FindValue(localeId.toUtf8());

	if((helpPathIndex >= 0) && (helpPathIndex < m_helpPathListCompPtr.GetCount())){
		ifile::IFileNameParam* helpPathParamPtr = m_helpPathListCompPtr[helpPathIndex];
		if (helpPathParamPtr != nullptr){
			return helpPathParamPtr->GetPath();
		}
	}

	return QString();
}


}


