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
class ITenantInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_TENANT_ID = idoc::IDocumentMetaInfo::MIT_USER + 200,
		MIT_TENANT_NAME,
		MIT_TENANT_DESCRIPTION,
		MIT_TENANT_IS_ACTIVE,
		MIT_TENANT_OWNER_ID
	};

	/**
		Tenant relationship roles.
	*/
	enum TenantRelationshipRole
	{
		Parent = 0,
		Child,
		Partner,
		Supplier,
		Customer,
		Affiliate
	};

	/**
		Structure describing a relationship between tenants.
	*/
	struct TenantRelationship
	{
		QByteArray relationshipId;
		QByteArray targetTenantId;
		TenantRelationshipRole role;
		QString description;
		QString createdAt;

		bool operator==(const TenantRelationship& other) const
		{
			return relationshipId == other.relationshipId
				&& targetTenantId == other.targetTenantId
				&& role == other.role
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
};


typedef istd::TUniqueInterfacePtr<ITenantInfo> ITenantInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantInfo> ITenantInfoSharedPtr;


} // namespace imtauth

