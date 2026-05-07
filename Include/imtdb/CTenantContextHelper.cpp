// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantContextHelper.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtservergql
{


const QByteArray CTenantContextHelper::DefaultParamKey = "TenantFilter";


// --- Private helpers ---

QByteArray CTenantContextHelper::ExtractTenantId(const imtgql::CGqlRequest& gqlRequest)
{
    const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
    if (gqlContextPtr == nullptr){
        return QByteArray();
    }
    return gqlContextPtr->GetTenantId();
}


bool CTenantContextHelper::IsAdmin(const imtgql::CGqlRequest& gqlRequest)
{
    const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
    if (gqlContextPtr == nullptr){
        return false;
    }

    const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
    return (userInfoPtr != nullptr && userInfoPtr->IsAdmin());
}


// --- Public API ---

bool CTenantContextHelper::InjectDirectFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& tenantIdColumn)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (IsAdmin(gqlRequest)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    imtdb::CTenantFilterParam* paramPtr = imtdb::CTenantFilterParam::CreateDirect(tenantId, tenantIdColumn);
    filterParamsPtr->SetEditableParameter(DefaultParamKey, paramPtr, true);

    return true;
}


bool CTenantContextHelper::InjectMembershipFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& membershipTable,
            const QByteArray& entityIdColumn,
            const QByteArray& tenantIdColumn,
            const QByteArray& activeColumn)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (IsAdmin(gqlRequest)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    imtdb::CTenantFilterParam* paramPtr = imtdb::CTenantFilterParam::CreateMembership(
                tenantId, membershipTable, entityIdColumn, tenantIdColumn, activeColumn);
    filterParamsPtr->SetEditableParameter(DefaultParamKey, paramPtr, true);

    return true;
}


bool CTenantContextHelper::InjectDocumentOwnerFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& membershipTable,
            const QByteArray& ownerIdJsonPath,
            const QByteArray& documentColumn)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (IsAdmin(gqlRequest)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    imtdb::CTenantFilterParam* paramPtr = imtdb::CTenantFilterParam::CreateDocumentOwner(
                tenantId, membershipTable, ownerIdJsonPath, documentColumn);
    filterParamsPtr->SetEditableParameter(DefaultParamKey, paramPtr, true);

    return true;
}


} // namespace imtservergql


