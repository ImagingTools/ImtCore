// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CApplicationInfoDelegatorComp.h>


// ACF includes
#include <iser/CMinimalVersionInfo.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IApplicationInfoController)

void CApplicationInfoDelegatorComp::SetApplicationAttribute(ApplicationAttribute /*attributeId*/, const QString& /*attribute*/)
{
}


// reimplemented (ibase::IApplicationInfo)

QString CApplicationInfoDelegatorComp::GetApplicationAttribute(int attributeId, bool allowTranslation) const
{
	if (m_applicationInfoCompPtr.IsValid()){
		return m_applicationInfoCompPtr->GetApplicationAttribute(attributeId, allowTranslation);
	}

	return QString();
}


int CApplicationInfoDelegatorComp::GetMainVersionId() const
{
	if (m_applicationInfoCompPtr.IsValid()){
		return m_applicationInfoCompPtr->GetMainVersionId();
	}

	return -1;
}


const iser::IVersionInfo& CApplicationInfoDelegatorComp::GetVersionInfo() const
{
	if (m_applicationInfoCompPtr.IsValid()){
		return m_applicationInfoCompPtr->GetVersionInfo();
	}

	static iser::CMinimalVersionInfo versionInfo(nullptr);

	return versionInfo;
}


} // namespace imtbase


