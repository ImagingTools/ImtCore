// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantFilterParam.h>


namespace imtdb
{


CTenantFilterParam::CTenantFilterParam()
    : m_filterType(FT_DIRECT)
    , m_directTenantIdColumn("TenantId")
    , m_membershipTable("TenantMemberships")
    , m_entityIdColumn("UserId")
    , m_tenantIdColumn("TenantId")
    , m_activeColumn("IsActive")
    , m_ownerIdJsonPath("OwnerId")
    , m_documentColumn("Document")
{
}


// --- Factory methods ---

CTenantFilterParam* CTenantFilterParam::CreateDirect(
            const QByteArray& tenantId,
            const QByteArray& tenantIdColumn)
{
    CTenantFilterParam* paramPtr = new CTenantFilterParam();
    paramPtr->SetTenantId(tenantId);
    paramPtr->SetFilterType(FT_DIRECT);
    paramPtr->SetDirectTenantIdColumn(tenantIdColumn);
    return paramPtr;
}


CTenantFilterParam* CTenantFilterParam::CreateMembership(
            const QByteArray& tenantId,
            const QByteArray& membershipTable,
            const QByteArray& entityIdColumn,
            const QByteArray& tenantIdColumn,
            const QByteArray& activeColumn)
{
    CTenantFilterParam* paramPtr = new CTenantFilterParam();
    paramPtr->SetTenantId(tenantId);
    paramPtr->SetFilterType(FT_MEMBERSHIP);
    paramPtr->SetMembershipTable(membershipTable);
    paramPtr->SetEntityIdColumn(entityIdColumn);
    paramPtr->SetTenantIdColumn(tenantIdColumn);
    paramPtr->SetActiveColumn(activeColumn);
    return paramPtr;
}


CTenantFilterParam* CTenantFilterParam::CreateDocumentOwner(
            const QByteArray& tenantId,
            const QByteArray& membershipTable,
            const QByteArray& ownerIdJsonPath,
            const QByteArray& documentColumn)
{
    CTenantFilterParam* paramPtr = new CTenantFilterParam();
    paramPtr->SetTenantId(tenantId);
    paramPtr->SetFilterType(FT_DOCUMENT_OWNER);
    paramPtr->SetMembershipTable(membershipTable);
    paramPtr->SetOwnerIdJsonPath(ownerIdJsonPath);
    paramPtr->SetDocumentColumn(documentColumn);
    return paramPtr;
}


// --- ITenantFilterParam implementation ---

QByteArray CTenantFilterParam::GetTenantId() const
{
    return m_tenantId;
}

void CTenantFilterParam::SetTenantId(const QByteArray& tenantId)
{
    m_tenantId = tenantId;
}

ITenantFilterParam::FilterType CTenantFilterParam::GetFilterType() const
{
    return m_filterType;
}

void CTenantFilterParam::SetFilterType(FilterType filterType)
{
    m_filterType = filterType;
}

QByteArray CTenantFilterParam::GetMembershipTable() const
{
    return m_membershipTable;
}

void CTenantFilterParam::SetMembershipTable(const QByteArray& tableName)
{
    m_membershipTable = tableName;
}

QByteArray CTenantFilterParam::GetEntityIdColumn() const
{
    return m_entityIdColumn;
}

void CTenantFilterParam::SetEntityIdColumn(const QByteArray& columnName)
{
    m_entityIdColumn = columnName;
}

QByteArray CTenantFilterParam::GetTenantIdColumn() const
{
    return m_tenantIdColumn;
}

void CTenantFilterParam::SetTenantIdColumn(const QByteArray& columnName)
{
    m_tenantIdColumn = columnName;
}

QByteArray CTenantFilterParam::GetActiveColumn() const
{
    return m_activeColumn;
}

void CTenantFilterParam::SetActiveColumn(const QByteArray& columnName)
{
    m_activeColumn = columnName;
}

QByteArray CTenantFilterParam::GetDirectTenantIdColumn() const
{
    return m_directTenantIdColumn;
}

void CTenantFilterParam::SetDirectTenantIdColumn(const QByteArray& columnName)
{
    m_directTenantIdColumn = columnName;
}

QByteArray CTenantFilterParam::GetOwnerIdJsonPath() const
{
    return m_ownerIdJsonPath;
}

void CTenantFilterParam::SetOwnerIdJsonPath(const QByteArray& jsonPath)
{
    m_ownerIdJsonPath = jsonPath;
}

QByteArray CTenantFilterParam::GetDocumentColumn() const
{
    return m_documentColumn;
}

void CTenantFilterParam::SetDocumentColumn(const QByteArray& columnName)
{
    m_documentColumn = columnName;
}


// --- ISerializable ---

bool CTenantFilterParam::Serialize(iser::IArchive& archive)
{
    archive.Serialize(m_tenantId, "TenantId", "Tenant ID");

    int filterType = static_cast<int>(m_filterType);
    archive.Serialize(filterType, "FilterType", "Filter type");
    m_filterType = static_cast<FilterType>(filterType);

    archive.Serialize(m_membershipTable, "MembershipTable", "Membership table name");
    archive.Serialize(m_entityIdColumn, "EntityIdColumn", "Entity ID column");
    archive.Serialize(m_tenantIdColumn, "TenantIdColumn", "Tenant ID column");
    archive.Serialize(m_activeColumn, "ActiveColumn", "Active status column");
    archive.Serialize(m_directTenantIdColumn, "DirectTenantIdColumn", "Direct TenantId column");
    archive.Serialize(m_ownerIdJsonPath, "OwnerIdJsonPath", "Owner ID JSON path");
    archive.Serialize(m_documentColumn, "DocumentColumn", "Document column");

    return true;
}


// --- IChangeable ---

int CTenantFilterParam::GetSupportedOperations() const
{
    return OnCopyFrom | OnClone | OnCompare | OnReset;
}


bool CTenantFilterParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
    const ITenantFilterParam* sourcePtr = dynamic_cast<const ITenantFilterParam*>(&object);
    if (sourcePtr == nullptr){
        return false;
    }

