// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionRequestManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


namespace
{


QString GenerateConnectCode()
{
	// A reasonably short, human-transferable one-time code.
	QByteArray raw = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray compact = raw.replace('-', "").left(12).toUpper();
	return QString::fromUtf8(compact);
}


} // anonymous namespace


QByteArray CTenantConnectionRequestManagerComp::StoreRequest(const TenantConnectionRequestInfo& info)
{
	if (!m_requestCollectionCompPtr.IsValid() || !m_requestFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Connection request collection or factory not configured", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	ITenantConnectionRequestDataUniquePtr requestPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!requestPtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	requestPtr->SetRequestInfo(info);

	QByteArray storedId = m_requestCollectionCompPtr->InsertNewObject(
				"TenantConnectionRequest", QString(), QString(), requestPtr.GetPtr(), info.requestId);

	return storedId.isEmpty() ? QByteArray() : info.requestId;
}


QByteArray CTenantConnectionRequestManagerComp::CreateConnectionRequest(
		const QByteArray& sourceTenantId,
		const QString& targetIdentifier,
		ITenantInfo::TenantRelationshipRole proposedSourceRole,
		ITenantInfo::TenantRelationshipRole proposedTargetRole,
		const QString& message,
		const QString& expiresAt)
{
	if (sourceTenantId.isEmpty()){
		SendErrorMessage(0, "Source tenant is required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	if (targetIdentifier.isEmpty()){
		SendErrorMessage(0, "Target identifier is required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	TenantConnectionRequestInfo info;
	info.requestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	info.sourceTenantId = sourceTenantId;
	info.targetIdentifier = targetIdentifier;
	info.proposedSourceRole = proposedSourceRole;
	info.proposedTargetRole = proposedTargetRole;
	info.message = message;
	info.status = TCS_PENDING;
	info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	info.expiresAt = expiresAt;

	return StoreRequest(info);
}


QByteArray CTenantConnectionRequestManagerComp::CreateConnectCode(
		const QByteArray& sourceTenantId,
		ITenantInfo::TenantRelationshipRole proposedSourceRole,
		ITenantInfo::TenantRelationshipRole proposedTargetRole,
		const QString& message,
		const QString& expiresAt,
		QString& generatedCode)
{
	if (sourceTenantId.isEmpty()){
		SendErrorMessage(0, "Source tenant is required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	generatedCode = GenerateConnectCode();

	TenantConnectionRequestInfo info;
	info.requestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	info.sourceTenantId = sourceTenantId;
	info.connectCode = generatedCode;
	info.proposedSourceRole = proposedSourceRole;
	info.proposedTargetRole = proposedTargetRole;
	info.message = message;
	info.status = TCS_PENDING;
	info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	info.expiresAt = expiresAt;

	QByteArray storedId = StoreRequest(info);
	if (storedId.isEmpty()){
		generatedCode.clear();
	}

	return storedId;
}


bool CTenantConnectionRequestManagerComp::IsRequestPending(const TenantConnectionRequestInfo& info) const
{
	if (info.status != TCS_PENDING){
		return false;
	}

	if (!info.expiresAt.isEmpty()){
		QDateTime expiresAt = QDateTime::fromString(info.expiresAt, Qt::ISODateWithMs);
		if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
			return false;
		}
	}

	return true;
}


bool CTenantConnectionRequestManagerComp::EstablishRelationship(
		const TenantConnectionRequestInfo& info,
		const QByteArray& acceptingTenantId)
{
	if (!m_tenantManagerCompPtr.IsValid()){
		// Relationship creation is optional: the request can still be marked accepted.
		return true;
	}

	if (info.sourceTenantId.isEmpty() || acceptingTenantId.isEmpty()){
		return false;
	}

	// Source tenant keeps a relationship pointing at the accepting tenant ...
	QByteArray sourceRel = m_tenantManagerCompPtr->AddTenantRelationship(
				info.sourceTenantId,
				acceptingTenantId,
				info.proposedTargetRole,
				info.proposedSourceRole,
				info.proposedTargetRole,
				QString(),
				info.createdAt,
				QString(),
				info.message);

	// ... and the accepting tenant gets the mirror entry with swapped roles.
	QByteArray targetRel = m_tenantManagerCompPtr->AddTenantRelationship(
				acceptingTenantId,
				info.sourceTenantId,
				info.proposedSourceRole,
				info.proposedTargetRole,
				info.proposedSourceRole,
				QString(),
				info.createdAt,
				QString(),
				info.message);

	return !sourceRel.isEmpty() && !targetRel.isEmpty();
}


bool CTenantConnectionRequestManagerComp::AcceptConnectionRequest(const QByteArray& requestId, const QByteArray& acceptingTenantId)
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return false;
	}

	ITenantConnectionRequestData* requestPtr = dynamic_cast<ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (requestPtr == nullptr){
		return false;
	}

	TenantConnectionRequestInfo info = requestPtr->GetRequestInfo();
	if (!IsRequestPending(info)){
		return false;
	}

	if (acceptingTenantId.isEmpty() || acceptingTenantId == info.sourceTenantId){
		return false;
	}

	if (!EstablishRelationship(info, acceptingTenantId)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = TCS_ACCEPTED;
	info.targetTenantId = acceptingTenantId;
	info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	requestPtr->SetRequestInfo(info);

	return m_requestCollectionCompPtr->SetObjectData(requestId, *requestPtr);
}


QByteArray CTenantConnectionRequestManagerComp::AcceptConnectCode(const QString& connectCode, const QByteArray& acceptingTenantId)
{
	if (connectCode.isEmpty()){
		return QByteArray();
	}

	TenantConnectionRequestInfo info = GetRequestByCode(connectCode);
	if (info.requestId.isEmpty()){
		return QByteArray();
	}

	if (!AcceptConnectionRequest(info.requestId, acceptingTenantId)){
		return QByteArray();
	}

	return info.requestId;
}


bool CTenantConnectionRequestManagerComp::UpdateRequestStatus(const QByteArray& requestId, TenantConnectionStatus status)
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return false;
	}

	ITenantConnectionRequestData* requestPtr = dynamic_cast<ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (requestPtr == nullptr){
		return false;
	}

	TenantConnectionRequestInfo info = requestPtr->GetRequestInfo();
	if (info.status != TCS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = status;
	info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	requestPtr->SetRequestInfo(info);

	return m_requestCollectionCompPtr->SetObjectData(requestId, *requestPtr);
}


bool CTenantConnectionRequestManagerComp::RejectConnectionRequest(const QByteArray& requestId)
{
	return UpdateRequestStatus(requestId, TCS_REJECTED);
}


bool CTenantConnectionRequestManagerComp::RevokeConnectionRequest(const QByteArray& requestId)
{
	return UpdateRequestStatus(requestId, TCS_REVOKED);
}


TenantConnectionRequestInfo CTenantConnectionRequestManagerComp::GetConnectionRequest(const QByteArray& requestId) const
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return TenantConnectionRequestInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return TenantConnectionRequestInfo();
	}

	const ITenantConnectionRequestData* requestPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (requestPtr == nullptr){
		return TenantConnectionRequestInfo();
	}

	return requestPtr->GetRequestInfo();
}


TenantConnectionRequests CTenantConnectionRequestManagerComp::CollectRequests(
		const std::function<bool(const TenantConnectionRequestInfo&)>& predicate) const
{
	TenantConnectionRequests result;
	if (!m_requestCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionRequestData* requestPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
			if (requestPtr != nullptr){
				TenantConnectionRequestInfo info = requestPtr->GetRequestInfo();
				if (predicate(info)){
					result.append(info);
				}
			}
		}
	}

	return result;
}


