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

bool CTenantConnectionRequestManagerComp::StoreConnectionCode(const QByteArray& tenantId, const TenantConnectionCodeInfo& info)
{
	if (!m_connectionCodeCollectionCompPtr.IsValid() || !m_connectionCodeFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionCodeDataUniquePtr dataPtr = m_connectionCodeFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetCodeInfo(info);

	// Try update first, if object exists
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_connectionCodeCollectionCompPtr->GetObjectData(tenantId, existingPtr)){
		return m_connectionCodeCollectionCompPtr->SetObjectData(tenantId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_connectionCodeCollectionCompPtr->InsertNewObject("TenantConnectionCode", QString(), QString(), dataPtr.GetPtr(), tenantId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreConnectionRequest(const QByteArray& requestId, const ConnectionRequestInfo& info)
{
	if (!m_requestCollectionCompPtr.IsValid() || !m_requestFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionRequestDataUniquePtr dataPtr = m_requestFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetRequestInfo(info);

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_requestCollectionCompPtr->GetObjectData(requestId, existingPtr)){
		return m_requestCollectionCompPtr->SetObjectData(requestId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_requestCollectionCompPtr->InsertNewObject("TenantConnectionRequest", QString(), QString(), dataPtr.GetPtr(), requestId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreConnection(const QByteArray& connectionId, const TenantConnectionInfo& info)
{
	if (!m_connectionCollectionCompPtr.IsValid() || !m_connectionFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantConnectionDataUniquePtr dataPtr = m_connectionFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetConnectionInfo(info);

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_connectionCollectionCompPtr->GetObjectData(connectionId, existingPtr)){
		return m_connectionCollectionCompPtr->SetObjectData(connectionId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_connectionCollectionCompPtr->InsertNewObject("TenantConnection", QString(), QString(), dataPtr.GetPtr(), connectionId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreProposal(const QByteArray& proposalId, const RelationshipProposalInfo& info)
{
	if (!m_proposalCollectionCompPtr.IsValid() || !m_proposalFactoryCompPtr.IsValid()){
		return false;
	}

	ITenantRelationshipProposalDataUniquePtr dataPtr = m_proposalFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetProposalInfo(info);

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_proposalCollectionCompPtr->GetObjectData(proposalId, existingPtr)){
		return m_proposalCollectionCompPtr->SetObjectData(proposalId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_proposalCollectionCompPtr->InsertNewObject("TenantRelationshipProposal", QString(), QString(), dataPtr.GetPtr(), proposalId);
	return !storedId.isEmpty();
}


bool CTenantConnectionRequestManagerComp::StoreRelationship(const QByteArray& relationshipId, const ITenantInfo::TenantRelationship& rel)
{
	if (!m_relationshipCollectionCompPtr.IsValid() || !m_relationshipFactoryCompPtr.IsValid()){
		return false;
	}

	istd::TUniqueInterfacePtr<ITenantRelationshipInfo> dataPtr = m_relationshipFactoryCompPtr.CreateInstance();
	if (!dataPtr.IsValid()){
		return false;
	}
	dataPtr->SetRelationshipId(rel.relationshipId);
	dataPtr->SetConnectionId(rel.connectionId);
	dataPtr->SetSourceTenantId(rel.sourceTenantId);
	dataPtr->SetTargetTenantId(rel.targetTenantId);
	dataPtr->SetSourceRole(rel.sourceRole);
	dataPtr->SetTargetRole(rel.targetRole);
	dataPtr->SetScope(rel.scope);
	dataPtr->SetDescription(rel.description);
	dataPtr->SetValidFrom(rel.validFrom);
	dataPtr->SetValidUntil(rel.validUntil);
	dataPtr->SetStatus(rel.status);
	dataPtr->SetCreatedAt(rel.createdAt);
	dataPtr->SetUpdatedAt(rel.updatedAt);

	// Try update first
	imtbase::IObjectCollection::DataPtr existingPtr;
	if (m_relationshipCollectionCompPtr->GetObjectData(relationshipId, existingPtr)){
		return m_relationshipCollectionCompPtr->SetObjectData(relationshipId, *dataPtr);
	}

	// Insert new
	QByteArray storedId = m_relationshipCollectionCompPtr->InsertNewObject("TenantRelationship", QString(), QString(), dataPtr.GetPtr(), relationshipId);
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
			const ITenantConnectionCodeData* codePtr = dynamic_cast<const ITenantConnectionCodeData*>(dataPtr.GetPtr());
			if (codePtr != nullptr && codePtr->GetCodeInfo().connectionCode == connectionCode){
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
			const ITenantConnectionData* connPtr = dynamic_cast<const ITenantConnectionData*>(dataPtr.GetPtr());
			if (connPtr == nullptr){
				continue;
			}
			TenantConnectionInfo info = connPtr->GetConnectionInfo();
			if (info.status != CS_ACTIVE){
				continue;
			}
			if ((info.tenantAId == tenantAId && info.tenantBId == tenantBId)
					|| (info.tenantAId == tenantBId && info.tenantBId == tenantAId)){
				return true;
			}
		}
	}
	return false;
}


QByteArray CTenantConnectionRequestManagerComp::CreateConnection(const QByteArray& tenantAId, const QByteArray& tenantBId)
{
	if (ConnectionExists(tenantAId, tenantBId)){
		return QByteArray();
	}

	TenantConnectionInfo conn;
	conn.connectionId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	// Store in canonical order (smaller ID first)
	if (tenantAId < tenantBId){
		conn.tenantAId = tenantAId;
		conn.tenantBId = tenantBId;
	} else {
		conn.tenantAId = tenantBId;
		conn.tenantBId = tenantAId;
	}
	conn.status = CS_ACTIVE;
	conn.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	conn.updatedAt = conn.createdAt;

	if (StoreConnection(conn.connectionId, conn)){
		return conn.connectionId;
	}
	return QByteArray();
}


void CTenantConnectionRequestManagerComp::ArchiveRelationshipsForConnection(const QByteArray& connectionId)
{
	if (!m_relationshipCollectionCompPtr.IsValid()){
		return;
	}

	for (const QByteArray& id : m_relationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr && relPtr->GetConnectionId() == connectionId && relPtr->GetStatus() == TRS_ACTIVE){
				ITenantInfo::TenantRelationship rel;
				rel.relationshipId = relPtr->GetRelationshipId();
				rel.connectionId = relPtr->GetConnectionId();
				rel.sourceTenantId = relPtr->GetSourceTenantId();
				rel.targetTenantId = relPtr->GetTargetTenantId();
				rel.sourceRole = relPtr->GetSourceRole();
				rel.targetRole = relPtr->GetTargetRole();
				rel.scope = relPtr->GetScope();
				rel.description = relPtr->GetDescription();
				rel.validFrom = relPtr->GetValidFrom();
				rel.validUntil = relPtr->GetValidUntil();
				rel.status = TRS_ARCHIVED;
				rel.createdAt = relPtr->GetCreatedAt();
				rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				StoreRelationship(rel.relationshipId, rel);
			}
		}
	}
}


bool CTenantConnectionRequestManagerComp::ApplyRelationshipProposal(const RelationshipProposalInfo& proposal)
{
	if (proposal.proposalType == RPT_CREATE){
		ITenantInfo::TenantRelationship rel;
		rel.relationshipId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		rel.connectionId = proposal.connectionId;
		rel.sourceTenantId = proposal.initiatorTenantId;
		rel.targetTenantId = proposal.counterpartyTenantId;
		rel.sourceRole = proposal.proposedSourceRole;
		rel.targetRole = proposal.proposedTargetRole;
		rel.scope = proposal.proposedScope;
		rel.description = proposal.proposedDescription;
		rel.validFrom = proposal.proposedValidFrom;
		rel.validUntil = proposal.proposedValidUntil;
		rel.status = TRS_ACTIVE;
		rel.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		return StoreRelationship(rel.relationshipId, rel);
	}
	else if (proposal.proposalType == RPT_UPDATE){
		if (!m_relationshipCollectionCompPtr.IsValid()){
			return false;
		}
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(proposal.existingRelationshipId, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr){
				ITenantInfo::TenantRelationship rel;
				rel.relationshipId = relPtr->GetRelationshipId();
				rel.connectionId = relPtr->GetConnectionId();
				rel.sourceTenantId = relPtr->GetSourceTenantId();
				rel.targetTenantId = relPtr->GetTargetTenantId();
				rel.sourceRole = proposal.proposedSourceRole;
				rel.targetRole = proposal.proposedTargetRole;
				rel.scope = proposal.proposedScope;
				rel.description = proposal.proposedDescription;
				rel.validFrom = proposal.proposedValidFrom;
				rel.validUntil = proposal.proposedValidUntil;
				rel.status = relPtr->GetStatus();
				rel.createdAt = relPtr->GetCreatedAt();
				rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return StoreRelationship(rel.relationshipId, rel);
			}
		}
		return false;
	}
	else if (proposal.proposalType == RPT_DELETE){
		if (!m_relationshipCollectionCompPtr.IsValid()){
			return false;
		}
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(proposal.existingRelationshipId, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr){
				ITenantInfo::TenantRelationship rel;
				rel.relationshipId = relPtr->GetRelationshipId();
				rel.connectionId = relPtr->GetConnectionId();
				rel.sourceTenantId = relPtr->GetSourceTenantId();
				rel.targetTenantId = relPtr->GetTargetTenantId();
				rel.sourceRole = relPtr->GetSourceRole();
				rel.targetRole = relPtr->GetTargetRole();
				rel.scope = relPtr->GetScope();
				rel.description = relPtr->GetDescription();
				rel.validFrom = relPtr->GetValidFrom();
				rel.validUntil = relPtr->GetValidUntil();
				rel.status = TRS_ARCHIVED;
				rel.createdAt = relPtr->GetCreatedAt();
				rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return StoreRelationship(rel.relationshipId, rel);
			}
		}
		return false;
	}
	return false;
}


// --- Connection Code ---

TenantConnectionCodeInfo CTenantConnectionRequestManagerComp::GetConnectionCode(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return TenantConnectionCodeInfo();
	}

	if (m_connectionCodeCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCodeCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
			const ITenantConnectionCodeData* codePtr = dynamic_cast<const ITenantConnectionCodeData*>(dataPtr.GetPtr());
			if (codePtr != nullptr){
				return codePtr->GetCodeInfo();
			}
		}
	}

	// Auto-create connection code for this tenant
	TenantConnectionCodeInfo info;
	info.tenantId = tenantId;
	info.connectionCode = GenerateConnectionCode();
	info.allowConnectionsByCode = true;
	info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	StoreConnectionCode(tenantId, info);
	return info;
}


QString CTenantConnectionRequestManagerComp::RegenerateConnectionCode(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return QString();
	}

	istd::CChangeNotifier changeNotifier(this);

	TenantConnectionCodeInfo info = GetConnectionCode(tenantId);
	info.connectionCode = GenerateConnectionCode();

	StoreConnectionCode(tenantId, info);
	return info.connectionCode;
}


bool CTenantConnectionRequestManagerComp::SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow)
{
	if (tenantId.isEmpty()){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	TenantConnectionCodeInfo info = GetConnectionCode(tenantId);
	info.allowConnectionsByCode = allow;

	return StoreConnectionCode(tenantId, info);
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
	TenantConnectionCodeInfo codeInfo = GetConnectionCode(targetTenantId);
	if (!codeInfo.allowConnectionsByCode){
		SendErrorMessage(0, "Target organization has disabled connections by code", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Check if connection already exists
	if (ConnectionExists(sourceTenantId, targetTenantId)){
		SendErrorMessage(0, "Connection already exists between these organizations", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	ConnectionRequestInfo info;
	info.requestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	info.sourceTenantId = sourceTenantId;
	info.targetTenantId = targetTenantId;
	info.connectionCode = connectionCode;
	info.message = message;
	info.status = CRS_PENDING;
	info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	if (StoreConnectionRequest(info.requestId, info)){
		return info.requestId;
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

	const ITenantConnectionRequestData* reqPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return QByteArray();
	}

	ConnectionRequestInfo info = reqPtr->GetRequestInfo();
	if (info.status != CRS_PENDING){
		SendErrorMessage(0, "Request is not in pending state", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}
	if (info.targetTenantId != approvingTenantId){
		SendErrorMessage(0, "Only the target tenant can approve this request", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = CRS_APPROVED;
	info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	StoreConnectionRequest(requestId, info);

	// Create the connection
	QByteArray connectionId = CreateConnection(info.sourceTenantId, info.targetTenantId);
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

	const ITenantConnectionRequestData* reqPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return false;
	}

	ConnectionRequestInfo info = reqPtr->GetRequestInfo();
	if (info.targetTenantId != tenantId || info.status != CRS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = CRS_REJECTED;
	info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	return StoreConnectionRequest(requestId, info);
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

	const ITenantConnectionRequestData* reqPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
	if (reqPtr == nullptr){
		return false;
	}

	ConnectionRequestInfo info = reqPtr->GetRequestInfo();
	if (info.sourceTenantId != tenantId || info.status != CRS_PENDING){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = CRS_CANCELED;
	info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	return StoreConnectionRequest(requestId, info);
}


ConnectionRequests CTenantConnectionRequestManagerComp::GetConnectionRequests(const QByteArray& tenantId) const
{
	ConnectionRequests result;
	if (!m_requestCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionRequestData* reqPtr = dynamic_cast<const ITenantConnectionRequestData*>(dataPtr.GetPtr());
			if (reqPtr != nullptr){
				ConnectionRequestInfo info = reqPtr->GetRequestInfo();
				if (info.sourceTenantId == tenantId || info.targetTenantId == tenantId){
					result.append(info);
				}
			}
		}
	}
	return result;
}


// --- Connections ---

TenantConnections CTenantConnectionRequestManagerComp::GetConnections(const QByteArray& tenantId) const
{
	TenantConnections result;
	if (!m_connectionCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantConnectionData* connPtr = dynamic_cast<const ITenantConnectionData*>(dataPtr.GetPtr());
			if (connPtr != nullptr){
				TenantConnectionInfo info = connPtr->GetConnectionInfo();
				if (info.status == CS_ACTIVE && (info.tenantAId == tenantId || info.tenantBId == tenantId)){
					result.append(info);
				}
			}
		}
	}
	return result;
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

	const ITenantConnectionData* connPtr = dynamic_cast<const ITenantConnectionData*>(dataPtr.GetPtr());
	if (connPtr == nullptr){
		return false;
	}

	TenantConnectionInfo info = connPtr->GetConnectionInfo();
	if (info.tenantAId != tenantId && info.tenantBId != tenantId){
		return false;
	}
	if (info.status != CS_ACTIVE){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = CS_REMOVED;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	StoreConnection(connectionId, info);

	// Cascade: archive all relationships for this connection
	ArchiveRelationshipsForConnection(connectionId);
	return true;
}


// --- Relationship Proposals ---

QByteArray CTenantConnectionRequestManagerComp::CreateRelationshipProposal(const RelationshipProposalInfo& proposal)
{
	if (proposal.connectionId.isEmpty() || proposal.initiatorTenantId.isEmpty() || proposal.counterpartyTenantId.isEmpty()){
		SendErrorMessage(0, "Connection ID and both tenant IDs are required", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	// Verify connection exists and is active
	bool connectionValid = false;
	if (m_connectionCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_connectionCollectionCompPtr->GetObjectData(proposal.connectionId, dataPtr)){
			const ITenantConnectionData* connPtr = dynamic_cast<const ITenantConnectionData*>(dataPtr.GetPtr());
			if (connPtr != nullptr && connPtr->GetConnectionInfo().status == CS_ACTIVE){
				connectionValid = true;
			}
		}
	}
	if (!connectionValid){
		SendErrorMessage(0, "No active connection found", "CTenantConnectionRequestManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	RelationshipProposalInfo newProposal = proposal;
	newProposal.proposalId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	newProposal.status = RPS_APPROVED_BY_INITIATOR; // Initiator auto-approves
	newProposal.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	newProposal.updatedAt = newProposal.createdAt;

	if (StoreProposal(newProposal.proposalId, newProposal)){
		return newProposal.proposalId;
	}
	return QByteArray();
}


QByteArray CTenantConnectionRequestManagerComp::ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return QByteArray();
	}

	const ITenantRelationshipProposalData* proposalPtr = dynamic_cast<const ITenantRelationshipProposalData*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return QByteArray();
	}

	RelationshipProposalInfo p = proposalPtr->GetProposalInfo();

	// The counterparty approves
	if (p.counterpartyTenantId == tenantId && p.status == RPS_APPROVED_BY_INITIATOR){
		istd::CChangeNotifier changeNotifier(this);
		p.status = RPS_APPLIED;
		p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		StoreProposal(proposalId, p);
		if (ApplyRelationshipProposal(p)){
			// Return the new relationship ID — find it from DB
			if (m_relationshipCollectionCompPtr.IsValid()){
				QByteArrayList ids = m_relationshipCollectionCompPtr->GetElementIds();
				if (!ids.isEmpty()){
					return ids.last();
				}
			}
		}
		return QByteArray();
	}

	// The initiator approves (when counterparty already approved)
	if (p.initiatorTenantId == tenantId && p.status == RPS_APPROVED_BY_COUNTERPARTY){
		istd::CChangeNotifier changeNotifier(this);
		p.status = RPS_APPLIED;
		p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		StoreProposal(proposalId, p);
		if (ApplyRelationshipProposal(p)){
			if (m_relationshipCollectionCompPtr.IsValid()){
				QByteArrayList ids = m_relationshipCollectionCompPtr->GetElementIds();
				if (!ids.isEmpty()){
					return ids.last();
				}
			}
		}
		return QByteArray();
	}

	// If initiator hasn't approved yet but counterparty is approving
	if (p.counterpartyTenantId == tenantId && p.status == RPS_PENDING){
		istd::CChangeNotifier changeNotifier(this);
		p.status = RPS_APPROVED_BY_COUNTERPARTY;
		p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		StoreProposal(proposalId, p);
		return QByteArray();
	}

	return QByteArray();
}


bool CTenantConnectionRequestManagerComp::RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return false;
	}

	const ITenantRelationshipProposalData* proposalPtr = dynamic_cast<const ITenantRelationshipProposalData*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return false;
	}

	RelationshipProposalInfo p = proposalPtr->GetProposalInfo();
	if (p.initiatorTenantId != tenantId && p.counterpartyTenantId != tenantId){
		return false;
	}
	if (p.status == RPS_APPLIED || p.status == RPS_REJECTED || p.status == RPS_CANCELED){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	p.status = RPS_REJECTED;
	p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	return StoreProposal(proposalId, p);
}


bool CTenantConnectionRequestManagerComp::CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
	if (!m_proposalCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_proposalCollectionCompPtr->GetObjectData(proposalId, dataPtr)){
		return false;
	}

	const ITenantRelationshipProposalData* proposalPtr = dynamic_cast<const ITenantRelationshipProposalData*>(dataPtr.GetPtr());
	if (proposalPtr == nullptr){
		return false;
	}

	RelationshipProposalInfo p = proposalPtr->GetProposalInfo();
	if (p.initiatorTenantId != tenantId){
		return false;
	}
	if (p.status == RPS_APPLIED || p.status == RPS_CANCELED){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	p.status = RPS_CANCELED;
	p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	return StoreProposal(proposalId, p);
}


RelationshipProposals CTenantConnectionRequestManagerComp::GetRelationshipProposals(const QByteArray& tenantId) const
{
	RelationshipProposals result;
	if (!m_proposalCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_proposalCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_proposalCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipProposalData* proposalPtr = dynamic_cast<const ITenantRelationshipProposalData*>(dataPtr.GetPtr());
			if (proposalPtr != nullptr){
				RelationshipProposalInfo info = proposalPtr->GetProposalInfo();
				if (info.initiatorTenantId == tenantId || info.counterpartyTenantId == tenantId){
					result.append(info);
				}
			}
		}
	}
	return result;
}


// --- Relationships ---

ITenantInfo::TenantRelationships CTenantConnectionRequestManagerComp::GetTenantRelationships(const QByteArray& tenantId) const
{
	ITenantInfo::TenantRelationships result;
	if (!m_relationshipCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_relationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_relationshipCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
			if (relPtr != nullptr && relPtr->GetStatus() == TRS_ACTIVE
					&& (relPtr->GetSourceTenantId() == tenantId || relPtr->GetTargetTenantId() == tenantId)){
				ITenantInfo::TenantRelationship rel;
				rel.relationshipId = relPtr->GetRelationshipId();
				rel.connectionId = relPtr->GetConnectionId();
				rel.sourceTenantId = relPtr->GetSourceTenantId();
				rel.targetTenantId = relPtr->GetTargetTenantId();
				rel.sourceRole = relPtr->GetSourceRole();
				rel.targetRole = relPtr->GetTargetRole();
				rel.scope = relPtr->GetScope();
				rel.description = relPtr->GetDescription();
				rel.validFrom = relPtr->GetValidFrom();
				rel.validUntil = relPtr->GetValidUntil();
				rel.status = relPtr->GetStatus();
				rel.createdAt = relPtr->GetCreatedAt();
				rel.updatedAt = relPtr->GetUpdatedAt();
				result.append(rel);
			}
		}
	}
	return result;
}


bool CTenantConnectionRequestManagerComp::RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId)
{
	if (!m_relationshipCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_relationshipCollectionCompPtr->GetObjectData(relationshipId, dataPtr)){
		return false;
	}

	const ITenantRelationshipInfo* relPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
	if (relPtr == nullptr){
		return false;
	}
	if (relPtr->GetSourceTenantId() != tenantId && relPtr->GetTargetTenantId() != tenantId){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	ITenantInfo::TenantRelationship rel;
	rel.relationshipId = relPtr->GetRelationshipId();
	rel.connectionId = relPtr->GetConnectionId();
	rel.sourceTenantId = relPtr->GetSourceTenantId();
	rel.targetTenantId = relPtr->GetTargetTenantId();
	rel.sourceRole = relPtr->GetSourceRole();
	rel.targetRole = relPtr->GetTargetRole();
	rel.scope = relPtr->GetScope();
	rel.description = relPtr->GetDescription();
	rel.validFrom = relPtr->GetValidFrom();
	rel.validUntil = relPtr->GetValidUntil();
	rel.status = TRS_ARCHIVED;
	rel.createdAt = relPtr->GetCreatedAt();
	rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
	return StoreRelationship(rel.relationshipId, rel);
}


} // namespace imtauth
