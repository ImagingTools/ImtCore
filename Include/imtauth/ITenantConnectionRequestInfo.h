// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtauth
{


/**
Standalone data model for a single connection request between two tenants.

\ingroup Tenant
*/
class ITenantConnectionRequestInfo: virtual public iser::IObject
{
public:
	/**
		Status of a connection request.
	*/
	enum ConnectionRequestStatus
	{
		CRS_PENDING = 0,
		CRS_APPROVED,
		CRS_REJECTED,
		CRS_CANCELED
	};

	I_DECLARE_ENUM(ConnectionRequestStatus, CRS_PENDING, CRS_APPROVED, CRS_REJECTED, CRS_CANCELED);

	virtual QByteArray GetRequestId() const = 0;
	virtual void SetRequestId(const QByteArray& requestId) = 0;

	virtual QByteArray GetSourceTenantId() const = 0;
	virtual void SetSourceTenantId(const QByteArray& sourceTenantId) = 0;

	virtual QByteArray GetTargetTenantId() const = 0;
	virtual void SetTargetTenantId(const QByteArray& targetTenantId) = 0;

	virtual QString GetConnectionCode() const = 0;
	virtual void SetConnectionCode(const QString& connectionCode) = 0;

	virtual QString GetMessage() const = 0;
	virtual void SetMessage(const QString& message) = 0;

	virtual ConnectionRequestStatus GetStatus() const = 0;
	virtual void SetStatus(ConnectionRequestStatus status) = 0;

	virtual QString GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	virtual QString GetRespondedAt() const = 0;
	virtual void SetRespondedAt(const QString& respondedAt) = 0;

	virtual QString GetSourceTenantName() const = 0;
	virtual void SetSourceTenantName(const QString& sourceTenantName) = 0;

	virtual QString GetTargetTenantName() const = 0;
	virtual void SetTargetTenantName(const QString& targetTenantName) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionRequestInfo> ITenantConnectionRequestInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionRequestInfo> ITenantConnectionRequestInfoSharedPtr;


} // namespace imtauth
