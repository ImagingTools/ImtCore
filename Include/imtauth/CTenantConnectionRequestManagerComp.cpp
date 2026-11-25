// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionRequestManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QCryptographicHash>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/CollectionSelection.h>
#include <imtauth/TenantEvents.h>


namespace imtauth
{


using namespace CollectionSelection;


// --- DB Helper Methods ---

bool CTenantConnectionRequestManagerComp::StoreConnectionCode(const QByteArray& tenantId, const ITenantConnectionCodeInfo& codeInfo)
{
	if (!m_connectionCodeCollectionCompPtr.IsValid()){
		return false;
	}

	if (!m_connectionCodeFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionCodeInfoUniquePtr dataPtr = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}

	dataPtr->SetTenantId(codeInfo.GetTenantId());
	dataPtr->SetConnectionCode(codeInfo.GetConnectionCode());
	dataPtr->SetAllowConnectionsByCode(codeInfo.GetAllowConnectionsByCode());
	dataPtr->SetCreatedAt(codeInfo.GetCreatedAt());

	imtbase::IObjectCollection::DataPtr existingDataPtr;
	if (m_connectionCodeCollectionCompPtr->GetObjectData(tenantId, existingDataPtr)){
		return m_connectionCodeCollectionCompPtr->SetObjectData(tenantId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_connectionCodeCollectionCompPtr->InsertNewObject("TenantConnectionCode", QString(), QString(), dataPtr.GetPtr(), tenantId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreConnectionRequest(const ITenantConnectionRequestInfo& requestInfo)
{
	if (!m_requestCollectionCompPtr.IsValid() || !m_requestFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionRequestInfoUniquePtr dataPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	if (!dataPtr->CopyFrom(requestInfo)){
		return false;
	}

	QByteArray requestId = requestInfo.GetRequestId();

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_requestCollectionCompPtr->GetObjectData(requestId, existingPtr)){
		return m_requestCollectionCompPtr->SetObjectData(requestId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_requestCollectionCompPtr->InsertNewObject("TenantConnectionRequest", QString(), QString(), dataPtr.GetPtr(), requestId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreConnection(const ITenantConnectionInfo& connectionInfo)
{
	if (!m_connectionCollectionCompPtr.IsValid() || !m_connectionFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionInfoUniquePtr dataPtr = m_connectionFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetConnectionId(connectionInfo.GetConnectionId());
	dataPtr->SetTenantAId(connectionInfo.GetTenantAId());
	dataPtr->SetTenantBId(connectionInfo.GetTenantBId());
	dataPtr->SetStatus(connectionInfo.GetStatus());
	dataPtr->SetCreatedAt(connectionInfo.GetCreatedAt());
	dataPtr->SetUpdatedAt(connectionInfo.GetUpdatedAt());

	QByteArray connectionId = connectionInfo.GetConnectionId();

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_connectionCollectionCompPtr->GetObjectData(connectionId, existingPtr)){
		return m_connectionCollectionCompPtr->SetObjectData(connectionId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_connectionCollectionCompPtr->InsertNewObject("TenantConnection", QString(), QString(), dataPtr.GetPtr(), connectionId);
	return !storedId.isEmpty();
}


// --- Helpers ---

QString CTenantConnectionRequestManagerComp::GenerateConnectionCode() const
{
	// Generate a short, human-readable code (12 chars uppercase alphanumeric)
	QByteArray raw = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray hash = QCryptographicHash::hash(raw, QCryptographicHash::Sha256).toHex().left(12).toUpper();
	return QString::fromUtf8(hash);
}


QByteArray CTenantConnectionRequestManagerComp::FindTenantByConnectionCode(const QString& connectionCode) const
{
	if (!m_connectionCodeCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	for (const QByteArray& id : SelectIds(*m_connectionCodeCollectionCompPtr, ColumnEquals("ConnectionCode", connectionCode.toUtf8()), {"ConnectionCode"})){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCodeCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionCodeInfo* codePtr = dynamic_cast<const ITenantConnectionCodeInfo*>(dataPtr.GetPtr());
			if (codePtr != nullptr && codePtr->GetConnectionCode() == connectionCode){
				return id;
			}
		}
	}
	return QByteArray();
}


bool CTenantConnectionRequestManagerComp::ConnectionExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const
{
	if (!m_connectionCollectionCompPtr.IsValid()){
		return false;
	}

	for (const QByteArray& id : SelectIds(*m_connectionCollectionCompPtr, PairInEitherOrder("TenantAId", "TenantBId", tenantAId, tenantBId))){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionInfo* connPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
			if (connPtr == nullptr){
				continue;
			}
			if (connPtr->GetStatus() != ITenantConnectionInfo::CS_ACTIVE){
				continue;
			}
			if ((connPtr->GetTenantAId() == tenantAId && connPtr->GetTenantBId() == tenantBId)
			|| (connPtr->GetTenantAId() == tenantBId && connPtr->GetTenantBId() == tenantAId)){
				return true;
			}
		}
	}
	return false;
}


bool CTenantConnectionRequestManagerComp::PendingRequestExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return false;
	}

	for (const QByteArray& id : SelectIds(*m_requestCollectionCompPtr, PairInEitherOrder("SourceTenantId", "TargetTenantId", tenantAId, tenantBId))){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionRequestInfo* reqPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
			if (reqPtr == nullptr){
				continue;
			}
			if (reqPtr->GetStatus() != ITenantConnectionRequestInfo::CRS_PENDING){
				continue;
			}
			if ((reqPtr->GetSourceTenantId() == tenantAId && reqPtr->GetTargetTenantId() == tenantBId)
			|| (reqPtr->GetSourceTenantId() == tenantBId && reqPtr->GetTargetTenantId() == tenantAId)){
				return true;
			}
		}
	}
	return false;
}


QByteArray CTenantConnectionRequestManagerComp::CreateConnection(const QByteArray& tenantAId, const QByteArray& tenantBId)
{
	if (!m_connectionCollectionCompPtr.IsValid() || !m_connectionFactoryCompPtr.IsValid()){
		return QByteArray();
	}

	// Reuse existing pair to avoid violating DB unique constraint on (TenantAId, TenantBId).
	for (const QByteArray& id : SelectIds(*m_connectionCollectionCompPtr, PairInEitherOrder("TenantAId", "TenantBId", tenantAId, tenantBId))){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
			continue;
		}

		const ITenantConnectionInfo* existingPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
		if (existingPtr == nullptr){
			continue;
		}

		const bool samePair =
			(existingPtr->GetTenantAId() == tenantAId && existingPtr->GetTenantBId() == tenantBId)
			|| (existingPtr->GetTenantAId() == tenantBId && existingPtr->GetTenantBId() == tenantAId);
		if (!samePair){
			continue;
		}

		if (existingPtr->GetStatus() == ITenantConnectionInfo::CS_ACTIVE){
			return existingPtr->GetConnectionId();
		}

		ITenantConnectionInfoUniquePtr updatedConnPtr = m_connectionFactoryCompPtr.CreateInstance();
		if (!updatedConnPtr.IsValid()){
			return QByteArray();
		}

		updatedConnPtr->CopyFrom(*existingPtr);
		updatedConnPtr->SetStatus(ITenantConnectionInfo::CS_ACTIVE);
		updatedConnPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
		if (StoreConnection(*updatedConnPtr)){
			return updatedConnPtr->GetConnectionId();
		}
		return QByteArray();
	}

	ITenantConnectionInfoUniquePtr connPtr = m_connectionFactoryCompPtr.CreateInstance();
	if (!connPtr.IsValid()){
		return QByteArray();
	}

	QByteArray connectionId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	connPtr->SetConnectionId(connectionId);
	// Store in canonical order (smaller ID first)
	if (tenantAId < tenantBId){
		connPtr->SetTenantAId(tenantAId);
		connPtr->SetTenantBId(tenantBId);
	} else {
		connPtr->SetTenantAId(tenantBId);
		connPtr->SetTenantBId(tenantAId);
	}
	connPtr->SetStatus(ITenantConnectionInfo::CS_ACTIVE);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	connPtr->SetCreatedAt(now);
	connPtr->SetUpdatedAt(now);

	if (StoreConnection(*connPtr)){
		return connectionId;
	}
	return QByteArray();
}


ITenantConnectionCodeInfoUniquePtr CTenantConnectionRequestManagerComp::GetConnectionCodeObject(const QByteArray& tenantId)
{
	if (!m_connectionCodeCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_connectionCodeCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		return nullptr;
	}

	const ITenantConnectionCodeInfo* tenantCodePtr = dynamic_cast<const ITenantConnectionCodeInfo*>(dataPtr.GetPtr());
	if (tenantCodePtr == nullptr){
		return nullptr;
	}

	ITenantConnectionCodeInfoUniquePtr clonedTenant = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!clonedTenant.IsValid()){
		return nullptr;
	}

	if (!clonedTenant->CopyFrom(*tenantCodePtr)){
		return nullptr;
	}

	return clonedTenant;
}


void CTenantConnectionRequestManagerComp::EnsureConnectionCode(const QByteArray& tenantId)
{
	if (!m_connectionCodeCollectionCompPtr.IsValid()){
		return;
	}

	if (!m_connectionCodeFactoryCompPtr.IsValid()){
		return;
	}

	if (tenantId.isEmpty()){
		return;
	}

	imtbase::IObjectCollection::DataPtr existingDataPtr;
	if (m_connectionCodeCollectionCompPtr->GetObjectData(tenantId, existingDataPtr)){
		return; // Already exists
	}

	// Auto-create connection code for this tenant
	ITenantConnectionCodeInfoUniquePtr codePtr = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!codePtr.IsValid()){
		return;
	}

	codePtr->SetTenantId(tenantId);
	codePtr->SetConnectionCode(GenerateConnectionCode());
	codePtr->SetAllowConnectionsByCode(true);
	codePtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	StoreConnectionCode(tenantId, *codePtr);
}


// --- Connection Code ---

QString CTenantConnectionRequestManagerComp::GetConnectionCode(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return QString();
	}

	EnsureConnectionCode(tenantId);

	ITenantConnectionCodeInfoUniquePtr codePtr = GetConnectionCodeObject(tenantId);
	if (codePtr != nullptr){
		return codePtr->GetConnectionCode();
	}

	return QString();
}


bool CTenantConnectionRequestManagerComp::GetAllowConnectionsByCode(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return true;
	}

	EnsureConnectionCode(tenantId);

	ITenantConnectionCodeInfoUniquePtr codePtr = GetConnectionCodeObject(tenantId);
	if (codePtr != nullptr){
		return codePtr->GetAllowConnectionsByCode();
	}
	return true;
}


QString CTenantConnectionRequestManagerComp::RegenerateConnectionCode(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return QString();
	}

	EnsureConnectionCode(tenantId);

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionCodeInfoUniquePtr codePtr = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!codePtr.IsValid()){
		return QString();
	}

