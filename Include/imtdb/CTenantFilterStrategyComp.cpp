// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantFilterStrategyComp.h>


// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdb
{


// --- CDirectTenantFilterStrategyComp ---

QString CDirectTenantFilterStrategyComp::qualifiedColumn(const QString& tableAlias) const
{
    QString column = QString::fromUtf8(*m_tenantIdColumnAttrPtr);
    if (tableAlias.isEmpty()){
        return QStringLiteral("\"%1\"").arg(column);
    }
    return QStringLiteral("%1.\"%2\"").arg(tableAlias, column);
}


QString CDirectTenantFilterStrategyComp::CreateTenantFilterSql(
            const QString& tenantId,
            const QString& tableAlias) const
{
    if (tenantId.isEmpty()){
        return CreateGlobalScopeFilterSql(tableAlias);
    }

    return QStringLiteral("%1='%2'")
        .arg(qualifiedColumn(tableAlias), imtdb::EscapeSql(tenantId));
}


QString CDirectTenantFilterStrategyComp::CreateGlobalScopeFilterSql(const QString& tableAlias) const
{
    QString col = qualifiedColumn(tableAlias);
    return QStringLiteral("(%1 IS NULL OR %1 = '')").arg(col);
}


bool CDirectTenantFilterStrategyComp::RequiresJoin() const
{
    return false;
}


QString CDirectTenantFilterStrategyComp::CreateJoinClauseSql(const QString& /*tableAlias*/) const
{
    return QString();
}


// --- CMembershipTenantFilterStrategyComp ---

QString CMembershipTenantFilterStrategyComp::buildSubquery(
            const QString& tenantCondition,
            const QString& tableAlias) const
{
    QString membershipTable = QString::fromUtf8(*m_membershipTableAttrPtr);
    QString entityIdCol = QString::fromUtf8(*m_entityIdColumnAttrPtr);
    QString entityTableIdCol = QString::fromUtf8(*m_entityTableIdColumnAttrPtr);
    QString activeCol = QString::fromUtf8(*m_activeColumnAttrPtr);

    QString entityRef;
    if (tableAlias.isEmpty()){
        entityRef = QStringLiteral("\"%1\"").arg(entityTableIdCol);
    } else {
        entityRef = QStringLiteral("%1.\"%2\"").arg(tableAlias, entityTableIdCol);
    }

    QString activeCondition;
    if (!activeCol.isEmpty()){
        activeCondition = QStringLiteral(" AND \"%1\"=true").arg(activeCol);
    }

    return QStringLiteral("%1 IN (SELECT \"%2\" FROM \"%3\" WHERE %4%5)")
        .arg(entityRef, entityIdCol, membershipTable, tenantCondition, activeCondition);
}


QString CMembershipTenantFilterStrategyComp::CreateTenantFilterSql(
            const QString& tenantId,
            const QString& tableAlias) const
{
    if (tenantId.isEmpty()){
        return CreateGlobalScopeFilterSql(tableAlias);
    }

    QString tenantIdCol = QString::fromUtf8(*m_tenantIdColumnAttrPtr);
    QString tenantCondition = QStringLiteral("\"%1\"='%2'")
        .arg(tenantIdCol, imtdb::EscapeSql(tenantId));

    return buildSubquery(tenantCondition, tableAlias);
}


QString CMembershipTenantFilterStrategyComp::CreateGlobalScopeFilterSql(const QString& tableAlias) const
{
    QString membershipTable = QString::fromUtf8(*m_membershipTableAttrPtr);
    QString entityIdCol = QString::fromUtf8(*m_entityIdColumnAttrPtr);
    QString entityTableIdCol = QString::fromUtf8(*m_entityTableIdColumnAttrPtr);
    QString activeCol = QString::fromUtf8(*m_activeColumnAttrPtr);

    QString entityRef;
    if (tableAlias.isEmpty()){
        entityRef = QStringLiteral("\"%1\"").arg(entityTableIdCol);
    } else {
        entityRef = QStringLiteral("%1.\"%2\"").arg(tableAlias, entityTableIdCol);
    }

    QString activeCondition;
    if (!activeCol.isEmpty()){
        activeCondition = QStringLiteral(" WHERE \"%1\"=true").arg(activeCol);
    }

    return QStringLiteral("%1 NOT IN (SELECT \"%2\" FROM \"%3\"%4)")
        .arg(entityRef, entityIdCol, membershipTable, activeCondition);
}


bool CMembershipTenantFilterStrategyComp::RequiresJoin() const
{
    return false;
}


QString CMembershipTenantFilterStrategyComp::CreateJoinClauseSql(const QString& /*tableAlias*/) const
{
    return QString();
}


// --- CDocumentOwnerTenantFilterStrategyComp ---

QString CDocumentOwnerTenantFilterStrategyComp::CreateTenantFilterSql(
            const QString& tenantId,
            const QString& tableAlias) const
{
    if (tenantId.isEmpty()){
        return CreateGlobalScopeFilterSql(tableAlias);
    }

    QString ownerIdPath = QString::fromUtf8(*m_ownerIdJsonPathAttrPtr);
    QString documentCol = QString::fromUtf8(*m_documentColumnAttrPtr);
    QString membershipTable = QString::fromUtf8(*m_membershipTableAttrPtr);
    QString tenantIdCol = QString::fromUtf8(*m_tenantIdColumnAttrPtr);
    QString activeCol = QString::fromUtf8(*m_activeColumnAttrPtr);

    QString docRef;
    if (tableAlias.isEmpty()){
        docRef = QStringLiteral("\"%1\"").arg(documentCol);
    } else {
        docRef = QStringLiteral("%1.\"%2\"").arg(tableAlias, documentCol);
    }

    QString activeCondition;
    if (!activeCol.isEmpty()){
        activeCondition = QStringLiteral(" AND \"%1\"=true").arg(activeCol);
    }

    return QStringLiteral("EXISTS (SELECT 1 FROM \"%1\" WHERE \"%2\"=(%3->>'%4') AND \"%5\"='%6'%7)")
        .arg(membershipTable,
             QStringLiteral("UserId"),
             docRef, ownerIdPath,
             tenantIdCol, imtdb::EscapeSql(tenantId),
             activeCondition);
}


QString CDocumentOwnerTenantFilterStrategyComp::CreateGlobalScopeFilterSql(const QString& tableAlias) const
{
    QString ownerIdPath = QString::fromUtf8(*m_ownerIdJsonPathAttrPtr);
    QString documentCol = QString::fromUtf8(*m_documentColumnAttrPtr);
    QString membershipTable = QString::fromUtf8(*m_membershipTableAttrPtr);
    QString activeCol = QString::fromUtf8(*m_activeColumnAttrPtr);

    QString docRef;
    if (tableAlias.isEmpty()){
        docRef = QStringLiteral("\"%1\"").arg(documentCol);
    } else {
        docRef = QStringLiteral("%1.\"%2\"").arg(tableAlias, documentCol);
    }

    QString activeCondition;
    if (!activeCol.isEmpty()){
        activeCondition = QStringLiteral(" AND \"%1\"=true").arg(activeCol);
    }

    return QStringLiteral("NOT EXISTS (SELECT 1 FROM \"%1\" WHERE \"%2\"=(%3->>'%4')%5)")
        .arg(membershipTable,
             QStringLiteral("UserId"),
             docRef, ownerIdPath,
             activeCondition);
}


bool CDocumentOwnerTenantFilterStrategyComp::RequiresJoin() const
{
    return false;
}


QString CDocumentOwnerTenantFilterStrategyComp::CreateJoinClauseSql(const QString& /*tableAlias*/) const
{
    return QString();
}


} // namespace imtdb


