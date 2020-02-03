#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>
#include <ifile/IFileNameParam.h>
#include <icomp/CComponentBase.h>
#include <idoc/IHelpFileProvider.h>


namespace imtbase
{


/**
	Component for language-related help path access.
*/
class CLocalizedHelpPathProviderComp:
			public icomp::CComponentBase,
			virtual public idoc::IHelpFileProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLocalizedHelpPathProviderComp);
		I_REGISTER_INTERFACE(idoc::IHelpFileProvider);
		I_ASSIGN(m_languageSelectionCompPtr, "LanguageSelection", "Language selector", true, "LanguageSelection");
		I_ASSIGN_MULTI_0(m_helpUrlPathListCompPtr, "HelpUrlPaths", "List of help URL paths related to the locale-IDs", false);
		I_ASSIGN_MULTI_0(m_helpLocalPathListCompPtr, "HelpLocalPaths", "List of help local paths related to the locale-IDs", false);
		I_ASSIGN_MULTI_0(m_localeIdsAttrPtr, "LocaleIds", "List of registered locale-IDs for corresponding help file paths", true);
	I_END_COMPONENT;

	// reimplemented (idoc::IHelpFileProvider)
	virtual QString GetHelpFilePath(const QString& contextText, const istd::IPolymorphic* contextObjectPtr) const override;

	// reimplemented (idoc::IHelpFileInfoProvider)
	virtual double GetHelpQuality(const QString& contextText, const istd::IPolymorphic* contextObjectPtr) const override;

private:
	QString GetPathForLocale(int languageIndex = -1) const;

private:
	I_REF(iprm::ISelectionParam, m_languageSelectionCompPtr);
	I_MULTIREF(ifile::IFileNameParam, m_helpUrlPathListCompPtr);
	I_MULTIREF(ifile::IFileNameParam, m_helpLocalPathListCompPtr);
	I_MULTIATTR(QByteArray, m_localeIdsAttrPtr);
};


} // namespace imtbase


