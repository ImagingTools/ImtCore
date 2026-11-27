// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistdb/CAssistantConfigurationDbDelegateComp.h>


namespace imtassistdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CAssistantConfigurationDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtassist::IAssistantConfiguration> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CAssistantConfigurationDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtassist::IAssistantConfiguration* configurationPtr = dynamic_cast<const imtassist::IAssistantConfiguration*>(&object);
	if (configurationPtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("TenantId"), IdLiteral(configurationPtr->GetTenantId())});
	columns.append({QStringLiteral("IsEnabled"), NumberLiteral(configurationPtr->IsEnabled() ? 1 : 0)});
	columns.append({QStringLiteral("UpdatedAt"), TimeLiteral(configurationPtr->GetUpdatedAt())});

	return true;
}


} // namespace imtassistdb


