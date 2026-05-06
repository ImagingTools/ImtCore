// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/ITenantFilterStrategy.h>


namespace imtdb
{


/**
 * @brief Direct tenant filter strategy — filters by a TenantId column on the entity table.
 *
 * Use this strategy for entities that have a direct "TenantId" column
 * (e.g., tickets, projects, messages).
 *
 * Configuration:
 * - TenantIdColumn: Name of the column (default: "TenantId")
 *
 * Generated SQL when tenantId is present:
 * @code{.sql}
 * "TenantId"='<tenantId>'
 * @endcode
 *
 * Generated SQL when tenantId is empty (global scope):
 * @code{.sql}
 * ("TenantId" IS NULL OR "TenantId" = '')
 * @endcode
 */
class CDirectTenantFilterStrategyComp:
            public icomp::CComponentBase,
            virtual public imtdb::ITenantFilterStrategy
{
public:
    I_BEGIN_COMPONENT(CDirectTenantFilterStrategyComp)
        I_REGISTER_INTERFACE(imtdb::ITenantFilterStrategy)
        I_ASSIGN(m_tenantIdColumnAttrPtr, "TenantIdColumn", "Name of the TenantId column", false, "TenantId");
    I_END_COMPONENT

    // reimplemented (imtdb::ITenantFilterStrategy)
    virtual QString CreateTenantFilterSql(const QString& tenantId, const QString& tableAlias = QString()) const override;
    virtual QString CreateGlobalScopeFilterSql(const QString& tableAlias = QString()) const override;
    virtual bool RequiresJoin() const override;
    virtual QString CreateJoinClauseSql(const QString& tableAlias = QString()) const override;

protected:
    I_ATTR(QByteArray, m_tenantIdColumnAttrPtr);

private:
    QString qualifiedColumn(const QString& tableAlias) const;
};


/**
 * @brief Membership-based tenant filter strategy — filters through a join/membership table.
 *
 * Use this strategy for entities that don't have a direct TenantId column
 * but are associated with tenants through a separate membership/link table
 * (e.g., users → TenantMemberships).
 *
 * Configuration:
 * - MembershipTable: Name of the membership table (default: "TenantMemberships")
 * - EntityIdColumn: Column in membership table referencing the entity (default: "UserId")
 * - TenantIdColumn: Column in membership table referencing the tenant (default: "TenantId")
 * - ActiveColumn: Column for active status filtering (default: "IsActive")
 * - EntityTableIdColumn: Column in the main table that is referenced (default: "Id")
 *
 * Generated SQL when tenantId is present:
 * @code{.sql}
 * "Id" IN (SELECT "UserId" FROM "TenantMemberships" WHERE "TenantId"='<tenantId>' AND "IsActive"=true)
 * @endcode
 *
 * Generated SQL when tenantId is empty (global scope):
 * @code{.sql}
 * "Id" NOT IN (SELECT "UserId" FROM "TenantMemberships" WHERE "IsActive"=true)
 * @endcode
 */
class CMembershipTenantFilterStrategyComp:
            public icomp::CComponentBase,
            virtual public imtdb::ITenantFilterStrategy
{
public:
    I_BEGIN_COMPONENT(CMembershipTenantFilterStrategyComp)
        I_REGISTER_INTERFACE(imtdb::ITenantFilterStrategy)
        I_ASSIGN(m_membershipTableAttrPtr, "MembershipTable", "Name of the membership/join table", false, "TenantMemberships");
        I_ASSIGN(m_entityIdColumnAttrPtr, "EntityIdColumn", "Column in membership table referencing the entity", false, "UserId");
        I_ASSIGN(m_tenantIdColumnAttrPtr, "TenantIdColumn", "Column in membership table referencing the tenant", false, "TenantId");
        I_ASSIGN(m_activeColumnAttrPtr, "ActiveColumn", "Column for active status (empty to skip)", false, "IsActive");
        I_ASSIGN(m_entityTableIdColumnAttrPtr, "EntityTableIdColumn", "Column in the main entity table to match against", false, "Id");
    I_END_COMPONENT

    // reimplemented (imtdb::ITenantFilterStrategy)
    virtual QString CreateTenantFilterSql(const QString& tenantId, const QString& tableAlias = QString()) const override;
    virtual QString CreateGlobalScopeFilterSql(const QString& tableAlias = QString()) const override;
    virtual bool RequiresJoin() const override;
    virtual QString CreateJoinClauseSql(const QString& tableAlias = QString()) const override;

protected:
    I_ATTR(QByteArray, m_membershipTableAttrPtr);
    I_ATTR(QByteArray, m_entityIdColumnAttrPtr);
    I_ATTR(QByteArray, m_tenantIdColumnAttrPtr);
    I_ATTR(QByteArray, m_activeColumnAttrPtr);
    I_ATTR(QByteArray, m_entityTableIdColumnAttrPtr);

private:
    QString buildSubquery(const QString& tenantCondition, const QString& tableAlias) const;
};


/**
 * @brief Document-owner tenant filter strategy — filters documents by owner's tenant membership.
 *
 * Use this strategy for document-based entities (CSqlDatabaseDocumentDelegateComp)
 * where the tenant is determined indirectly through the document owner's tenant membership.
 *
 * The strategy uses a subquery that finds the OwnerId from the document's JSONB data
 * and checks if that owner belongs to the specified tenant.
 *
 * Configuration:
 * - OwnerIdJsonPath: JSON path to extract OwnerId from the document (default: "OwnerId")
 * - DocumentColumn: Name of the JSONB document column (default: "Document")
 * - MembershipTable: Table to check owner-tenant relationship (default: "TenantMemberships")
 *
 * Generated SQL when tenantId is present:
 * @code{.sql}
 * EXISTS (SELECT 1 FROM "TenantMemberships"
 *         WHERE "UserId" = (root."Document"->>'OwnerId')
 *         AND "TenantId"='<tenantId>' AND "IsActive"=true)
 * @endcode
 */
class CDocumentOwnerTenantFilterStrategyComp:
            public icomp::CComponentBase,
            virtual public imtdb::ITenantFilterStrategy
{
public:
    I_BEGIN_COMPONENT(CDocumentOwnerTenantFilterStrategyComp)
        I_REGISTER_INTERFACE(imtdb::ITenantFilterStrategy)
        I_ASSIGN(m_ownerIdJsonPathAttrPtr, "OwnerIdJsonPath", "JSON key to extract OwnerId from document", false, "OwnerId");
        I_ASSIGN(m_documentColumnAttrPtr, "DocumentColumn", "Name of the JSONB document column", false, "Document");
        I_ASSIGN(m_membershipTableAttrPtr, "MembershipTable", "Table for owner-tenant membership check", false, "TenantMemberships");
        I_ASSIGN(m_tenantIdColumnAttrPtr, "TenantIdColumn", "TenantId column in membership table", false, "TenantId");
        I_ASSIGN(m_activeColumnAttrPtr, "ActiveColumn", "Active status column in membership table", false, "IsActive");
    I_END_COMPONENT

    // reimplemented (imtdb::ITenantFilterStrategy)
    virtual QString CreateTenantFilterSql(const QString& tenantId, const QString& tableAlias = QString()) const override;
    virtual QString CreateGlobalScopeFilterSql(const QString& tableAlias = QString()) const override;
    virtual bool RequiresJoin() const override;
    virtual QString CreateJoinClauseSql(const QString& tableAlias = QString()) const override;

protected:
    I_ATTR(QByteArray, m_ownerIdJsonPathAttrPtr);
    I_ATTR(QByteArray, m_documentColumnAttrPtr);
    I_ATTR(QByteArray, m_membershipTableAttrPtr);
    I_ATTR(QByteArray, m_tenantIdColumnAttrPtr);
    I_ATTR(QByteArray, m_activeColumnAttrPtr);
};


} // namespace imtdb


