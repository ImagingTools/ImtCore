// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtdb
{


/**
 * @brief Helper class for injecting TenantId from GQL context into filter params.
 *
 * This is a utility class (not a component) that controllers can use in their
 * SetAdditionalFilters() override to automatically propagate the tenant context
 * from the GQL request to the database delegate layer.
 *
 * @section usage Usage in a collection controller
 *
 * @code{.cpp}
 * void MyCollectionController::SetAdditionalFilters(
 *         const imtgql::CGqlRequest& gqlRequest,
 *         const imtgql::CGqlParamObject& viewParams,
 *         iprm::CParamsSet* filterParamsPtr) const
 * {
 *     // Inject TenantId from GQL context
 *     imtdb::CTenantContextHelper::InjectTenantId(gqlRequest, filterParamsPtr);
 *     
 *     // Additional custom filters if needed...
 * }
 * @endcode
 *
 * @section behavior Behavior
 *
 * - Extracts TenantId from IGqlContext (set from JWT/session/headers)
 * - Creates an IIdParam with key "TenantId" in the filterParamsPtr
 * - If TenantId is empty in context, the param is still set (empty) to signal global scope
 * - Delegates using CTenantAwareDelegateCompBase will automatically use this param
 *
 * @section admin Admin bypass
 *
 * By default, admin users (IUserInfo::IsAdmin()) skip tenant filtering.
 * Set skipForAdmin=false to enforce filtering even for admins.
 */
class CTenantContextHelper
{
public:
    /**
     * @brief Injects TenantId from GQL context into filter params.
     *
     * @param gqlRequest The GQL request containing the context with TenantId.
     * @param filterParamsPtr Target params set to inject the TenantId param into.
     * @param paramKey Key name for the parameter (default: "TenantId").
     * @param skipForAdmin If true, admin users won't have the filter applied.
     * @return true if TenantId was successfully injected, false if skipped.
     */
    static bool InjectTenantId(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& paramKey = "TenantId",
                bool skipForAdmin = true);
};


} // namespace imtdb