	ITenantConnectionCodeInfoUniquePtr existingPtr = GetConnectionCodeObject(tenantId);
	if (existingPtr.IsValid()){
		codePtr->SetTenantId(existingPtr->GetTenantId());
		codePtr->SetAllowConnectionsByCode(existingPtr->GetAllowConnectionsByCode());
		codePtr->SetCreatedAt(existingPtr->GetCreatedAt());
	}
	else{
		codePtr->SetTenantId(tenantId);
		codePtr->SetAllowConnectionsByCode(true);
		codePtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	}
	codePtr->SetConnectionCode(GenerateConnectionCode());

	StoreConnectionCode(tenantId, *codePtr);

	return codePtr->GetConnectionCode();
}


bool CTenantConnectionRequestManagerComp::SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow)
{
	if (tenantId.isEmpty()){
		return false;
	}

	EnsureConnectionCode(tenantId);

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionCodeInfoUniquePtr codePtr = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!codePtr.IsValid()){
		return false;
	}

	ITenantConnectionCodeInfoUniquePtr existingPtr = GetConnectionCodeObject(tenantId);
	if (existingPtr.IsValid()){
		codePtr->SetTenantId(existingPtr->GetTenantId());
		codePtr->SetConnectionCode(existingPtr->GetConnectionCode());
		codePtr->SetCreatedAt(existingPtr->GetCreatedAt());
	}
	else{
		codePtr->SetTenantId(tenantId);
		codePtr->SetConnectionCode(GenerateConnectionCode());
		codePtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	}
	codePtr->SetAllowConnectionsByCode(allow);

	return StoreConnectionCode(tenantId, *codePtr);
}


