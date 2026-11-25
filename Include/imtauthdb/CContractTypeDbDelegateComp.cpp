// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CContractTypeDbDelegateComp.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CContractTypeDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::IContractTypeInfo> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CContractTypeDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::IContractTypeInfo* typePtr = dynamic_cast<const imtauth::IContractTypeInfo*>(&object);
	if (typePtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("OwnerTenantId"), IdLiteral(typePtr->GetOwnerTenantId())});
	columns.append({QStringLiteral("Name"), TextLiteral(typePtr->GetName())});
	columns.append({QStringLiteral("IsArchived"), NumberLiteral(typePtr->IsArchived() ? 1 : 0)});

	return true;
}


} // namespace imtauthdb