    m_tenantId = sourcePtr->GetTenantId();
    m_filterType = sourcePtr->GetFilterType();
    m_membershipTable = sourcePtr->GetMembershipTable();
    m_entityIdColumn = sourcePtr->GetEntityIdColumn();
    m_tenantIdColumn = sourcePtr->GetTenantIdColumn();
    m_activeColumn = sourcePtr->GetActiveColumn();
    m_directTenantIdColumn = sourcePtr->GetDirectTenantIdColumn();
    m_ownerIdJsonPath = sourcePtr->GetOwnerIdJsonPath();
    m_documentColumn = sourcePtr->GetDocumentColumn();

    return true;
}


bool CTenantFilterParam::IsEqual(const IChangeable& object) const
{
    const ITenantFilterParam* otherPtr = dynamic_cast<const ITenantFilterParam*>(&object);
    if (otherPtr == nullptr){
        return false;
    }

    return m_tenantId == otherPtr->GetTenantId()
        && m_filterType == otherPtr->GetFilterType()
        && m_membershipTable == otherPtr->GetMembershipTable()
        && m_entityIdColumn == otherPtr->GetEntityIdColumn()
        && m_tenantIdColumn == otherPtr->GetTenantIdColumn()
        && m_activeColumn == otherPtr->GetActiveColumn()
        && m_directTenantIdColumn == otherPtr->GetDirectTenantIdColumn()
        && m_ownerIdJsonPath == otherPtr->GetOwnerIdJsonPath()
        && m_documentColumn == otherPtr->GetDocumentColumn();
}


istd::IChangeableUniquePtr CTenantFilterParam::CloneMe(CompatibilityMode mode) const
{
    CTenantFilterParam* clonePtr = new CTenantFilterParam();
    clonePtr->CopyFrom(*this, mode);
    return istd::IChangeableUniquePtr(clonePtr);
}


bool CTenantFilterParam::ResetData(CompatibilityMode /*mode*/)
{
    m_tenantId.clear();
    m_filterType = FT_DIRECT;
    m_membershipTable = "TenantMemberships";
    m_entityIdColumn = "UserId";
    m_tenantIdColumn = "TenantId";
    m_activeColumn = "IsActive";
    m_directTenantIdColumn = "TenantId";
    m_ownerIdJsonPath = "OwnerId";
    m_documentColumn = "Document";
    return true;
}


} // namespace imtdb