// --- Connection Requests ---

QByteArray CTenantConnectionRequestManagerComp::CreateConnectionRequest(
		const QByteArray& sourceTenantId,
		const QString& connectionCode,
		const QString& message,
		QString* errorMessagePtr)
{
	if (sourceTenantId.isEmpty() || connectionCode.isEmpty()){
		return RefuseRequest(QStringLiteral("Source organization and connection code are required"), errorMessagePtr);
	}

	const QByteArray targetTenantId = FindTenantByConnectionCode(connectionCode);
	if (targetTenantId.isEmpty()){
		return RefuseRequest(QStringLiteral("No organization has this connection code"), errorMessagePtr);
	}

	if (!GetAllowConnectionsByCode(targetTenantId)){
		return RefuseRequest(QStringLiteral("The target organization does not accept connections by code"), errorMessagePtr);
	}

	return CreateRequest(sourceTenantId, targetTenantId, connectionCode, message, errorMessagePtr);
}


QByteArray CTenantConnectionRequestManagerComp::CreateConnectionRequestToTenant(
			const QByteArray& sourceTenantId,
			const QByteArray& targetTenantId,
			const QString& message,
			QString* errorMessagePtr)
{
	if (sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
		return RefuseRequest(QStringLiteral("Source and target organizations are required"), errorMessagePtr);
	}

	ITenantDirectoryProfileUniquePtr profilePtr = m_directoryCompPtr.IsValid() ? m_directoryCompPtr->GetProfile(targetTenantId) : nullptr;
	if (!profilePtr.IsValid() || !profilePtr->IsListed()){
		return RefuseRequest(QStringLiteral("The organization is not listed in the directory"), errorMessagePtr);
	}

	return CreateRequest(sourceTenantId, targetTenantId, QString(), message, errorMessagePtr);
}


