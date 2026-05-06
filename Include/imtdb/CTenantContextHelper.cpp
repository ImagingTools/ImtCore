// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantContextHelper.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtdb
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


bool CTenantContextHelper::ShouldSkip(const imtgql::CGqlRequest& gqlRequest, bool skipForAdmin)
{
    if (!skipForAdmin){
        return false;
    }

    const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
    if (gqlContextPtr == nullptr){
        return false;
    }

    const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
    if (userInfoPtr != nullptr && userInfoPtr->IsAdmin()){
        return true;
    }

    return false;
}


// --- Public API ---

bool CTenantContextHelper::InjectDirectFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& tenantIdColumn,
            const QByteArray& paramKey,
            bool skipForAdmin)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (ShouldSkip(gqlRequest, skipForAdmin)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    CTenantFilterParam* paramPtr = CTenantFilterParam::CreateDirect(tenantId, tenantIdColumn);

    QByteArray key = paramKey.isEmpty() ? DefaultParamKey : paramKey;
    filterParamsPtr->SetEditableParameter(key, paramPtr, true);

    return true;
}


bool CTenantContextHelper::InjectMembershipFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& membershipTable,
            const QByteArray& entityIdColumn,
            const QByteArray& tenantIdColumn,
            const QByteArray& activeColumn,
            const QByteArray& paramKey,
            bool skipForAdmin)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (ShouldSkip(gqlRequest, skipForAdmin)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    CTenantFilterParam* paramPtr = CTenantFilterParam::CreateMembership(
                tenantId, membershipTable, entityIdColumn, tenantIdColumn, activeColumn);

    QByteArray key = paramKey.isEmpty() ? DefaultParamKey : paramKey;
    filterParamsPtr->SetEditableParameter(key, paramPtr, true);

    return true;
}


bool CTenantContextHelper::InjectDocumentOwnerFilter(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            const QByteArray& membershipTable,
            const QByteArray& ownerIdJsonPath,
            const QByteArray& documentColumn,
            const QByteArray& paramKey,
            bool skipForAdmin)
{
    if (filterParamsPtr == nullptr){
        return false;
    }

    if (ShouldSkip(gqlRequest, skipForAdmin)){
        return false;
    }

    QByteArray tenantId = ExtractTenantId(gqlRequest);

    CTenantFilterParam* paramPtr = CTenantFilterParam::CreateDocumentOwner(
                tenantId, membershipTable, ownerIdJsonPath, documentColumn);

    QByteArray key = paramKey.isEmpty() ? DefaultParamKey : paramKey;
    filterParamsPtr->SetEditableParameter(key, paramPtr, true);

    return true;
}


bool CTenantContextHelper::InjectFilterParam(
            const imtgql::CGqlRequest& gqlRequest,
            iprm::CParamsSet* filterParamsPtr,
            CTenantFilterParam* filterParamPtr,
            const QByteArray& paramKey,
            bool skipForAdmin)
{
    if (filterParamsPtr == nullptr || filterParamPtr == nullptr){
        return false;
    }

    if (ShouldSkip(gqlRequest, skipForAdmin)){
        delete filterParamPtr;
        return false;
    }

    // Fill tenantId from context if not already set
    if (filterParamPtr->GetTenantId().isEmpty()){
        QByteArray tenantId = ExtractTenantId(gqlRequest);
        filterParamPtr->SetTenantId(tenantId);
    }

    QByteArray key = paramKey.isEmpty() ? DefaultParamKey : paramKey;
    filterParamsPtr->SetEditableParameter(key, filterParamPtr, true);

    return true;
}


} // namespace imtdb


