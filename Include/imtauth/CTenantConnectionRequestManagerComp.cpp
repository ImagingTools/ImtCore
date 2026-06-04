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
for (auto it = m_connectionCodes.constBegin(); it != m_connectionCodes.constEnd(); ++it){
if (it.value().connectionCode == connectionCode){
return it.key();
}
}
return QByteArray();
}


bool CTenantConnectionRequestManagerComp::ConnectionExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const
{
for (const TenantConnectionInfo& conn : m_connections){
if (conn.status != CS_ACTIVE){
continue;
}
if ((conn.tenantAId == tenantAId && conn.tenantBId == tenantBId)
|| (conn.tenantAId == tenantBId && conn.tenantBId == tenantAId)){
return true;
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

m_connections.append(conn);
return conn.connectionId;
}


void CTenantConnectionRequestManagerComp::ArchiveRelationshipsForConnection(const QByteArray& connectionId)
{
for (int i = 0; i < m_relationships.size(); ++i){
if (m_relationships[i].connectionId == connectionId){
m_relationships[i].status = RS_ARCHIVED;
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
rel.status = RS_ACTIVE;
rel.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_relationships.append(rel);
return true;
}
else if (proposal.proposalType == RPT_UPDATE){
for (int i = 0; i < m_relationships.size(); ++i){
if (m_relationships[i].relationshipId == proposal.existingRelationshipId){
m_relationships[i].sourceRole = proposal.proposedSourceRole;
m_relationships[i].targetRole = proposal.proposedTargetRole;
m_relationships[i].scope = proposal.proposedScope;
m_relationships[i].description = proposal.proposedDescription;
m_relationships[i].validFrom = proposal.proposedValidFrom;
m_relationships[i].validUntil = proposal.proposedValidUntil;
m_relationships[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}
else if (proposal.proposalType == RPT_DELETE){
for (int i = 0; i < m_relationships.size(); ++i){
if (m_relationships[i].relationshipId == proposal.existingRelationshipId){
m_relationships[i].status = RS_ARCHIVED;
m_relationships[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
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

auto it = m_connectionCodes.find(tenantId);
if (it != m_connectionCodes.end()){
return it.value();
}

// Auto-create connection code for this tenant
TenantConnectionCodeInfo info;
info.tenantId = tenantId;
info.connectionCode = GenerateConnectionCode();
info.allowConnectionsByCode = true;
info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_connectionCodes.insert(tenantId, info);
return info;
}


QString CTenantConnectionRequestManagerComp::RegenerateConnectionCode(const QByteArray& tenantId)
{
if (tenantId.isEmpty()){
return QString();
}

istd::CChangeNotifier changeNotifier(this);

QString newCode = GenerateConnectionCode();
auto it = m_connectionCodes.find(tenantId);
if (it != m_connectionCodes.end()){
it.value().connectionCode = newCode;
} else {
TenantConnectionCodeInfo info;
info.tenantId = tenantId;
info.connectionCode = newCode;
info.allowConnectionsByCode = true;
info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_connectionCodes.insert(tenantId, info);
}
return newCode;
}


bool CTenantConnectionRequestManagerComp::SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow)
{
if (tenantId.isEmpty()){
return false;
}

istd::CChangeNotifier changeNotifier(this);

// Ensure code exists
GetConnectionCode(tenantId);
m_connectionCodes[tenantId].allowConnectionsByCode = allow;
return true;
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
auto it = m_connectionCodes.constFind(targetTenantId);
if (it != m_connectionCodes.constEnd() && !it.value().allowConnectionsByCode){
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

m_requests.append(info);
return info.requestId;
}


QByteArray CTenantConnectionRequestManagerComp::ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId)
{
if (requestId.isEmpty() || approvingTenantId.isEmpty()){
return QByteArray();
}

for (int i = 0; i < m_requests.size(); ++i){
if (m_requests[i].requestId == requestId){
if (m_requests[i].status != CRS_PENDING){
SendErrorMessage(0, "Request is not in pending state", "CTenantConnectionRequestManagerComp");
return QByteArray();
}
if (m_requests[i].targetTenantId != approvingTenantId){
SendErrorMessage(0, "Only the target tenant can approve this request", "CTenantConnectionRequestManagerComp");
return QByteArray();
}

istd::CChangeNotifier changeNotifier(this);

m_requests[i].status = CRS_APPROVED;
m_requests[i].respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

// Create the connection
QByteArray connectionId = CreateConnection(m_requests[i].sourceTenantId, m_requests[i].targetTenantId);
return connectionId;
}
}
return QByteArray();
}


bool CTenantConnectionRequestManagerComp::RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
for (int i = 0; i < m_requests.size(); ++i){
if (m_requests[i].requestId == requestId && m_requests[i].targetTenantId == tenantId){
if (m_requests[i].status != CRS_PENDING){
return false;
}
istd::CChangeNotifier changeNotifier(this);
m_requests[i].status = CRS_REJECTED;
m_requests[i].respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}


bool CTenantConnectionRequestManagerComp::CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
for (int i = 0; i < m_requests.size(); ++i){
if (m_requests[i].requestId == requestId && m_requests[i].sourceTenantId == tenantId){
if (m_requests[i].status != CRS_PENDING){
return false;
}
istd::CChangeNotifier changeNotifier(this);
m_requests[i].status = CRS_CANCELED;
m_requests[i].respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}


ConnectionRequests CTenantConnectionRequestManagerComp::GetConnectionRequests(const QByteArray& tenantId) const
{
ConnectionRequests result;
for (const ConnectionRequestInfo& req : m_requests){
if (req.sourceTenantId == tenantId || req.targetTenantId == tenantId){
result.append(req);
}
}
return result;
}


// --- Connections ---

TenantConnections CTenantConnectionRequestManagerComp::GetConnections(const QByteArray& tenantId) const
{
TenantConnections result;
for (const TenantConnectionInfo& conn : m_connections){
if (conn.status == CS_ACTIVE && (conn.tenantAId == tenantId || conn.tenantBId == tenantId)){
result.append(conn);
}
}
return result;
}


bool CTenantConnectionRequestManagerComp::RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId)
{
for (int i = 0; i < m_connections.size(); ++i){
if (m_connections[i].connectionId == connectionId){
if (m_connections[i].tenantAId != tenantId && m_connections[i].tenantBId != tenantId){
return false;
}
if (m_connections[i].status != CS_ACTIVE){
return false;
}

istd::CChangeNotifier changeNotifier(this);

m_connections[i].status = CS_REMOVED;
m_connections[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

// Cascade: archive all relationships for this connection
ArchiveRelationshipsForConnection(connectionId);
return true;
}
}
return false;
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
for (const TenantConnectionInfo& conn : m_connections){
if (conn.connectionId == proposal.connectionId && conn.status == CS_ACTIVE){
connectionValid = true;
break;
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

m_proposals.append(newProposal);
return newProposal.proposalId;
}


QByteArray CTenantConnectionRequestManagerComp::ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (int i = 0; i < m_proposals.size(); ++i){
if (m_proposals[i].proposalId != proposalId){
continue;
}

RelationshipProposalInfo& p = m_proposals[i];

// The counterparty approves
if (p.counterpartyTenantId == tenantId && p.status == RPS_APPROVED_BY_INITIATOR){
istd::CChangeNotifier changeNotifier(this);
p.status = RPS_APPLIED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
if (ApplyRelationshipProposal(p)){
// Return the relationship ID (last added)
if (!m_relationships.isEmpty()){
return m_relationships.last().relationshipId;
}
}
return QByteArray();
}

// The initiator approves (when counterparty already approved)
if (p.initiatorTenantId == tenantId && p.status == RPS_APPROVED_BY_COUNTERPARTY){
istd::CChangeNotifier changeNotifier(this);
p.status = RPS_APPLIED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
if (ApplyRelationshipProposal(p)){
if (!m_relationships.isEmpty()){
return m_relationships.last().relationshipId;
}
}
return QByteArray();
}

// If initiator hasn't approved yet but counterparty is approving
if (p.counterpartyTenantId == tenantId && p.status == RPS_PENDING){
istd::CChangeNotifier changeNotifier(this);
p.status = RPS_APPROVED_BY_COUNTERPARTY;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return QByteArray();
}

return QByteArray();
}
return QByteArray();
}


bool CTenantConnectionRequestManagerComp::RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (int i = 0; i < m_proposals.size(); ++i){
if (m_proposals[i].proposalId == proposalId){
if (m_proposals[i].initiatorTenantId != tenantId && m_proposals[i].counterpartyTenantId != tenantId){
return false;
}
if (m_proposals[i].status == RPS_APPLIED || m_proposals[i].status == RPS_REJECTED || m_proposals[i].status == RPS_CANCELED){
return false;
}
istd::CChangeNotifier changeNotifier(this);
m_proposals[i].status = RPS_REJECTED;
m_proposals[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}


bool CTenantConnectionRequestManagerComp::CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (int i = 0; i < m_proposals.size(); ++i){
if (m_proposals[i].proposalId == proposalId && m_proposals[i].initiatorTenantId == tenantId){
if (m_proposals[i].status == RPS_APPLIED || m_proposals[i].status == RPS_CANCELED){
return false;
}
istd::CChangeNotifier changeNotifier(this);
m_proposals[i].status = RPS_CANCELED;
m_proposals[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}


RelationshipProposals CTenantConnectionRequestManagerComp::GetRelationshipProposals(const QByteArray& tenantId) const
{
RelationshipProposals result;
for (const RelationshipProposalInfo& p : m_proposals){
if (p.initiatorTenantId == tenantId || p.counterpartyTenantId == tenantId){
result.append(p);
}
}
return result;
}


// --- Relationships ---

ITenantInfo::TenantRelationships CTenantConnectionRequestManagerComp::GetTenantRelationships(const QByteArray& tenantId) const
{
ITenantInfo::TenantRelationships result;
for (const ITenantInfo::TenantRelationship& rel : m_relationships){
if (rel.status == RS_ACTIVE && (rel.sourceTenantId == tenantId || rel.targetTenantId == tenantId)){
result.append(rel);
}
}
return result;
}


bool CTenantConnectionRequestManagerComp::RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId)
{
for (int i = 0; i < m_relationships.size(); ++i){
if (m_relationships[i].relationshipId == relationshipId){
if (m_relationships[i].sourceTenantId != tenantId && m_relationships[i].targetTenantId != tenantId){
return false;
}
istd::CChangeNotifier changeNotifier(this);
m_relationships[i].status = RS_ARCHIVED;
m_relationships[i].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}


} // namespace imtauth