QByteArray CTenantConnectionRequestManagerComp::RefuseRequest(const QString& reason, QString* errorMessagePtr) const
{
	SendErrorMessage(0, reason, "CTenantConnectionRequestManagerComp");
	if (errorMessagePtr != nullptr){
		*errorMessagePtr = reason;
	}

	return QByteArray();
}


QByteArray CTenantConnectionRequestManagerComp::CreateRequest(
			const QByteArray& sourceTenantId,
			const QByteArray& targetTenantId,
			const QString& connectionCode,
			const QString& message,
			QString* errorMessagePtr)
{
	auto refuse = [this, errorMessagePtr](const QString& reason){
		return RefuseRequest(reason, errorMessagePtr);
	};

	if (m_tenantManagerCompPtr.IsValid()){
		ITenantInfoUniquePtr sourceTenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
		if (!sourceTenantPtr.IsValid()){
			return refuse(QStringLiteral("Source organization not found"));
		}

		ITenantInfoUniquePtr targetTenantPtr = m_tenantManagerCompPtr->GetTenant(targetTenantId);
		if (!targetTenantPtr.IsValid()){
			return refuse(QStringLiteral("Target organization not found"));
		}
	}

	if (targetTenantId == sourceTenantId){
		return refuse(QStringLiteral("An organization does not connect to itself"));
	}

	if (ConnectionExists(sourceTenantId, targetTenantId)){
		return refuse(QStringLiteral("The organizations are already connected"));
	}

	// In either direction
	if (PendingRequestExists(sourceTenantId, targetTenantId)){
		return refuse(QStringLiteral("A connection request between these organizations is already pending"));
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionRequestInfoUniquePtr reqPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!reqPtr.IsValid()){
		return QByteArray();
	}

	QByteArray requestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	reqPtr->SetRequestId(requestId);
	reqPtr->SetSourceTenantId(sourceTenantId);
	reqPtr->SetTargetTenantId(targetTenantId);
	reqPtr->SetConnectionCode(connectionCode);
	reqPtr->SetMessage(message);
	reqPtr->SetStatus(ITenantConnectionRequestInfo::CRS_PENDING);
	reqPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	// Resolve tenant names so they display correctly in the UI
	if (m_tenantManagerCompPtr.IsValid()){
		ITenantInfoUniquePtr sourceTenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
		if (sourceTenantPtr.IsValid()){
			reqPtr->SetSourceTenantName(sourceTenantPtr->GetTenantName());
		}
		ITenantInfoUniquePtr targetTenantPtr = m_tenantManagerCompPtr->GetTenant(targetTenantId);
		if (targetTenantPtr.IsValid()){
			reqPtr->SetTargetTenantName(targetTenantPtr->GetTenantName());
		}
	}

	if (StoreConnectionRequest(*reqPtr)){
		TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_connectionRequestCreated},
				{TenantEvent::s_requestId, reqPtr->GetRequestId()},
				{TenantEvent::s_sourceTenantId, reqPtr->GetSourceTenantId()},
				{TenantEvent::s_targetTenantId, reqPtr->GetTargetTenantId()},
				{TenantEvent::s_status, int(reqPtr->GetStatus())}});

		return requestId;
	}
	return QByteArray();
}


