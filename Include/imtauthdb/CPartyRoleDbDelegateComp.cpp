// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CPartyRoleDbDelegateComp.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CPartyRoleDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::IPartyRoleInfo> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CPartyRoleDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::IPartyRoleInfo* rolePtr = dynamic_cast<const imtauth::IPartyRoleInfo*>(&object);
	if (rolePtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("OwnerTenantId"), IdLiteral(rolePtr->GetOwnerTenantId())});
	columns.append({QStringLiteral("Name"), TextLiteral(rolePtr->GetName())});
	columns.append({QStringLiteral("IsArchived"), NumberLiteral(rolePtr->IsArchived() ? 1 : 0)});

	return true;
}


} // namespace imtauthdb


