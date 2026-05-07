// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtdb/CTenantFilterParam.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtservergql
{


/**
 * @brief Helper class for injecting tenant filter information into the ParamsSet.
 *
 * This utility extracts TenantId from the GQL request context (JWT/session) and
 * creates a CTenantFilterParam model with the appropriate configuration, then
 * places it into the ParamsSet for downstream database delegates.
 *
 * @section usage Usage in a collection controller
 *
 * @code{.cpp}
 * void MyCollectionController::SetAdditionalFilters(
 *         const imtgql::CGqlRequest& gqlRequest,
 *         const imtgql::CGqlParamObject& viewParams,
 *         iprm::CParamsSet* filterParamsPtr) const
 * {
 *     imtservergql::CTenantContextHelper::InjectDirectFilter(gqlRequest, filterParamsPtr);
 * }
 * @endcode
 */
class CTenantContextHelper
{
public:
    /**
     * @brief Default key for the tenant filter param in ParamsSet.
     */
    static const QByteArray DefaultParamKey;

    /**
     * @brief Injects a direct tenant filter (entity has TenantId column).
     */
    static bool InjectDirectFilter(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& tenantIdColumn = "TenantId");

    /**
     * @brief Injects a membership-based tenant filter.
     */
    static bool InjectMembershipFilter(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& entityIdColumn = "UserId",
                const QByteArray& tenantIdColumn = "TenantId",
                const QByteArray& activeColumn = "IsActive");

    /**
     * @brief Injects a document-owner tenant filter.
     */
    static bool InjectDocumentOwnerFilter(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& ownerIdJsonPath = "OwnerId",
                const QByteArray& documentColumn = "Document");

private:
    static QByteArray ExtractTenantId(const imtgql::CGqlRequest& gqlRequest);
    static bool IsAdmin(const imtgql::CGqlRequest& gqlRequest);
};


} // namespace imtservergql


