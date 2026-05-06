// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantContextHelper.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtdb
{


bool CTenantContextHelper::InjectTenantId(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& paramKey,
            bool skipForAdmin)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
    if (gqlContextPtr == nullptr){
        return false;
    }

    // Skip for admins if configured
    if (skipForAdmin){
        const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
        if (userInfoPtr != nullptr && userInfoPtr->IsAdmin()){
            return false;
        }
    }

    QByteArray tenantId = gqlContextPtr->GetTenantId();

    iprm::CIdParam* tenantIdParamPtr = new iprm::CIdParam();
    tenantIdParamPtr->SetId(tenantId);
    filterParamsPtr->SetEditableParameter(paramKey, tenantIdParamPtr, true);

    return true;
}


} // namespace imtdb


