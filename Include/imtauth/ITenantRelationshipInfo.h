// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtauth
{


/**
	Roles that describe the nature of a tenant-to-tenant relationship.
*/
enum TenantRelationshipRole
{
	TRR_PARENT = 0,
	TRR_CHILD,
	TRR_PARTNER,
	TRR_SUPPLIER,
	TRR_CUSTOMER,
	TRR_AFFILIATE
};


/**
	Status of a tenant relationship.
*/
enum TenantRelationshipStatus
{
	TRS_ACTIVE = 0,
	TRS_ARCHIVED,
	TRS_PENDING_APPROVED
};


/**
	Standalone interface describing a relationship between two tenants.

	Relationships are built on top of a Connection and require bilateral
	approval (via RelationshipProposal). They are direction-aware:
	sourceRole describes the role of the source tenant, while
	targetRole describes the role of the target tenant.

	\ingroup Tenant
*/
class ITenantRelationshipInfo: virtual public iser::IObject
{
public:
	I_DECLARE_ENUM(TenantRelationshipRole, TRR_PARENT, TRR_CHILD, TRR_PARTNER, TRR_SUPPLIER, TRR_CUSTOMER, TRR_AFFILIATE);
	I_DECLARE_ENUM(TenantRelationshipStatus, TRS_ACTIVE, TRS_ARCHIVED, TRS_PENDING_APPROVED);

	enum MetaInfoTypes
	{
		MIT_RELATIONSHIP_ID = idoc::IDocumentMetaInfo::MIT_USER + 500,
		MIT_RELATIONSHIP_CONNECTION_ID,
		MIT_RELATIONSHIP_SOURCE_TENANT_ID,
		MIT_RELATIONSHIP_TARGET_TENANT_ID,
		MIT_RELATIONSHIP_SOURCE_ROLE,
		MIT_RELATIONSHIP_TARGET_ROLE,
		MIT_RELATIONSHIP_STATUS
	};

	virtual QByteArray GetRelationshipId() const = 0;
	virtual void SetRelationshipId(const QByteArray& relationshipId) = 0;

	virtual QByteArray GetConnectionId() const = 0;
	virtual void SetConnectionId(const QByteArray& connectionId) = 0;

	virtual QByteArray GetSourceTenantId() const = 0;
	virtual void SetSourceTenantId(const QByteArray& sourceTenantId) = 0;

	virtual QByteArray GetTargetTenantId() const = 0;
	virtual void SetTargetTenantId(const QByteArray& targetTenantId) = 0;

	virtual TenantRelationshipRole GetSourceRole() const = 0;
	virtual void SetSourceRole(TenantRelationshipRole role) = 0;

	virtual TenantRelationshipRole GetTargetRole() const = 0;
	virtual void SetTargetRole(TenantRelationshipRole role) = 0;

	virtual QString GetScope() const = 0;
	virtual void SetScope(const QString& scope) = 0;

	virtual QString GetValidFrom() const = 0;
	virtual void SetValidFrom(const QString& validFrom) = 0;

	virtual QString GetValidUntil() const = 0;
	virtual void SetValidUntil(const QString& validUntil) = 0;

	virtual TenantRelationshipStatus GetStatus() const = 0;
	virtual void SetStatus(TenantRelationshipStatus status) = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& description) = 0;

	virtual QString GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	virtual QString GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantRelationshipInfo> ITenantRelationshipInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantRelationshipInfo> ITenantRelationshipInfoSharedPtr;


} // namespace imtauth
