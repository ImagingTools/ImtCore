// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionInfo.h>


namespace imtauth
{


/**
	The address books of the organizations: who may invite an organization directly.
	A connection gives no access; the contracts do (\see IContractManager).
	An organization connects to another by its connection code, or to an organization listed in the directory.

	\ingroup Tenant
*/
class ITenantConnectionRequestManager: virtual public istd::IChangeable
{
public:
	virtual QString GetConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool GetAllowConnectionsByCode(const QByteArray& tenantId) = 0;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) = 0;

	/**
		The organization of a connection code, empty when there is none.
	*/
	virtual QByteArray FindTenantByConnectionCode(const QString& connectionCode) const = 0;

	/**
		Create a connection request to the organization with the given connection code.
		\param errorMessagePtr Receives the reason when the request is refused.
	*/
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString(),
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Create a connection request to an organization listed in the directory.
		\param errorMessagePtr Receives the reason when the request is refused.
	*/
	virtual QByteArray CreateConnectionRequestToTenant(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QString& message = QString(),
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Approve a connection request: the organizations get connected.
		\return The connection, empty when the request is not approved.
	*/
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) = 0;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetConnectionRequestIds(const QByteArray& tenantId) const = 0;
	virtual ITenantConnectionRequestInfoUniquePtr GetConnectionRequest(const QByteArray& requestId) const = 0;
	virtual QByteArrayList GetConnectionIds(const QByteArray& tenantId) const = 0;
	virtual ITenantConnectionInfoUniquePtr GetConnection(const QByteArray& connectionId) const = 0;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) = 0;

	/**
		The two organizations have an active connection.
	*/
	virtual bool AreConnected(const QByteArray& tenantId, const QByteArray& otherTenantId) const = 0;

	/**
		Connect two organizations that concluded a contract with each other; an existing connection is kept.
		\return The connection, empty when it could not be made.
	*/
	virtual QByteArray EnsureConnection(const QByteArray& tenantId, const QByteArray& otherTenantId) = 0;
};


} // namespace imtauth


