// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	A document of one tenant (owner) made available to another tenant (target).
	The document itself stays in the owner tenant; the share only grants access to it.
	Users of the target tenant see it within their own permissions, capped by the access level.
	If target groups are set, only members of these target tenant groups see the document.
	\ingroup Tenant
*/
class IDocumentShareInfo: virtual public iser::IObject
{
public:
	/**
		Access a tenant has to a document. A higher level includes the lower ones.
	*/
	enum AccessLevel
	{
		DAL_NONE = 0,
		DAL_VIEW,
		DAL_EDIT,
		DAL_OWNER
	};

	/**
		Lifecycle status of a share. Only active shares give the target tenant access to the document.
	*/
	enum ShareStatus
	{
		DSS_PENDING = 0,
		DSS_ACTIVE,
		DSS_DECLINED
	};

	virtual QByteArray GetShareId() const = 0;
	virtual void SetShareId(const QByteArray& shareId) = 0;

	virtual QByteArray GetCollectionId() const = 0;
	virtual void SetCollectionId(const QByteArray& collectionId) = 0;

	virtual QByteArray GetEntityId() const = 0;
	virtual void SetEntityId(const QByteArray& entityId) = 0;

	virtual QString GetEntityName() const = 0;
	virtual void SetEntityName(const QString& entityName) = 0;

	virtual QByteArray GetOwnerTenantId() const = 0;
	virtual void SetOwnerTenantId(const QByteArray& tenantId) = 0;

	virtual QString GetOwnerTenantName() const = 0;
	virtual void SetOwnerTenantName(const QString& tenantName) = 0;

	virtual QByteArray GetTargetTenantId() const = 0;
	virtual void SetTargetTenantId(const QByteArray& tenantId) = 0;

	virtual QString GetTargetTenantName() const = 0;
	virtual void SetTargetTenantName(const QString& tenantName) = 0;

	/**
		Groups of the target tenant seeing the document. Empty means every member of the target tenant.
	*/
	virtual QByteArrayList GetTargetGroupIds() const = 0;
	virtual void SetTargetGroupIds(const QByteArrayList& groupIds) = 0;

	virtual AccessLevel GetAccessLevel() const = 0;
	virtual void SetAccessLevel(AccessLevel accessLevel) = 0;

	virtual ShareStatus GetStatus() const = 0;
	virtual void SetStatus(ShareStatus status) = 0;

	/**
		End of the validity. An invalid time means no expiration.
	*/
	virtual QDateTime GetValidUntil() const = 0;
	virtual void SetValidUntil(const QDateTime& validUntil) = 0;

	virtual QString GetMessage() const = 0;
	virtual void SetMessage(const QString& message) = 0;

	virtual QByteArray GetCreatedByUserId() const = 0;
	virtual void SetCreatedByUserId(const QByteArray& userId) = 0;

	virtual QDateTime GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QDateTime& createdAt) = 0;

	virtual QDateTime GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QDateTime& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IDocumentShareInfo> IDocumentShareInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IDocumentShareInfo> IDocumentShareInfoSharedPtr;


} // namespace imtauth


