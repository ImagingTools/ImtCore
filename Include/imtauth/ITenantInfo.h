// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QList>


namespace imtauth
{


/**
	Interface for describing a tenant.
	\ingroup Tenant
*/
/**
	Get the well-known System-Tenant ID.
	The System-Tenant is the root tenant that is automatically created at server startup.
	All users implicitly belong to the System-Tenant.
*/
inline const QByteArray& GetSystemTenantId()
{
	static const QByteArray s_id = QByteArrayLiteral("00000000-0000-0000-0000-000000000001");
	return s_id;
}

class ITenantInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_TENANT_ID = idoc::IDocumentMetaInfo::MIT_USER + 200,
		MIT_TENANT_NAME,
		MIT_TENANT_DESCRIPTION,
		MIT_TENANT_IS_ACTIVE,
		MIT_TENANT_OWNER_ID,
		MIT_PARENT_TENANT_ID
	};

	/**
		Tenant relationship roles.
	*/
	enum TenantRelationshipRole
	{
		TRR_PARENT,
		TRR_CHILD,
		TRR_PARTNER,
		TRR_SUPPLIER,
		TRR_CUSTOMER,
		TRR_AFFILIATE
	};
	I_DECLARE_ENUM(TenantRelationshipRole, TRR_PARENT, TRR_CHILD, TRR_PARTNER, TRR_SUPPLIER, TRR_CUSTOMER, TRR_AFFILIATE);

	/**
		Tenant relationship status.
	*/
	enum TenantRelationshipStatus
	{
		TRS_ACTIVE,
		TRS_ARCHIVED,
		TRS_PENDING_APPROVED
	};
	I_DECLARE_ENUM(TenantRelationshipStatus, TRS_ACTIVE, TRS_ARCHIVED, TRS_PENDING_APPROVED);

	/**
		Structure describing a relationship between tenants.

		Relationships are built on top of a Connection and require bilateral
		approval (via RelationshipProposal). They are direction-aware:
		\a sourceRole describes the role of the source tenant, while
		\a targetRole describes the role of the target tenant.
	*/
	struct TenantRelationship
	{
		QByteArray relationshipId;
		QByteArray connectionId;
		QByteArray sourceTenantId;
		QByteArray targetTenantId;
		TenantRelationshipRole sourceRole = TRR_PARTNER;
		TenantRelationshipRole targetRole = TRR_PARTNER;
		QString scope;
		QString validFrom;
		QString validUntil;
		TenantRelationshipStatus status = TRS_ACTIVE;
		QString description;
		QString createdAt;
		QString updatedAt;

		bool operator==(const TenantRelationship& other) const
		{
			return relationshipId == other.relationshipId
				&& connectionId == other.connectionId
				&& sourceTenantId == other.sourceTenantId
				&& targetTenantId == other.targetTenantId
				&& sourceRole == other.sourceRole
				&& targetRole == other.targetRole
				&& scope == other.scope
				&& validFrom == other.validFrom
				&& validUntil == other.validUntil
				&& status == other.status
				&& description == other.description
				&& createdAt == other.createdAt;
		}

		bool operator!=(const TenantRelationship& other) const
		{
			return !(*this == other);
		}
	};

	typedef QList<TenantRelationship> TenantRelationships;

	/**
		Get tenant ID.
	*/
	virtual QByteArray GetTenantId() const = 0;

	/**
		Set tenant ID.
	*/
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	/**
		Get tenant name.
	*/
	virtual QString GetTenantName() const = 0;

	/**
		Set tenant name.
	*/
	virtual void SetTenantName(const QString& name) = 0;

	/**
		Get tenant description.
	*/
	virtual QString GetTenantDescription() const = 0;

	/**
		Set tenant description.
	*/
	virtual void SetTenantDescription(const QString& description) = 0;

	/**
		Get owner user ID.
	*/
	virtual QByteArray GetOwnerId() const = 0;

	/**
		Set owner user ID.
	*/
	virtual void SetOwnerId(const QByteArray& ownerId) = 0;

	/**
		Get creator user ID.
		The creator is permanently assigned when the tenant is first saved.
	*/
	virtual QByteArray GetCreatorId() const = 0;

	/**
		Set creator user ID.
		Should only be called once (immutable after first assignment).
	*/
	virtual void SetCreatorId(const QByteArray& creatorId) = 0;

	/**
		Check if tenant is active.
	*/
	virtual bool IsActive() const = 0;

	/**
	  Set active state.
	*/
	virtual void SetActive(bool isActive) = 0;

	/**
		Get created at timestamp.
	*/
	virtual QString GetCreatedAt() const = 0;

	/**
		Set created at timestamp.
	*/
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	/**
		Get updated at timestamp.
	*/
	virtual QString GetUpdatedAt() const = 0;

	/**
		Set updated at timestamp.
	*/
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;

	/**
		Get all relationships for this tenant.
	*/
	virtual TenantRelationships GetRelationships() const = 0;

	/**
		Set relationships for this tenant.
	*/
	virtual void SetRelationships(const TenantRelationships& relationships) = 0;

	/**
		Add a relationship to this tenant.
	*/
	virtual void AddRelationship(const TenantRelationship& relationship) = 0;

	/**
		Remove a relationship by its ID.
	*/
	virtual bool RemoveRelationship(const QByteArray& relationshipId) = 0;

	/**
		Get the list of permissions available for this tenant.
	*/
	virtual QByteArrayList GetTenantPermissions() const = 0;

	/**
		Set the list of permissions available for this tenant.
	*/
	virtual void SetTenantPermissions(const QByteArrayList& permissions) = 0;

	/**
		Get parent tenant ID.
		Empty if this is a top-level tenant or the System-Tenant.
	*/
	virtual QByteArray GetParentTenantId() const = 0;

	/**
		Set parent tenant ID.
	*/
	virtual void SetParentTenantId(const QByteArray& parentTenantId) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantInfo> ITenantInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantInfo> ITenantInfoSharedPtr;


} // namespace imtauth

