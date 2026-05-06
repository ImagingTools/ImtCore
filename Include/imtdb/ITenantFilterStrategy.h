// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>


namespace imtdb
{


/**
 * @brief Strategy interface for multi-tenant data filtering.
 *
 * Implementations of this interface provide the SQL WHERE clause fragment
 * that restricts query results to a specific tenant context. The strategy
 * pattern allows different filtering approaches depending on how a given
 * entity relates to tenants:
 *
 * - **Direct**: Entity has a TenantId column (e.g., tickets, projects)
 * - **Membership**: Entity is linked through a membership/join table (e.g., users → TenantMemberships)
 * - **DocumentOwner**: Document-based entity where tenant is determined by the owner's tenant
 *
 * @section usage Usage
 *
 * Strategies are registered as components and referenced by tenant-aware delegates.
 * The delegate calls CreateTenantFilterSql() during query building and appends
 * the result to the WHERE clause.
 *
 * @code{.cpp}
 * QString filter = strategyPtr->CreateTenantFilterSql(tenantId, "root");
 * // Direct: root."TenantId"='abc-123'
 * // Membership: root."Id" IN (SELECT "UserId" FROM "TenantMemberships" WHERE "TenantId"='abc-123' AND "IsActive"=true)
 * @endcode
 */
class ITenantFilterStrategy: virtual public iser::IObject
{
public:
    /**
     * @brief Creates an SQL WHERE clause fragment for tenant filtering.
     *
     * @param tenantId The tenant ID to filter by (already SQL-escaped by caller).
     * @param tableAlias The alias of the main table in the query (e.g., "root").
     * @return SQL fragment to be AND-ed into the WHERE clause, or empty string if no filter needed.
     */
    virtual QString CreateTenantFilterSql(const QString& tenantId, const QString& tableAlias = QString()) const = 0;

    /**
     * @brief Creates an SQL WHERE clause fragment for "no tenant" (global scope) filtering.
     *
     * When tenantId is empty, this returns a condition that selects only records
     * without any tenant association.
     *
     * @param tableAlias The alias of the main table in the query.
     * @return SQL fragment for global-scope filtering, or empty string if not applicable.
     */
    virtual QString CreateGlobalScopeFilterSql(const QString& tableAlias = QString()) const = 0;

    /**
     * @brief Returns whether this strategy requires a JOIN clause.
     * @return true if CreateJoinClauseSql() returns a non-empty string.
     */
    virtual bool RequiresJoin() const = 0;

    /**
     * @brief Returns an optional JOIN clause needed for the filtering.
     * @param tableAlias The alias of the main table.
     * @return SQL JOIN clause, or empty string.
     */
    virtual QString CreateJoinClauseSql(const QString& tableAlias = QString()) const = 0;
};


} // namespace imtdb


