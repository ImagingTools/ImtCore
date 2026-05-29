// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QUuid>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>


namespace imtauth
{


/**
	Self-contained mock replicating the lifecycle semantics of
	CTenantConnectionRequestManagerComp, used to verify the expected behaviour
	of tenant connection requests / connect codes without bringing up the
	component framework. Accepting a request increments m_relationshipCount to
	emulate the asymmetric relationship that the real manager creates on both
	sides of the connection.
*/
class CMockTenantConnectionRequestManager
{
public:
	QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& targetIdentifier,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message = QString(),
				const QString& expiresAt = QString())
	{
		if (sourceTenantId.isEmpty() || targetIdentifier.isEmpty()){
			return QByteArray();
		}

		TenantConnectionRequestInfo info;
		info.requestId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetIdentifier = targetIdentifier;
		info.proposedSourceRole = proposedSourceRole;
		info.proposedTargetRole = proposedTargetRole;
		info.message = message;
		info.status = TCS_PENDING;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.expiresAt = expiresAt;

		m_requests.append(info);
		return info.requestId;
	}

	QByteArray CreateConnectCode(
				const QByteArray& sourceTenantId,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message,
				const QString& expiresAt,
				QString& generatedCode)
	{
		if (sourceTenantId.isEmpty()){
			return QByteArray();
		}

		TenantConnectionRequestInfo info;
		info.requestId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.connectCode = QUuid::createUuid().toString(QUuid::WithoutBraces).left(12).toUpper();
		info.proposedSourceRole = proposedSourceRole;
		info.proposedTargetRole = proposedTargetRole;
		info.message = message;
		info.status = TCS_PENDING;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.expiresAt = expiresAt;

		m_requests.append(info);
		generatedCode = info.connectCode;
		return info.requestId;
	}

	static bool IsExpired(const TenantConnectionRequestInfo& info)
	{
		if (info.expiresAt.isEmpty()){
			return false;
		}
		QDateTime expiresAt = QDateTime::fromString(info.expiresAt, Qt::ISODateWithMs);
		return expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc();
	}

	bool AcceptConnectionRequest(const QByteArray& requestId, const QByteArray& acceptingTenantId)
	{
		if (acceptingTenantId.isEmpty()){
			return false;
		}

		for (TenantConnectionRequestInfo& info : m_requests){
			if (info.requestId != requestId){
				continue;
			}
			if (info.status != TCS_PENDING){
				return false;
			}
			if (IsExpired(info)){
				info.status = TCS_EXPIRED;
				return false;
			}
			if (info.sourceTenantId == acceptingTenantId){
				return false;
			}

			info.status = TCS_ACCEPTED;
			info.targetTenantId = acceptingTenantId;
			info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
			// Mirror entries on both sides of the new relationship.
			m_relationshipCount += 2;
			return true;
		}

		return false;
	}

	QByteArray AcceptConnectCode(const QString& connectCode, const QByteArray& acceptingTenantId)
	{
		for (const TenantConnectionRequestInfo& info : m_requests){
			if (info.connectCode == connectCode && info.status == TCS_PENDING){
				if (AcceptConnectionRequest(info.requestId, acceptingTenantId)){
					return info.requestId;
				}
				return QByteArray();
			}
		}

		return QByteArray();
	}

	bool RejectConnectionRequest(const QByteArray& requestId)
	{
		for (TenantConnectionRequestInfo& info : m_requests){
			if (info.requestId == requestId && info.status == TCS_PENDING){
				info.status = TCS_REJECTED;
				info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	bool RevokeConnectionRequest(const QByteArray& requestId)
	{
		for (TenantConnectionRequestInfo& info : m_requests){
			if (info.requestId == requestId && info.status == TCS_PENDING){
				info.status = TCS_REVOKED;
				info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	TenantConnectionRequestInfo GetConnectionRequest(const QByteArray& requestId) const
	{
		for (const TenantConnectionRequestInfo& info : m_requests){
			if (info.requestId == requestId){
				return info;
			}
		}
		return TenantConnectionRequestInfo();
	}

	TenantConnectionRequests GetOutgoingRequests(const QByteArray& sourceTenantId) const
	{
		TenantConnectionRequests result;
		for (const TenantConnectionRequestInfo& info : m_requests){
			if (info.sourceTenantId == sourceTenantId){
				result.append(info);
			}
		}
		return result;
	}

	QList<TenantConnectionRequestInfo> m_requests;
	int m_counter = 0;
	int m_relationshipCount = 0;
};


} // namespace imtauth


class CTenantConnectionRequestTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testRequestInfo_RoundTrip();
	void testRequestInfo_CopyFromAndClone();
	void testRequestInfo_ResetDataDefaults();

	// Manager semantics
	void testCreateRequest_Success();
	void testCreateRequest_MissingFields_Fails();
	void testCreateConnectCode_Success();
	void testCreateConnectCode_MissingSource_Fails();

	// Lifecycle
	void testAcceptRequest_EstablishesRelationship();
	void testAcceptRequest_SameTenant_Fails();
	void testAcceptRequest_Expired_Fails();
	void testAcceptConnectCode_Success();
	void testAcceptConnectCode_Unknown_Fails();
	void testRejectRequest_Pending();
	void testRevokeRequest_Pending();
	void testRejectRequest_NonPending_Fails();

private:
	imtauth::CMockTenantConnectionRequestManager* m_managerPtr = nullptr;
};
