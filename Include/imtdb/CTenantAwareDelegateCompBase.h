// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtdb/ITenantFilterStrategy.h>


namespace imtdb
{


/**
 * @brief Base class for tenant-aware database delegates.
 *
 * Extends CSqlDatabaseObjectDelegateCompBase with automatic multi-tenant
 * data filtering. The filtering is driven by a pluggable ITenantFilterStrategy
 * and a TenantId parameter passed through the filter params.
 *
 * @section usage How to use
 *
 * 1. Create a delegate that inherits CTenantAwareDelegateCompBase instead of
 *    CSqlDatabaseObjectDelegateCompBase.
 * 2. Wire a tenant filter strategy component (Direct, Membership, or DocumentOwner)
 *    to the "TenantFilterStrategy" slot.
 * 3. The controller passes TenantId from the GQL context via a "TenantId" parameter
 *    in the filter params (use CTenantContextFilterComp or manually add CIdParam).
 *
 * The delegate will automatically inject the tenant filter into every query's
 * WHERE clause via CreateAdditionalFiltersQuery().
 *
 * @section behavior Filtering behavior
 *
 * - If TenantId is present in filterParams → only records for that tenant are returned
 * - If TenantId is empty or absent → global-scope records (no tenant association) are returned
 * - If no strategy is configured (optional ref) → no filtering is applied (backward compatible)
 *
 * @section performance Performance
 *
 * The Direct strategy uses a simple equality check (leverages B-tree index on TenantId).
 * The Membership strategy uses EXISTS/IN subquery (ensure composite index on
 * (TenantId, UserId, IsActive) in the membership table).
 * The DocumentOwner strategy uses EXISTS with a correlated subquery (ensure index on
 * UserId in the membership table).
 *
 * @code{.cpp}
 * class CMyEntityDbDelegateComp: public imtdb::CTenantAwareDelegateCompBase
 * {
 *     I_BEGIN_COMPONENT(CMyEntityDbDelegateComp)
 *         I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectDelegate)
 *         // ... standard delegate config ...
 *         // Strategy will be wired via ACC
 *     I_END_COMPONENT
 * };
 * @endcode
 */
class CTenantAwareDelegateCompBase: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
    typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

    I_BEGIN_BASE_COMPONENT(CTenantAwareDelegateCompBase)
        I_ASSIGN(m_tenantFilterStrategyCompPtr, "TenantFilterStrategy", "Tenant filter strategy (Direct, Membership, or DocumentOwner)", false, "TenantFilterStrategy");
        I_ASSIGN(m_tenantParamKeyAttrPtr, "TenantParamKey", "Key name for TenantId in filter params", false, "TenantId");
    I_END_COMPONENT

protected:
    /**
     * @brief Overrides the additional filters hook to inject tenant filtering.
     *
     * If a subclass also overrides CreateAdditionalFiltersQuery, it should call
     * this base implementation and combine the results:
     * @code{.cpp}
     * QString MyDelegate::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
     * {
     *     QString tenantFilter = CTenantAwareDelegateCompBase::CreateAdditionalFiltersQuery(filterParams);
     *     QString myFilter = ...; // custom logic
     *     if (!tenantFilter.isEmpty() && !myFilter.isEmpty())
     *         return tenantFilter + " AND " + myFilter;
     *     return tenantFilter + myFilter;
     * }
     * @endcode
     */
    virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

protected:
    I_REF(imtdb::ITenantFilterStrategy, m_tenantFilterStrategyCompPtr);
    I_ATTR(QByteArray, m_tenantParamKeyAttrPtr);
};


} // namespace imtdb


