// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantItemInfoProviderComp.h>


// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtbase/ICollectionInfo.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imtauthgql
{


// reimplemented (ISelectableItemInfoProvider)

bool CCrossOrgGrantItemInfoProviderComp::GetItemParameters(
	const QByteArray& objectId,
	imtsdl::TElementList<sdl::V1_0::imtbase::CParameter>& parameters) const
{
	if (!m_grantCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_grantCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		return false;
	}

	const imtauth::ICrossOrgGrantData* grantDataPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(dataPtr.GetPtr());
	if (grantDataPtr == nullptr){
		return false;
	}

	imtauth::CrossOrgGrantInfo grantInfo = grantDataPtr->GetGrantInfo();

	// Target tenant name
	if (!grantInfo.targetTenantId.isEmpty()){
		sdl::V1_0::imtbase::CParameter targetParam;
		targetParam.id = "targetTenant";
		targetParam.name = QStringLiteral("Target");

		QString targetName;
		if (m_tenantCollectionCompPtr.IsValid()){
			targetName = m_tenantCollectionCompPtr->GetElementInfo(grantInfo.targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}
		targetParam.data = targetName.isEmpty() ? QString::fromUtf8(grantInfo.targetTenantId) : targetName;
		parameters << targetParam;
	}

	// Roles
	if (!grantInfo.roleIds.isEmpty()){
		sdl::V1_0::imtbase::CParameter rolesParam;
		rolesParam.id = "roles";
		rolesParam.name = QStringLiteral("Roles");

		QStringList roleNames;
		for (const QByteArray& roleId : grantInfo.roleIds){
			QString roleName;
			if (m_roleCollectionCompPtr.IsValid()){
				roleName = m_roleCollectionCompPtr->GetElementInfo(roleId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
			roleNames.append(roleName.isEmpty() ? QString::fromUtf8(roleId) : roleName);
		}
		rolesParam.data = roleNames.join(QStringLiteral(", "));
		parameters << rolesParam;
	}

	// Expiry
	if (!grantInfo.expiresAt.isEmpty()){
		sdl::V1_0::imtbase::CParameter expiryParam;
		expiryParam.id = "expiresAt";
		expiryParam.name = QStringLiteral("Expires");
		expiryParam.data = grantInfo.expiresAt;
		parameters << expiryParam;
	}

	return true;
}


} // namespace imtauthgql