QByteArray CTenantConnectionRequestManagerComp::ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId)
{
	if (requestId.isEmpty() || approvingTenantId.isEmpty() || !m_requestCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return QByteArray();
	}

	const ITenantConnectionRequestInfo* reqPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return QByteArray();
	}

	if (reqPtr->GetStatus() != ITenantConnectionRequestInfo::CRS_PENDING){
		SendErrorMessage(0, "Request is not in pending state", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}
	if (reqPtr->GetTargetTenantId() != approvingTenantId){
		SendErrorMessage(0, "Only the target tenant can approve this request", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray connectionId;

	// If a connection already exists (e.g. duplicate approve from another client),
	// reuse it instead of failing the approval flow.
	if (m_connectionCollectionCompPtr.IsValid()){
		for (const QByteArray& id : SelectIds(*m_connectionCollectionCompPtr, PairInEitherOrder("TenantAId", "TenantBId", reqPtr->GetSourceTenantId(), reqPtr->GetTargetTenantId()))){
			imtbase::IObjectCollection::DataPtr connDataPtr;
			if (!m_connectionCollectionCompPtr->GetObjectData(id, connDataPtr)){
				continue;
			}

			const ITenantConnectionInfo* connPtr = dynamic_cast<const ITenantConnectionInfo*>(connDataPtr.GetPtr());
			if (connPtr == nullptr || connPtr->GetStatus() != ITenantConnectionInfo::CS_ACTIVE){
				continue;
			}

			const bool samePair =
				(connPtr->GetTenantAId() == reqPtr->GetSourceTenantId() && connPtr->GetTenantBId() == reqPtr->GetTargetTenantId())
				|| (connPtr->GetTenantAId() == reqPtr->GetTargetTenantId() && connPtr->GetTenantBId() == reqPtr->GetSourceTenantId());
			if (samePair){
				connectionId = connPtr->GetConnectionId();
				break;
			}
		}
	}

	if (connectionId.isEmpty()){
		connectionId = CreateConnection(reqPtr->GetSourceTenantId(), reqPtr->GetTargetTenantId());
	}

	if (connectionId.isEmpty()){
		SendErrorMessage(0, "Failed to create or resolve connection for approved request", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Update the request status
	ITenantConnectionRequestInfoUniquePtr updatedReqPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!updatedReqPtr.IsValid()){
		return QByteArray();
	}
	updatedReqPtr->CopyFrom(*reqPtr);
	updatedReqPtr->SetStatus(ITenantConnectionRequestInfo::CRS_APPROVED);
	updatedReqPtr->SetRespondedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	if (!StoreConnectionRequest(*updatedReqPtr)){
		SendErrorMessage(0, "Failed to update request status to approved", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_connectionRequestStatusChanged},
				{TenantEvent::s_requestId, updatedReqPtr->GetRequestId()},
				{TenantEvent::s_sourceTenantId, updatedReqPtr->GetSourceTenantId()},
				{TenantEvent::s_targetTenantId, updatedReqPtr->GetTargetTenantId()},
				{TenantEvent::s_status, int(updatedReqPtr->GetStatus())}});

	return connectionId;
}


bool CTenantConnectionRequestManagerComp::RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return false;
	}

	const ITenantConnectionRequestInfo* reqPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return false;
	}

	if (reqPtr->GetTargetTenantId() != tenantId || reqPtr->GetStatus() != ITenantConnectionRequestInfo::CRS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionRequestInfoUniquePtr updatedReqPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!updatedReqPtr.IsValid()){
		return false;
	}
	updatedReqPtr->CopyFrom(*reqPtr);
	updatedReqPtr->SetStatus(ITenantConnectionRequestInfo::CRS_REJECTED);
	updatedReqPtr->SetRespondedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	if (!StoreConnectionRequest(*updatedReqPtr)){
		return false;
	}

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_connectionRequestStatusChanged},
				{TenantEvent::s_requestId, updatedReqPtr->GetRequestId()},
				{TenantEvent::s_sourceTenantId, updatedReqPtr->GetSourceTenantId()},
				{TenantEvent::s_targetTenantId, updatedReqPtr->GetTargetTenantId()},
				{TenantEvent::s_status, int(updatedReqPtr->GetStatus())}});

	return true;
}


