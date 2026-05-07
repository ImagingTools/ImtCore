// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtdb
{


/**
 * @brief Serializable model carrying all information needed for multi-tenant data filtering.
 *
 * This interface defines the data contract for tenant filter information that is
 * passed through the ParamsSet to database delegates. Each delegate reads this
 * model and constructs its own SQL filter based on the provided information.
 *
 * @section usage Usage
 *
 * 1. Controller creates CTenantFilterParam via factory and places it into ParamsSet
 * 2. The param travels to the delegate layer
 * 3. Each delegate reads the param and constructs its own SQL filter
 *
 * @code{.cpp}
 * // In delegate's CreateAdditionalFiltersQuery:
 * iprm::TParamsPtr<imtdb::ITenantFilterParam> tenantParam(&filterParams, "TenantFilter");
 * if (tenantParam.IsValid()) {
 *     QByteArray tenantId = tenantParam->GetTenantId();
 *     // Build SQL based on GetFilterType()...
 * }
 * @endcode
 */
class ITenantFilterParam: virtual public iser::ISerializable
{
public:
    /**
     * @brief Enum for the type of tenant filtering to apply.
     */
    enum FilterType
    {
        /** Entity has a direct TenantId column */
        FT_DIRECT = 0,

        /** Entity is linked to tenant through a membership/join table */
        FT_MEMBERSHIP = 1,

        /** Document entity where tenant is determined via owner's membership */
        FT_DOCUMENT_OWNER = 2
    };

    /**
     * @brief Gets the tenant ID to filter by. Empty means global scope.
     */
    virtual QByteArray GetTenantId() const = 0;

    /**
     * @brief Gets the filter type.
     */
    virtual FilterType GetFilterType() const = 0;

    /**
     * @brief Gets the membership/join table name (for FT_MEMBERSHIP and FT_DOCUMENT_OWNER).
     */
    virtual QByteArray GetMembershipTable() const = 0;

    /**
     * @brief Gets the entity ID column in the membership table (for FT_MEMBERSHIP).
     */
    virtual QByteArray GetEntityIdColumn() const = 0;

    /**
     * @brief Gets the tenant ID column in the membership table.
     */
    virtual QByteArray GetTenantIdColumn() const = 0;

    /**
     * @brief Gets the active status column in the membership table (empty to skip).
     */
    virtual QByteArray GetActiveColumn() const = 0;

    /**
     * @brief Gets the direct TenantId column name on the entity table (for FT_DIRECT).
     */
    virtual QByteArray GetDirectTenantIdColumn() const = 0;

    /**
     * @brief Gets the owner ID JSON path for document-based filtering (for FT_DOCUMENT_OWNER).
     */
    virtual QByteArray GetOwnerIdJsonPath() const = 0;

    /**
     * @brief Gets the document JSONB column name (for FT_DOCUMENT_OWNER).
     */
    virtual QByteArray GetDocumentColumn() const = 0;
};


} // namespace imtdb


