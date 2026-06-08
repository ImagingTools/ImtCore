// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionRequestManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QCryptographicHash>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


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

	const QByteArrayList elementIds = m_connectionCodeCollectionCompPtr->GetElementIds();
	if (elementIds.contains(tenantId)){
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
	dataPtr->SetRequestId(requestInfo.GetRequestId());
	dataPtr->SetSourceTenantId(requestInfo.GetSourceTenantId());
	dataPtr->SetTargetTenantId(requestInfo.GetTargetTenantId());
	dataPtr->SetConnectionCode(requestInfo.GetConnectionCode());
	dataPtr->SetMessage(requestInfo.GetMessage());
	dataPtr->SetStatus(requestInfo.GetStatus());
	dataPtr->SetCreatedAt(requestInfo.GetCreatedAt());
	dataPtr->SetRespondedAt(requestInfo.GetRespondedAt());
	dataPtr->SetSourceTenantName(requestInfo.GetSourceTenantName());
	dataPtr->SetTargetTenantName(requestInfo.GetTargetTenantName());

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


bool CTenantConnectionRequestManagerComp::StoreProposal(const QByteArray& proposalId, const ITenantRelationshipProposalInfo& proposalInfo)
{
	if (!m_proposalCollectionCompPtr.IsValid() || !m_proposalFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantRelationshipProposalInfoUniquePtr dataPtr = m_proposalFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetProposalId(proposalInfo.GetProposalId());
	dataPtr->SetConnectionId(proposalInfo.GetConnectionId());
	dataPtr->SetExistingRelationshipId(proposalInfo.GetExistingRelationshipId());
	dataPtr->SetProposalType(proposalInfo.GetProposalType());
	dataPtr->SetInitiatorTenantId(proposalInfo.GetInitiatorTenantId());
	dataPtr->SetCounterpartyTenantId(proposalInfo.GetCounterpartyTenantId());
	dataPtr->SetProposedSourceRole(proposalInfo.GetProposedSourceRole());
	dataPtr->SetProposedTargetRole(proposalInfo.GetProposedTargetRole());
	dataPtr->SetProposedScope(proposalInfo.GetProposedScope());
	dataPtr->SetProposedDescription(proposalInfo.GetProposedDescription());
	dataPtr->SetProposedValidFrom(proposalInfo.GetProposedValidFrom());
	dataPtr->SetProposedValidUntil(proposalInfo.GetProposedValidUntil());
	dataPtr->SetStatus(proposalInfo.GetStatus());
	dataPtr->SetMessage(proposalInfo.GetMessage());
	dataPtr->SetCreatedAt(proposalInfo.GetCreatedAt());
	dataPtr->SetUpdatedAt(proposalInfo.GetUpdatedAt());

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_proposalCollectionCompPtr->GetObjectData(proposalId, existingPtr)){
		return m_proposalCollectionCompPtr->SetObjectData(proposalId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_proposalCollectionCompPtr->InsertNewObject("TenantRelationshipProposal", QString(), QString(), dataPtr.GetPtr(), proposalId);
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

	for (const QByteArray& id : m_connectionCodeCollectionCompPtr->GetElementIds()){
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

	for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
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

	for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
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
	for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
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


void CTenantConnectionRequestManagerComp::ArchiveRelationshipsForConnection(const QByteArray& connectionId)
{
	if (!m_relationshipCollectionCompPtr.IsValid() || !m_relationshipFactoryCompPtr.IsValid()){
		return;
	}

	for (const QByteArray& id : m_relationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr && relPtr->GetConnectionId() == connectionId
			&& relPtr->GetStatus() == ITenantRelationshipInfo::TRS_ACTIVE){
				istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
				if (updatedPtr.IsValid()){
					updatedPtr->CopyFrom(*relPtr);
					updatedPtr->SetStatus(ITenantRelationshipInfo::TRS_ARCHIVED);
					updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
					m_relationshipCollectionCompPtr->SetObjectData(id, *updatedPtr);
				}
			}
		}
	}
}


QByteArray CTenantConnectionRequestManagerComp::ApplyRelationshipProposal(const ITenantRelationshipProposalInfo* proposalPtr)
{
	if (!m_relationshipCollectionCompPtr.IsValid() || !m_relationshipFactoryCompPtr.IsValid() || proposalPtr == nullptr){
		return QByteArray();
	}

	if (proposalPtr->GetProposalType() == ITenantRelationshipProposalInfo::RPT_CREATE){
		istd::TUniqueInterfacePtr<ITenantRelationshipInfo> relPtr = m_relationshipFactoryCompPtr.CreateInstance();
		if (!relPtr.IsValid()){
			return QByteArray();
		}
		QByteArray relId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		relPtr->SetRelationshipId(relId);
		relPtr->SetConnectionId(proposalPtr->GetConnectionId());
		relPtr->SetSourceTenantId(proposalPtr->GetInitiatorTenantId());
		relPtr->SetTargetTenantId(proposalPtr->GetCounterpartyTenantId());
		relPtr->SetSourceRole(proposalPtr->GetProposedSourceRole());
		relPtr->SetTargetRole(proposalPtr->GetProposedTargetRole());
		relPtr->SetScope(proposalPtr->GetProposedScope());
		relPtr->SetDescription(proposalPtr->GetProposedDescription());
		relPtr->SetValidFrom(proposalPtr->GetProposedValidFrom());
		relPtr->SetValidUntil(proposalPtr->GetProposedValidUntil());
		relPtr->SetStatus(ITenantRelationshipInfo::TRS_ACTIVE);
		relPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

		QByteArray storedId = m_relationshipCollectionCompPtr->InsertNewObject("TenantRelationship", QString(), QString(), relPtr.GetPtr(), relId);
		if (!storedId.isEmpty()){
			return relId;
		}
		return QByteArray();
	}
	else if (proposalPtr->GetProposalType() == ITenantRelationshipProposalInfo::RPT_UPDATE){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_relationshipCollectionCompPtr->GetObjectData(proposalPtr->GetExistingRelationshipId(), dataPtr)){
			return QByteArray();
		}
		const ITenantRelationshipInfo* existingPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
		if (existingPtr == nullptr){
			return QByteArray();
		}
		istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
		if (!updatedPtr.IsValid()){
			return QByteArray();
		}
		updatedPtr->CopyFrom(*existingPtr);
		updatedPtr->SetSourceRole(proposalPtr->GetProposedSourceRole());
		updatedPtr->SetTargetRole(proposalPtr->GetProposedTargetRole());
		updatedPtr->SetScope(proposalPtr->GetProposedScope());
		updatedPtr->SetDescription(proposalPtr->GetProposedDescription());
		updatedPtr->SetValidFrom(proposalPtr->GetProposedValidFrom());
		updatedPtr->SetValidUntil(proposalPtr->GetProposedValidUntil());
		updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
		if (m_relationshipCollectionCompPtr->SetObjectData(proposalPtr->GetExistingRelationshipId(), *updatedPtr)){
			return proposalPtr->GetExistingRelationshipId();
		}
		return QByteArray();
	}
	else if (proposalPtr->GetProposalType() == ITenantRelationshipProposalInfo::RPT_DELETE){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_relationshipCollectionCompPtr->GetObjectData(proposalPtr->GetExistingRelationshipId(), dataPtr)){
			return QByteArray();
		}
		const ITenantRelationshipInfo* existingPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
		if (existingPtr == nullptr){
			return QByteArray();
		}
		istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
		if (!updatedPtr.IsValid()){
			return QByteArray();
		}
		updatedPtr->CopyFrom(*existingPtr);
		updatedPtr->SetStatus(ITenantRelationshipInfo::TRS_ARCHIVED);
		updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
		if (m_relationshipCollectionCompPtr->SetObjectData(proposalPtr->GetExistingRelationshipId(), *updatedPtr)){
			return proposalPtr->GetExistingRelationshipId();
		}
		return QByteArray();
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

	QByteArrayList elementIds = m_connectionCodeCollectionCompPtr->GetElementIds();
	if (elementIds.contains(tenantId)){
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
		const QString& message)
{
	if (sourceTenantId.isEmpty() || connectionCode.isEmpty()){
		SendErrorMessage(0, "Source tenant and connection code are required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Find target tenant by connection code
	QByteArray targetTenantId = FindTenantByConnectionCode(connectionCode);
	if (targetTenantId.isEmpty()){
		SendErrorMessage(0, "Invalid connection code", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Cannot connect to self
	if (targetTenantId == sourceTenantId){
		SendErrorMessage(0, "Cannot create connection request to own organization", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Check if target allows connections by code
	if (!GetAllowConnectionsByCode(targetTenantId)){
		SendErrorMessage(0, "Target organization has disabled connections by code", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Check if connection already exists
	if (ConnectionExists(sourceTenantId, targetTenantId)){
		SendErrorMessage(0, "Connection already exists between these organizations", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Check if a pending request already exists between these tenants (in either direction)
	if (PendingRequestExists(sourceTenantId, targetTenantId)){
		SendErrorMessage(0, "A pending connection request already exists between these organizations", "CTenantConnectionRequestManagerComp");
		return QByteArray();
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
		for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
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
	return StoreConnectionRequest(*updatedReqPtr);
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
	return StoreConnectionRequest(*updatedReqPtr);
}


QByteArrayList CTenantConnectionRequestManagerComp::GetConnectionRequestIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_requestCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
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

	for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
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
	StoreConnection(*updatedConnPtr);

	// Cascade: archive all relationships for this connection
	ArchiveRelationshipsForConnection(connectionId);
	return true;
}


// --- Relationship Proposals ---

QByteArray CTenantConnectionRequestManagerComp::CreateRelationshipProposal(
		const ITenantRelationshipProposalInfo& proposalInfo)
{
	QByteArray connectionId = proposalInfo.GetConnectionId();
	QByteArray initiatorTenantId = proposalInfo.GetInitiatorTenantId();
	QByteArray counterpartyTenantId = proposalInfo.GetCounterpartyTenantId();

	if (connectionId.isEmpty() || initiatorTenantId.isEmpty() || counterpartyTenantId.isEmpty()){
		SendErrorMessage(0, "Connection ID and both tenant IDs are required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Verify connection exists and is active
	bool connectionValid = false;
	if (m_connectionCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
			const ITenantConnectionInfo* connPtr = dynamic_cast<const ITenantConnectionInfo*>(dataPtr.GetPtr());
			if (connPtr != nullptr && connPtr->GetStatus() == ITenantConnectionInfo::CS_ACTIVE){
				connectionValid = true;
			}
		}
	}
	if (!connectionValid){
		SendErrorMessage(0, "No active connection found", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantRelationshipProposalInfoUniquePtr newProposal = m_proposalFactoryCompPtr.CreateInstance();
	if (!newProposal.IsValid()){
		return QByteArray();
	}
	QByteArray newProposalId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	newProposal->SetProposalId(newProposalId);
	newProposal->SetConnectionId(connectionId);
	newProposal->SetInitiatorTenantId(initiatorTenantId);
	newProposal->SetCounterpartyTenantId(counterpartyTenantId);
	newProposal->SetProposalType(proposalInfo.GetProposalType());
	newProposal->SetProposedSourceRole(proposalInfo.GetProposedSourceRole());
	newProposal->SetProposedTargetRole(proposalInfo.GetProposedTargetRole());
	newProposal->SetProposedScope(proposalInfo.GetProposedScope());
	newProposal->SetProposedDescription(proposalInfo.GetProposedDescription());
	newProposal->SetProposedValidFrom(proposalInfo.GetProposedValidFrom());
	newProposal->SetProposedValidUntil(proposalInfo.GetProposedValidUntil());
	newProposal->SetMessage(proposalInfo.GetMessage());
	if (!proposalInfo.GetExistingRelationshipId().isEmpty()){
		newProposal->SetExistingRelationshipId(proposalInfo.GetExistingRelationshipId());
	}
	newProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR); // Initiator auto-approves
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	newProposal->SetCreatedAt(now);
	newProposal->SetUpdatedAt(now);

	if (StoreProposal(newProposalId, *newProposal)){
		return newProposalId;
	}
	return QByteArray();
}


QByteArray CTenantConnectionRequestManagerComp::ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid() || !m_proposalFactoryCompPtr.IsValid()){
		return QByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return QByteArray();
	}

	const ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const ITenantRelationshipProposalInfo*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return QByteArray();
	}

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	// The counterparty approves
	if (proposalPtr->GetCounterpartyTenantId() == tenantId && proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR){
		istd::CChangeNotifier changeNotifier(this);
		ITenantRelationshipProposalInfoUniquePtr updatedProposal = m_proposalFactoryCompPtr.CreateInstance();
		if (!updatedProposal.IsValid()){
			return QByteArray();
		}
		updatedProposal->CopyFrom(*proposalPtr);
		updatedProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_APPLIED);
		updatedProposal->SetUpdatedAt(now);
		StoreProposal(proposalId, *updatedProposal);
		return ApplyRelationshipProposal(updatedProposal.GetPtr());
	}

	// The initiator approves (when counterparty already approved)
	if (proposalPtr->GetInitiatorTenantId() == tenantId && proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_APPROVED_BY_COUNTERPARTY){
		istd::CChangeNotifier changeNotifier(this);
		ITenantRelationshipProposalInfoUniquePtr updatedProposal = m_proposalFactoryCompPtr.CreateInstance();
		if (!updatedProposal.IsValid()){
			return QByteArray();
		}
		updatedProposal->CopyFrom(*proposalPtr);
		updatedProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_APPLIED);
		updatedProposal->SetUpdatedAt(now);
		StoreProposal(proposalId, *updatedProposal);
		return ApplyRelationshipProposal(updatedProposal.GetPtr());
	}

	// If initiator hasn't approved yet but counterparty is approving
	if (proposalPtr->GetCounterpartyTenantId() == tenantId && proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_PENDING){
		istd::CChangeNotifier changeNotifier(this);
		ITenantRelationshipProposalInfoUniquePtr updatedProposal = m_proposalFactoryCompPtr.CreateInstance();
		if (!updatedProposal.IsValid()){
			return QByteArray();
		}
		updatedProposal->CopyFrom(*proposalPtr);
		updatedProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_APPROVED_BY_COUNTERPARTY);
		updatedProposal->SetUpdatedAt(now);
		StoreProposal(proposalId, *updatedProposal);
		return QByteArray();
	}

	return QByteArray();
}


bool CTenantConnectionRequestManagerComp::RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid() || !m_proposalFactoryCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return false;
	}

	const ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const ITenantRelationshipProposalInfo*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return false;
	}

	if (proposalPtr->GetInitiatorTenantId() != tenantId && proposalPtr->GetCounterpartyTenantId() != tenantId){
		return false;
	}
	if (proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_APPLIED
	|| proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_REJECTED
	|| proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_CANCELED){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	ITenantRelationshipProposalInfoUniquePtr updatedProposal = m_proposalFactoryCompPtr.CreateInstance();
	if (!updatedProposal.IsValid()){
		return false;
	}
	updatedProposal->CopyFrom(*proposalPtr);
	updatedProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_REJECTED);
	updatedProposal->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	return StoreProposal(proposalId, *updatedProposal);
}


bool CTenantConnectionRequestManagerComp::CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid() || !m_proposalFactoryCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return false;
	}

	const ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const ITenantRelationshipProposalInfo*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return false;
	}

	if (proposalPtr->GetInitiatorTenantId() != tenantId){
		return false;
	}
	if (proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_APPLIED
	|| proposalPtr->GetStatus() == ITenantRelationshipProposalInfo::RPS_CANCELED){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	ITenantRelationshipProposalInfoUniquePtr updatedProposal = m_proposalFactoryCompPtr.CreateInstance();
	if (!updatedProposal.IsValid()){
		return false;
	}
	updatedProposal->CopyFrom(*proposalPtr);
	updatedProposal->SetStatus(ITenantRelationshipProposalInfo::RPS_CANCELED);
	updatedProposal->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	return StoreProposal(proposalId, *updatedProposal);
}


QByteArrayList CTenantConnectionRequestManagerComp::GetRelationshipProposalIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_proposalCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_proposalCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_proposalCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const ITenantRelationshipProposalInfo*>(dataPtr.GetPtr());
			if (proposalPtr != nullptr){
				if (proposalPtr->GetInitiatorTenantId() == tenantId || proposalPtr->GetCounterpartyTenantId() == tenantId){
					result.append(proposalPtr->GetProposalId());
				}
			}
		}
	}
	return result;
}


// --- Relationships ---

QByteArrayList CTenantConnectionRequestManagerComp::GetTenantRelationshipIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_relationshipCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_relationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr && relPtr->GetStatus() == ITenantRelationshipInfo::TRS_ACTIVE
			&& (relPtr->GetSourceTenantId() == tenantId || relPtr->GetTargetTenantId() == tenantId)){
				result.append(relPtr->GetRelationshipId());
			}
		}
	}
	return result;
}