bool CTenantConnectionRequestManagerComp::CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
	if (!m_requestCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return false;
	}

	const ITenantConnectionRequestInfo* reqPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return false;
	}

	if (reqPtr->GetSourceTenantId() != tenantId || reqPtr->GetStatus() != ITenantConnectionRequestInfo::CRS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionRequestInfoUniquePtr updatedReqPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!updatedReqPtr.IsValid()){
		return false;
	}
	updatedReqPtr->CopyFrom(*reqPtr);
	updatedReqPtr->SetStatus(ITenantConnectionRequestInfo::CRS_CANCELED);
	updatedReqPtr->SetRespondedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	if (!StoreConnectionRequest(*updatedReqPtr)){
		return false;
	}

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_connectionRequestStatusChanged},
				{TenantEvent::s_requestId, updatedReqPtr->GetRequestId()},
				{TenantEvent::s_sourceTenantId, updatedReqPtr->GetSourceTenantId()},
				{TenantEvent::s_targetTenantId, updatedReqPtr->GetTargetTenantId()},
				{TenantEvent::s_status, int(updatedReqPtr->GetStatus())}});

	return true;
}


QByteArrayList CTenantConnectionRequestManagerComp::GetConnectionRequestIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_requestCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : SelectIds(*m_requestCollectionCompPtr, AnyColumnEquals("SourceTenantId", "TargetTenantId", tenantId))){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionRequestInfo* reqPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
			if (reqPtr != nullptr){
				if (reqPtr->GetSourceTenantId() == tenantId || reqPtr->GetTargetTenantId() == tenantId){
					result.append(reqPtr->GetRequestId());
				}
			}
		}
	}
	return result;
}


// --- Connections ---

