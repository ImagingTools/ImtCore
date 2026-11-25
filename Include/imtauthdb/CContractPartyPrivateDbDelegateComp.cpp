// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CContractPartyPrivateDbDelegateComp.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CContractPartyPrivateDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::IContractPartyPrivateInfo> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CContractPartyPrivateDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::IContractPartyPrivateInfo* privateInfoPtr = dynamic_cast<const imtauth::IContractPartyPrivateInfo*>(&object);
	if (privateInfoPtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("ContractId"), IdLiteral(privateInfoPtr->GetContractId())});
	columns.append({QStringLiteral("TenantId"), IdLiteral(privateInfoPtr->GetTenantId())});
	columns.append({QStringLiteral("UpdatedAt"), TimeLiteral(privateInfoPtr->GetUpdatedAt())});

	return true;
}


} // namespace imtauthdb