TenantConnectionRequests CTenantConnectionRequestManagerComp::GetOutgoingRequests(const QByteArray& sourceTenantId) const
{
	return CollectRequests([&sourceTenantId](const TenantConnectionRequestInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


TenantConnectionRequests CTenantConnectionRequestManagerComp::GetIncomingRequests(
		const QByteArray& targetTenantId,
		const QString& targetIdentifier) const
{
	return CollectRequests([&targetTenantId, &targetIdentifier](const TenantConnectionRequestInfo& info){
		if (!targetTenantId.isEmpty() && info.targetTenantId == targetTenantId){
			return true;
		}
		if (!targetIdentifier.isEmpty() && info.targetIdentifier == targetIdentifier){
			return true;
		}
		return false;
	});
}


TenantConnectionRequestInfo CTenantConnectionRequestManagerComp::GetRequestByCode(const QString& connectCode) const
{
	if (connectCode.isEmpty()){
		return TenantConnectionRequestInfo();
	}

	const TenantConnectionRequests requests = CollectRequests([&connectCode](const TenantConnectionRequestInfo& info){
		return !info.connectCode.isEmpty() && info.connectCode == connectCode;
	});

	if (requests.isEmpty()){
		return TenantConnectionRequestInfo();
	}

	return requests.first();
}


} // namespace imtauth