ITenantConnectionRequestInfoUniquePtr CTenantConnectionRequestManagerComp::GetConnectionRequest(const QByteArray& requestId) const
{
	if (!m_requestCollectionCompPtr.IsValid() || requestId.isEmpty()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
		return nullptr;
	}

	const ITenantConnectionRequestInfo* tenantConnectionRequestInfoPtr = dynamic_cast<const ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
	if (tenantConnectionRequestInfoPtr == nullptr){
		return nullptr;
	}

	ITenantConnectionRequestInfoUniquePtr clonedConnectionRequestInfo = m_requestFactoryCompPtr.CreateInstance();
	if (!clonedConnectionRequestInfo.IsValid()){
		return nullptr;
	}

	if (!clonedConnectionRequestInfo->CopyFrom(*tenantConnectionRequestInfoPtr)){
		return nullptr;
	}

	return clonedConnectionRequestInfo;
}


QByteArrayList CTenantConnectionRequestManagerComp::GetConnectionIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_connectionCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : SelectIds(*m_connectionCollectionCompPtr, AnyColumnEquals("TenantAId", "TenantBId", tenantId))){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionInfo* connPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
			if (connPtr != nullptr && connPtr->GetStatus() == ITenantConnectionInfo::CS_ACTIVE
			&& (connPtr->GetTenantAId() == tenantId || connPtr->GetTenantBId() == tenantId)){
				result.append(connPtr->GetConnectionId());
			}
		}
	}

	return result;
}


ITenantConnectionInfoUniquePtr CTenantConnectionRequestManagerComp::GetConnection(const QByteArray& connectionId) const
{
	if (!m_connectionCollectionCompPtr.IsValid() || connectionId.isEmpty()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_connectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
		return nullptr;
	}

	const ITenantConnectionInfo* tenantConnectionInfoPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
	if (tenantConnectionInfoPtr == nullptr){
		return nullptr;
	}

	ITenantConnectionInfoUniquePtr clonedConnectionInfo = m_connectionFactoryCompPtr.CreateInstance();
	if (!clonedConnectionInfo.IsValid()){
		return nullptr;
	}

	if (!clonedConnectionInfo->CopyFrom(*tenantConnectionInfoPtr)){
		return nullptr;
	}

	return clonedConnectionInfo;
}


bool CTenantConnectionRequestManagerComp::RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId)
{
	QMutexLocker locker(&m_connectionChangeMutex);

	if (!m_connectionCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_connectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
		return false;
	}

	const ITenantConnectionInfo* connPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
	if (connPtr == nullptr){
		return false;
	}

	if (connPtr->GetTenantAId() != tenantId && connPtr->GetTenantBId() != tenantId){
		return false;
	}
	if (connPtr->GetStatus() != ITenantConnectionInfo::CS_ACTIVE){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantConnectionInfoUniquePtr updatedConnPtr = m_connectionFactoryCompPtr.CreateInstance();
	if (!updatedConnPtr.IsValid()){
		return false;
	}
	updatedConnPtr->CopyFrom(*connPtr);
	updatedConnPtr->SetStatus(ITenantConnectionInfo::CS_REMOVED);
	updatedConnPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	// The contracts of the organizations stay: a connection is their address book only
	if (!StoreConnection(*updatedConnPtr)){
		return false;
	}

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_connectionRemoved},
				{TenantEvent::s_connectionId, updatedConnPtr->GetConnectionId()},
				{TenantEvent::s_sourceTenantId, updatedConnPtr->GetTenantAId()},
				{TenantEvent::s_targetTenantId, updatedConnPtr->GetTenantBId()}});

	return true;
}


bool CTenantConnectionRequestManagerComp::AreConnected(const QByteArray& tenantId, const QByteArray& otherTenantId) const
{
	return !tenantId.isEmpty() && !otherTenantId.isEmpty() && ConnectionExists(tenantId, otherTenantId);
}


QByteArray CTenantConnectionRequestManagerComp::EnsureConnection(const QByteArray& tenantId, const QByteArray& otherTenantId)
{
	if (tenantId.isEmpty() || otherTenantId.isEmpty() || tenantId == otherTenantId){
		return QByteArray();
	}

	QMutexLocker locker(&m_connectionChangeMutex);

	istd::CChangeNotifier changeNotifier(this);

	return CreateConnection(tenantId, otherTenantId);
}


} // namespace imtauth