bool CTenantConnectionRequestManagerComp::RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId)
{
	if (!m_relationshipCollectionCompPtr.IsValid() || !m_relationshipFactoryCompPtr.IsValid()){
		return false;
	}

	// Find the relationship by iterating collection - the relationshipId may be either
	// the collection object key or the internal GetRelationshipId() value
	QByteArray collectionKey;
	const ITenantRelationshipInfo* relPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;

	// First try direct lookup (relationshipId == collection key)
	if (m_relationshipCollectionCompPtr->GetObjectData(relationshipId, dataPtr)){
		relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
		if (relPtr != nullptr){
			collectionKey = relationshipId;
		}
	}

	// If direct lookup failed, iterate to find by GetRelationshipId()
	if (relPtr == nullptr){
		for (const QByteArray& id : m_relationshipCollectionCompPtr->GetElementIds()){
			imtbase::IObjectCollection::DataPtr iterDataPtr;
			if (m_relationshipCollectionCompPtr->GetObjectData(id, iterDataPtr)){
				const ITenantRelationshipInfo* iterRelPtr = dynamic_cast<const ITenantRelationshipInfo*>(iterDataPtr.GetPtr());
				if (iterRelPtr != nullptr && iterRelPtr->GetRelationshipId() == relationshipId){
					relPtr = iterRelPtr;
					collectionKey = id;
					dataPtr = iterDataPtr;
					break;
				}
			}
		}
	}

	if (relPtr == nullptr || collectionKey.isEmpty()){
		return false;
	}
	if (relPtr->GetSourceTenantId() != tenantId && relPtr->GetTargetTenantId() != tenantId){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
	if (!updatedPtr.IsValid()){
		return false;
	}
	updatedPtr->CopyFrom(*relPtr);
	updatedPtr->SetStatus(ITenantRelationshipInfo::TRS_ARCHIVED);
	updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
	return m_relationshipCollectionCompPtr->SetObjectData(collectionKey, *updatedPtr);
}


} // namespace imtauth
