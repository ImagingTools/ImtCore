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
    paramPtr->m_tenantId = tenantId;
    paramPtr->m_filterType = FT_DIRECT;
    paramPtr->m_directTenantIdColumn = tenantIdColumn;
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
    paramPtr->m_tenantId = tenantId;
    paramPtr->m_filterType = FT_MEMBERSHIP;
    paramPtr->m_membershipTable = membershipTable;
    paramPtr->m_entityIdColumn = entityIdColumn;
    paramPtr->m_tenantIdColumn = tenantIdColumn;
    paramPtr->m_activeColumn = activeColumn;
    return paramPtr;
}


CTenantFilterParam* CTenantFilterParam::CreateDocumentOwner(
            const QByteArray& tenantId,
            const QByteArray& membershipTable,
            const QByteArray& ownerIdJsonPath,
            const QByteArray& documentColumn)
{
    CTenantFilterParam* paramPtr = new CTenantFilterParam();
    paramPtr->m_tenantId = tenantId;
    paramPtr->m_filterType = FT_DOCUMENT_OWNER;
    paramPtr->m_membershipTable = membershipTable;
    paramPtr->m_ownerIdJsonPath = ownerIdJsonPath;
    paramPtr->m_documentColumn = documentColumn;
    return paramPtr;
}


// --- ITenantFilterParam implementation ---

QByteArray CTenantFilterParam::GetTenantId() const
{
    return m_tenantId;
}

ITenantFilterParam::FilterType CTenantFilterParam::GetFilterType() const
{
    return m_filterType;
}

QByteArray CTenantFilterParam::GetMembershipTable() const
{
    return m_membershipTable;
}

QByteArray CTenantFilterParam::GetEntityIdColumn() const
{
    return m_entityIdColumn;
}

QByteArray CTenantFilterParam::GetTenantIdColumn() const
{
    return m_tenantIdColumn;
}

QByteArray CTenantFilterParam::GetActiveColumn() const
{
    return m_activeColumn;
}

QByteArray CTenantFilterParam::GetDirectTenantIdColumn() const
{
    return m_directTenantIdColumn;
}

QByteArray CTenantFilterParam::GetOwnerIdJsonPath() const
{
    return m_ownerIdJsonPath;
}

QByteArray CTenantFilterParam::GetDocumentColumn() const
{
    return m_documentColumn;
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
    return OnCopyFrom | OnClone | OnReset;
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


