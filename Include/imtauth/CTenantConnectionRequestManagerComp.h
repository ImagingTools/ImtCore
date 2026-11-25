// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionCodeInfo.h>
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ITenantDirectory.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantConnectionRequestManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantConnectionRequestManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef ITenantConnectionRequestInfo::ConnectionRequestStatus ConnectionRequestStatus;
	typedef ITenantConnectionInfo::ConnectionStatus ConnectionStatus;

	I_BEGIN_COMPONENT(CTenantConnectionRequestManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantConnectionRequestManager);
		I_ASSIGN(m_requestCollectionCompPtr, "RequestCollection", "Connection request collection", true, "RequestCollection");
		I_ASSIGN(m_connectionCodeCollectionCompPtr, "ConnectionCodeCollection", "Connection code collection", true, "ConnectionCodeCollection");
		I_ASSIGN(m_connectionCollectionCompPtr, "ConnectionCollection", "Connections collection", true, "ConnectionCollection");
		I_ASSIGN(m_requestFactoryCompPtr, "RequestFactory", "Connection request factory", false, "TenantConnectionRequestInfo");
		I_ASSIGN(m_connectionCodeFactoryCompPtr, "ConnectionCodeFactory", "Connection code factory", false, "TenantConnectionCodeInfo");
		I_ASSIGN(m_connectionFactoryCompPtr, "ConnectionFactory", "Connection factory", false, "TenantConnectionInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_directoryCompPtr, "TenantDirectory", "Directory of the organizations, for the requests to the listed ones", false, "TenantDirectory");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantConnectionRequestManager)
	virtual QString GetConnectionCode(const QByteArray& tenantId) override;
	virtual bool GetAllowConnectionsByCode(const QByteArray& tenantId) override;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) override;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) override;
	virtual QByteArray FindTenantByConnectionCode(const QString& connectionCode) const override;
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString(),
				QString* errorMessagePtr = nullptr) override;
	virtual QByteArray CreateConnectionRequestToTenant(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QString& message = QString(),
				QString* errorMessagePtr = nullptr) override;
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) override;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual QByteArrayList GetConnectionRequestIds(const QByteArray& tenantId) const override;
	virtual ITenantConnectionRequestInfoUniquePtr GetConnectionRequest(const QByteArray& requestId) const override;
	virtual QByteArrayList GetConnectionIds(const QByteArray& tenantId) const override;
	virtual ITenantConnectionInfoUniquePtr GetConnection(const QByteArray& connectionId) const override;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) override;
	virtual bool AreConnected(const QByteArray& tenantId, const QByteArray& otherTenantId) const override;
	virtual QByteArray EnsureConnection(const QByteArray& tenantId, const QByteArray& otherTenantId) override;

private:
	QString GenerateConnectionCode() const;
	QByteArray RefuseRequest(const QString& reason, QString* errorMessagePtr) const;
	QByteArray CreateRequest(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QString& connectionCode,
				const QString& message,
				QString* errorMessagePtr);
	bool ConnectionExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const;
	bool PendingRequestExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const;
	QByteArray CreateConnection(const QByteArray& tenantAId, const QByteArray& tenantBId);

	// DB helper methods
	bool StoreConnectionCode(const QByteArray& tenantId, const ITenantConnectionCodeInfo& codeInfo);
	bool StoreConnectionRequest(const ITenantConnectionRequestInfo& requestInfo);
	bool StoreConnection(const ITenantConnectionInfo& connectionInfo);

	// Helper to get code object from collection
	ITenantConnectionCodeInfoUniquePtr GetConnectionCodeObject(const QByteArray& tenantId);
	void EnsureConnectionCode(const QByteArray& tenantId);

private:
	I_REF(imtbase::IObjectCollection, m_requestCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCodeCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCollectionCompPtr);
	I_FACT(imtauth::ITenantConnectionRequestInfo, m_requestFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionCodeInfo, m_connectionCodeFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionInfo, m_connectionFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantDirectory, m_directoryCompPtr);

	// A connection is made or removed once at a time
	QMutex m_connectionChangeMutex;
};


} // namespace imtauth
