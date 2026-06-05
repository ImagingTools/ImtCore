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


bool CTenantConnectionRequestManagerComp::ApplyRelationshipProposal(const RelationshipProposalInfo& proposal)
{
if (!m_relationshipCollectionCompPtr.IsValid() || !m_relationshipFactoryCompPtr.IsValid()){
return false;
}

if (proposal.proposalType == ITenantRelationshipProposalData::RPT_CREATE){
istd::TUniqueInterfacePtr<ITenantRelationshipInfo> relPtr = m_relationshipFactoryCompPtr.CreateInstance();
if (!relPtr.IsValid()){
return false;
}
QByteArray relId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
relPtr->SetRelationshipId(relId);
relPtr->SetConnectionId(proposal.connectionId);
relPtr->SetSourceTenantId(proposal.initiatorTenantId);
relPtr->SetTargetTenantId(proposal.counterpartyTenantId);
relPtr->SetSourceRole(proposal.proposedSourceRole);
relPtr->SetTargetRole(proposal.proposedTargetRole);
relPtr->SetScope(proposal.proposedScope);
relPtr->SetDescription(proposal.proposedDescription);
relPtr->SetValidFrom(proposal.proposedValidFrom);
relPtr->SetValidUntil(proposal.proposedValidUntil);
relPtr->SetStatus(ITenantRelationshipInfo::TRS_ACTIVE);
relPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

QByteArray storedId = m_relationshipCollectionCompPtr->InsertNewObject("TenantRelationship", QString(), QString(), relPtr.GetPtr(), relId);
return !storedId.isEmpty();
}
else if (proposal.proposalType == ITenantRelationshipProposalData::RPT_UPDATE){
imtbase::IObjectCollection::DataPtr dataPtr;
if (!m_relationshipCollectionCompPtr->GetObjectData(proposal.existingRelationshipId, dataPtr)){
return false;
}
const ITenantRelationshipInfo* existingPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
if (existingPtr == nullptr){
return false;
}
istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
if (!updatedPtr.IsValid()){
return false;
}
updatedPtr->CopyFrom(*existingPtr);
updatedPtr->SetSourceRole(proposal.proposedSourceRole);
updatedPtr->SetTargetRole(proposal.proposedTargetRole);
updatedPtr->SetScope(proposal.proposedScope);
updatedPtr->SetDescription(proposal.proposedDescription);
updatedPtr->SetValidFrom(proposal.proposedValidFrom);
updatedPtr->SetValidUntil(proposal.proposedValidUntil);
updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
return m_relationshipCollectionCompPtr->SetObjectData(proposal.existingRelationshipId, *updatedPtr);
}
else if (proposal.proposalType == ITenantRelationshipProposalData::RPT_DELETE){
imtbase::IObjectCollection::DataPtr dataPtr;
if (!m_relationshipCollectionCompPtr->GetObjectData(proposal.existingRelationshipId, dataPtr)){
return false;
}
const ITenantRelationshipInfo* existingPtr = dynamic_cast<const ITenantRelationshipInfo*>(dataPtr.GetPtr());
if (existingPtr == nullptr){
return false;
}
istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
if (!updatedPtr.IsValid()){
return false;
}
updatedPtr->CopyFrom(*existingPtr);
updatedPtr->SetStatus(ITenantRelationshipInfo::TRS_ARCHIVED);
updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
return m_relationshipCollectionCompPtr->SetObjectData(proposal.existingRelationshipId, *updatedPtr);
}
return false;
}


// --- Connection Code ---

ITenantConnectionRequest::TenantConnectionCodeInfo CTenantConnectionRequestManagerComp::GetConnectionCode(const QByteArray& tenantId)
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


ITenantConnectionRequest::ConnectionRequests CTenantConnectionRequestManagerComp::GetConnectionRequests(const QByteArray& tenantId) const
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

ITenantConnectionRequest::TenantConnections CTenantConnectionRequestManagerComp::GetConnections(const QByteArray& tenantId) const
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

QByteArray CTenantConnectionRequestManagerComp::CreateRelationshipProposal(
const QByteArray& /*proposalId*/,
const QByteArray& connectionId,
const QByteArray& initiatorTenantId,
const QByteArray& counterpartyTenantId)
{
if (connectionId.isEmpty() || initiatorTenantId.isEmpty() || counterpartyTenantId.isEmpty()){
SendErrorMessage(0, "Connection ID and both tenant IDs are required", "CTenantConnectionRequestManagerComp");
return QByteArray();
}

// Verify connection exists and is active
bool connectionValid = false;
if (m_connectionCollectionCompPtr.IsValid()){
imtbase::IObjectCollection::DataPtr dataPtr;
if (m_connectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
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

RelationshipProposalInfo newProposal;
newProposal.proposalId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
newProposal.connectionId = connectionId;
newProposal.initiatorTenantId = initiatorTenantId;
newProposal.counterpartyTenantId = counterpartyTenantId;
newProposal.proposalType = ITenantRelationshipProposalData::RPT_CREATE;
newProposal.status = ITenantRelationshipProposalData::RPS_APPROVED_BY_INITIATOR; // Initiator auto-approves
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
if (p.counterpartyTenantId == tenantId && p.status == ITenantRelationshipProposalData::RPS_APPROVED_BY_INITIATOR){
istd::CChangeNotifier changeNotifier(this);
p.status = ITenantRelationshipProposalData::RPS_APPLIED;
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
if (p.initiatorTenantId == tenantId && p.status == ITenantRelationshipProposalData::RPS_APPROVED_BY_COUNTERPARTY){
istd::CChangeNotifier changeNotifier(this);
p.status = ITenantRelationshipProposalData::RPS_APPLIED;
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
if (p.counterpartyTenantId == tenantId && p.status == ITenantRelationshipProposalData::RPS_PENDING){
istd::CChangeNotifier changeNotifier(this);
p.status = ITenantRelationshipProposalData::RPS_APPROVED_BY_COUNTERPARTY;
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
if (p.status == ITenantRelationshipProposalData::RPS_APPLIED || p.status == ITenantRelationshipProposalData::RPS_REJECTED || p.status == ITenantRelationshipProposalData::RPS_CANCELED){
return false;
}

istd::CChangeNotifier changeNotifier(this);
p.status = ITenantRelationshipProposalData::RPS_REJECTED;
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
if (p.status == ITenantRelationshipProposalData::RPS_APPLIED || p.status == ITenantRelationshipProposalData::RPS_CANCELED){
return false;
}

istd::CChangeNotifier changeNotifier(this);
p.status = ITenantRelationshipProposalData::RPS_CANCELED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return StoreProposal(proposalId, p);
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
const ITenantRelationshipProposalData* proposalPtr = dynamic_cast<const ITenantRelationshipProposalData*>(dataPtr.GetPtr());
if (proposalPtr != nullptr){
RelationshipProposalInfo info = proposalPtr->GetProposalInfo();
if (info.initiatorTenantId == tenantId || info.counterpartyTenantId == tenantId){
result.append(info.proposalId);
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

istd::TUniqueInterfacePtr<ITenantRelationshipInfo> updatedPtr = m_relationshipFactoryCompPtr.CreateInstance();
if (!updatedPtr.IsValid()){
return false;
}
updatedPtr->CopyFrom(*relPtr);
updatedPtr->SetStatus(ITenantRelationshipInfo::TRS_ARCHIVED);
updatedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
return m_relationshipCollectionCompPtr->SetObjectData(relationshipId, *updatedPtr);
}


} // namespace imtauth
