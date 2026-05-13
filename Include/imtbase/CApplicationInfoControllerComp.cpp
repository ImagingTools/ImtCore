// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CApplicationInfoControllerComp.h>


// ACF includes
#include <icomm/CCommonVersionInfo.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::IApplicationInfoController)

void CApplicationInfoControllerComp::SetApplicationAttribute(ApplicationAttribute attributeId, const QString& attribute)
{
	istd::CChangeNotifier changeNotifier(this);

	m_attributes[attributeId] = attribute;
}


// reimplemented (ibase::IApplicationInfo)

QString CApplicationInfoControllerComp::GetApplicationAttribute(int attributeId, bool /*allowTranslation*/) const
{
	if (m_attributes.contains(attributeId)){
		return m_attributes[attributeId];
	}

	return QString();
}


int CApplicationInfoControllerComp::GetMainVersionId() const
{
	return -1;
}


const iser::IVersionInfo& CApplicationInfoControllerComp::GetVersionInfo() const
{
	if (m_versionInfoCompPtr.IsValid()){
		return *m_versionInfoCompPtr;
	}

	static icomm::CCommonVersionInfo commonVersion;

	return commonVersion;
}


} // namespace imtbase


