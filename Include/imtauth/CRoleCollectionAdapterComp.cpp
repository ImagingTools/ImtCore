// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleCollectionAdapterComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IRole.h>


namespace imtauth
{


// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CRoleCollectionAdapterComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	if (m_roleCollectionCompPtr.IsValid()){
		return *m_roleCollectionCompPtr;
	}

	return emptyCollection;
}


imtauth::IRoleUniquePtr CRoleCollectionAdapterComp::GetRole(const QByteArray& objectId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const IRole* rolePtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (rolePtr != nullptr){
				imtauth::IRoleUniquePtr roleInfoUniquePtr;
				roleInfoUniquePtr.MoveCastedPtr(rolePtr->CloneMe());

				return roleInfoUniquePtr;
			}
		}
	}

	return nullptr;
}


} // namespace imtauth


