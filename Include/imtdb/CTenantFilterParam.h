// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/ITenantFilterParam.h>


namespace imtdb
{


/**
 * @brief Concrete serializable model for tenant filter parameters.
 *
 * Contains all necessary information for database delegates to construct
 * tenant-scoped SQL queries. Placed into the ParamsSet and passed to delegates.
 *
 * @section factory Factory methods
 *
 * Use the static factory methods for convenient construction:
 * @code{.cpp}
 * // For entities with direct TenantId column:
 * auto param = CTenantFilterParam::CreateDirect(tenantId, "TenantId");
 *
 * // For entities linked through membership table:
 * auto param = CTenantFilterParam::CreateMembership(tenantId, "TenantMemberships", "UserId", "TenantId", "IsActive");
 *
 * // For document entities:
 * auto param = CTenantFilterParam::CreateDocumentOwner(tenantId, "TenantMemberships", "OwnerId", "Document");
 * @endcode
 */
class CTenantFilterParam: virtual public ITenantFilterParam
{
public:
    CTenantFilterParam();

    // Factory methods
    static CTenantFilterParam* CreateDirect(
                const QByteArray& tenantId,
                const QByteArray& tenantIdColumn = "TenantId");

    static CTenantFilterParam* CreateMembership(
                const QByteArray& tenantId,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& entityIdColumn = "UserId",
                const QByteArray& tenantIdColumn = "TenantId",
                const QByteArray& activeColumn = "IsActive");

    static CTenantFilterParam* CreateDocumentOwner(
                const QByteArray& tenantId,
                const QByteArray& membershipTable = "TenantMemberships",
                const QByteArray& ownerIdJsonPath = "OwnerId",
                const QByteArray& documentColumn = "Document");

    // reimplemented (imtdb::ITenantFilterParam)
    virtual QByteArray GetTenantId() const override;
    virtual void SetTenantId(const QByteArray& tenantId) override;
    virtual FilterType GetFilterType() const override;
    virtual void SetFilterType(FilterType filterType) override;
    virtual QByteArray GetMembershipTable() const override;
    virtual void SetMembershipTable(const QByteArray& tableName) override;
    virtual QByteArray GetEntityIdColumn() const override;
    virtual void SetEntityIdColumn(const QByteArray& columnName) override;
    virtual QByteArray GetTenantIdColumn() const override;
    virtual void SetTenantIdColumn(const QByteArray& columnName) override;
    virtual QByteArray GetActiveColumn() const override;
    virtual void SetActiveColumn(const QByteArray& columnName) override;
    virtual QByteArray GetDirectTenantIdColumn() const override;
    virtual void SetDirectTenantIdColumn(const QByteArray& columnName) override;
    virtual QByteArray GetOwnerIdJsonPath() const override;
    virtual void SetOwnerIdJsonPath(const QByteArray& jsonPath) override;
    virtual QByteArray GetDocumentColumn() const override;
    virtual void SetDocumentColumn(const QByteArray& columnName) override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;

    // reimplemented (istd::IChangeable)
    virtual int GetSupportedOperations() const override;
    virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
    virtual bool IsEqual(const IChangeable& object) const override;
    virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
    virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
    QByteArray m_tenantId;
    FilterType m_filterType;
    QByteArray m_membershipTable;
    QByteArray m_entityIdColumn;
    QByteArray m_tenantIdColumn;
    QByteArray m_activeColumn;
    QByteArray m_directTenantIdColumn;
    QByteArray m_ownerIdJsonPath;
    QByteArray m_documentColumn;
};


} // namespace imtdb


