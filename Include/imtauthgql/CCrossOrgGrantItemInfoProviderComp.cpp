// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossOrgGrantItemInfoProviderComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauthgql
{


// reimplemented (IObjectParamsFiller)

bool CCrossOrgGrantItemInfoProviderComp::FillParams(
	const QByteArray& objectId,
	iprm::IParamsSet& paramsSet,
	const QByteArray& contextTenantId) const
{
	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr == nullptr){
		return false;
	}

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

	// Determine the "other" tenant: if context is the source, show target; if context is target, show source
	QByteArray otherTenantId;
	QString direction;
	if (!contextTenantId.isEmpty() && grantInfo.targetTenantId == contextTenantId){
		// Current tenant is the target (recipient) — show who granted
		otherTenantId = grantInfo.sourceTenantId;
		direction = QStringLiteral("from");
	}
	else{
		// Current tenant is the source (grantor) — show who receives
		otherTenantId = grantInfo.targetTenantId;
		direction = QStringLiteral("to");
	}

	// Target/source tenant name
	if (!otherTenantId.isEmpty()){
		QString tenantName;
		if (m_tenantCollectionCompPtr.IsValid()){
			tenantName = m_tenantCollectionCompPtr->GetElementInfo(otherTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
		}

		iprm::CTextParam* targetParamPtr = new iprm::CTextParam;
		targetParamPtr->SetText(tenantName.isEmpty() ? QString::fromUtf8(otherTenantId) : tenantName);
		paramsSetPtr->SetEditableParameter("targetTenant", targetParamPtr, true);
	}

	// Direction (from/to)
	{
		iprm::CTextParam* dirParamPtr = new iprm::CTextParam;
		dirParamPtr->SetText(direction);
		paramsSetPtr->SetEditableParameter("direction", dirParamPtr, true);
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
		paramsSetPtr->SetEditableParameter("roles", rolesParamPtr, true);
	}

	// Expiry
	if (!grantInfo.expiresAt.isEmpty()){
		iprm::CTextParam* expiryParamPtr = new iprm::CTextParam;
		expiryParamPtr->SetText(grantInfo.expiresAt);
		paramsSetPtr->SetEditableParameter("expiresAt", expiryParamPtr, true);
	}

	return true;
}


} // namespace imtauthgql
