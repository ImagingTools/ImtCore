// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CContractEventDbDelegateComp.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CContractEventDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::IContractEventInfo> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CContractEventDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::IContractEventInfo* eventPtr = dynamic_cast<const imtauth::IContractEventInfo*>(&object);
	if (eventPtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("ContractId"), IdLiteral(eventPtr->GetContractId())});
	columns.append({QStringLiteral("TenantId"), IdLiteral(eventPtr->GetTenantId())});
	columns.append({QStringLiteral("Type"), NumberLiteral(int(eventPtr->GetEventType()))});
	columns.append({QStringLiteral("Time"), TimeLiteral(eventPtr->GetTime())});

	return true;
}


} // namespace imtauthdb


