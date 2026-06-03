// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ITenantConnectionRequestData.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantConnectionRequestManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantConnectionRequest
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantConnectionRequestManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantConnectionRequest);
		I_ASSIGN(m_requestCollectionCompPtr, "RequestCollection", "Connection request collection", false, "RequestCollection");
		I_ASSIGN(m_requestFactoryCompPtr, "RequestFactory", "Connection request factory", false, "TenantConnectionRequestInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager used to create relationships on accept", false, "TenantManager");
	I_END_COMPONENT;

	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& targetIdentifier,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message = QString(),
				const QString& expiresAt = QString()) override;
	virtual QByteArray CreateConnectCode(
				const QByteArray& sourceTenantId,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message,
				const QString& expiresAt,
				QString& generatedCode) override;
	virtual bool AcceptConnectionRequest(const QByteArray& requestId, const QByteArray& acceptingTenantId) override;
	virtual QByteArray AcceptConnectCode(const QString& connectCode, const QByteArray& acceptingTenantId) override;
	virtual bool RejectConnectionRequest(const QByteArray& requestId) override;
	virtual bool RevokeConnectionRequest(const QByteArray& requestId) override;
	virtual bool DeleteConnectCode(const QByteArray& requestId) override;
	virtual int PurgeExpiredConnectCodes(const QByteArray& sourceTenantId) override;
	virtual TenantConnectionRequestInfo GetConnectionRequest(const QByteArray& requestId) const override;
	virtual TenantConnectionRequests GetOutgoingRequests(const QByteArray& sourceTenantId) const override;
	virtual TenantConnectionRequests GetIncomingRequests(
				const QByteArray& targetTenantId,
				const QString& targetIdentifier = QString()) const override;
	virtual TenantConnectionRequestInfo GetRequestByCode(const QString& connectCode) const override;

private:
	QByteArray StoreRequest(const TenantConnectionRequestInfo& info);
	TenantConnectionRequests CollectRequests(const std::function<bool(const TenantConnectionRequestInfo&)>& predicate) const;
	bool UpdateRequestStatus(const QByteArray& requestId, TenantConnectionStatus status);
	bool IsRequestPending(const TenantConnectionRequestInfo& info) const;
	bool EstablishRelationship(const TenantConnectionRequestInfo& info, const QByteArray& acceptingTenantId);

private:
	I_REF(imtbase::IObjectCollection, m_requestCollectionCompPtr);
	I_FACT(imtauth::ITenantConnectionRequestData, m_requestFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth
