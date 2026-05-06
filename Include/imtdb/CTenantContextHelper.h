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


namespace imtdb
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
 *     // For entities with direct TenantId column:
 *     imtdb::CTenantContextHelper::InjectDirectFilter(gqlRequest, filterParamsPtr);
 *
 *     // For entities linked via membership table:
 *     imtdb::CTenantContextHelper::InjectMembershipFilter(gqlRequest, filterParamsPtr);
 *
 *     // For document entities:
 *     imtdb::CTenantContextHelper::InjectDocumentOwnerFilter(gqlRequest, filterParamsPtr);
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
                const QByteArray& tenantIdColumn = "TenantId",
                const QByteArray& paramKey = QByteArray(),
                bool skipForAdmin = true);

    /**
     * @brief Injects a membership-based tenant filter.
     */
    static bool InjectMembershipFilter(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& entityIdColumn = "UserId",
                const QByteArray& tenantIdColumn = "TenantId",
                const QByteArray& activeColumn = "IsActive",
                const QByteArray& paramKey = QByteArray(),
                bool skipForAdmin = true);

    /**
     * @brief Injects a document-owner tenant filter.
     */
    static bool InjectDocumentOwnerFilter(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& ownerIdJsonPath = "OwnerId",
                const QByteArray& documentColumn = "Document",
                const QByteArray& paramKey = QByteArray(),
                bool skipForAdmin = true);

    /**
     * @brief Injects a pre-configured CTenantFilterParam into the ParamsSet.
     */
    static bool InjectFilterParam(
                const imtgql::CGqlRequest& gqlRequest,
                iprm::CParamsSet* filterParamsPtr,
                CTenantFilterParam* filterParamPtr,
                const QByteArray& paramKey = QByteArray(),
                bool skipForAdmin = true);

private:
    static QByteArray ExtractTenantId(const imtgql::CGqlRequest& gqlRequest);
    static bool ShouldSkip(const imtgql::CGqlRequest& gqlRequest, bool skipForAdmin);
};


} // namespace imtdb


