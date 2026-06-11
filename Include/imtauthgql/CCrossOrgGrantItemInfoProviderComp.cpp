// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantItemInfoProviderComp.h>


// ACF includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauthgql
{


// reimplemented (ISelectableItemInfoProvider)

bool CCrossOrgGrantItemInfoProviderComp::GetItemParameters(
	const QByteArray& objectId,
	iprm::IParamsSet& paramsSet) const
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
		QString targetName;
		if (m_tenantCollectionCompPtr.IsValid()){
			targetName = m_tenantCollectionCompPtr->GetElementInfo(grantInfo.targetTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}

		iprm::CTextParam* targetParamPtr = new iprm::CTextParam;
		targetParamPtr->SetText(targetName.isEmpty() ? QString::fromUtf8(grantInfo.targetTenantId) : targetName);
		paramsSet.SetEditableParameter("targetTenant", targetParamPtr, true);
	}

	// Roles
	if (!grantInfo.roleIds.isEmpty()){
		QStringList roleNames;
		for (const QByteArray& roleId : grantInfo.roleIds){
			QString roleName;
			if (m_roleCollectionCompPtr.IsValid()){
				roleName = m_roleCollectionCompPtr->GetElementInfo(roleId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
			roleNames.append(roleName.isEmpty() ? QString::fromUtf8(roleId) : roleName);
		}

		iprm::CTextParam* rolesParamPtr = new iprm::CTextParam;
		rolesParamPtr->SetText(roleNames.join(QStringLiteral(", ")));
		paramsSet.SetEditableParameter("roles", rolesParamPtr, true);
	}

	// Expiry
	if (!grantInfo.expiresAt.isEmpty()){
		iprm::CTextParam* expiryParamPtr = new iprm::CTextParam;
		expiryParamPtr->SetText(grantInfo.expiresAt);
		paramsSet.SetEditableParameter("expiresAt", expiryParamPtr, true);
	}

	return true;
}


} // namespace imtauthgql
