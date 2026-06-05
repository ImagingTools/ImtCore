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
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


/**
Self-contained mock replicating the lifecycle semantics of
CTenantConnectionRequestManagerComp for the new corporate connection model.

Covers: Connection Codes, Connection Requests, Connections,
Relationships, and Relationship Proposals.
*/
class CMockConnectionManager
{
public:
typedef ITenantConnectionRequest::TenantConnectionCodeInfo TenantConnectionCodeInfo;
typedef ITenantConnectionRequest::ConnectionRequestInfo ConnectionRequestInfo;
typedef ITenantConnectionRequest::ConnectionRequests ConnectionRequests;
typedef ITenantConnectionRequest::TenantConnectionInfo TenantConnectionInfo;
typedef ITenantConnectionRequest::TenantConnections TenantConnections;
typedef ITenantRelationshipProposalInfo::RelationshipProposalInfo RelationshipProposalInfo;
typedef ITenantRelationshipProposalInfo::RelationshipProposalStatus RelationshipProposalStatus;
typedef ITenantRelationshipProposalInfo::RelationshipProposalType RelationshipProposalType;

// --- Connection Code ---

TenantConnectionCodeInfo GetConnectionCode(const QByteArray& tenantId)
{
for (const TenantConnectionCodeInfo& code : m_codes){
if (code.tenantId == tenantId){
return code;
}
}
TenantConnectionCodeInfo newCode;
newCode.tenantId = tenantId;
newCode.connectionCode = GenerateCode();
newCode.allowConnectionsByCode = true;
newCode.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_codes.append(newCode);
return newCode;
}

QString RegenerateConnectionCode(const QByteArray& tenantId)
{
for (TenantConnectionCodeInfo& code : m_codes){
if (code.tenantId == tenantId){
code.connectionCode = GenerateCode();
return code.connectionCode;
}
}
TenantConnectionCodeInfo newCode;
newCode.tenantId = tenantId;
newCode.connectionCode = GenerateCode();
newCode.allowConnectionsByCode = true;
newCode.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_codes.append(newCode);
return newCode.connectionCode;
}

bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow)
{
for (TenantConnectionCodeInfo& code : m_codes){
if (code.tenantId == tenantId){
code.allowConnectionsByCode = allow;
return true;
}
}
return false;
}

// --- Connection Requests ---

QByteArray CreateConnectionRequest(
const QByteArray& sourceTenantId,
const QString& connectionCode,
const QString& message = QString())
{
if (sourceTenantId.isEmpty() || connectionCode.isEmpty()){
return QByteArray();
}

QByteArray targetTenantId;
for (const TenantConnectionCodeInfo& code : m_codes){
if (code.connectionCode == connectionCode){
if (!code.allowConnectionsByCode){
return QByteArray();
}
targetTenantId = code.tenantId;
break;
}
}
if (targetTenantId.isEmpty()){
return QByteArray();
}

if (sourceTenantId == targetTenantId){
return QByteArray();
}

for (const TenantConnectionInfo& conn : m_connections){
if (conn.status == ITenantConnectionRequest::CS_ACTIVE){
if ((conn.tenantAId == sourceTenantId && conn.tenantBId == targetTenantId)
|| (conn.tenantAId == targetTenantId && conn.tenantBId == sourceTenantId)){
return QByteArray();
}
}
}

ConnectionRequestInfo info;
info.requestId = QByteArray::number(++m_counter);
info.sourceTenantId = sourceTenantId;
info.targetTenantId = targetTenantId;
info.connectionCode = connectionCode;
info.message = message;
info.status = ITenantConnectionRequest::CRS_PENDING;
info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

m_requests.append(info);
return info.requestId;
}

QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId)
{
for (ConnectionRequestInfo& info : m_requests){
if (info.requestId != requestId){
continue;
}
if (info.status != ITenantConnectionRequest::CRS_PENDING){
return QByteArray();
}
if (info.targetTenantId != approvingTenantId){
return QByteArray();
}

info.status = ITenantConnectionRequest::CRS_APPROVED;
info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

TenantConnectionInfo conn;
conn.connectionId = QByteArray::number(++m_counter);
if (info.sourceTenantId < info.targetTenantId){
conn.tenantAId = info.sourceTenantId;
conn.tenantBId = info.targetTenantId;
} else {
conn.tenantAId = info.targetTenantId;
conn.tenantBId = info.sourceTenantId;
}
conn.status = ITenantConnectionRequest::CS_ACTIVE;
conn.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_connections.append(conn);
return conn.connectionId;
}
return QByteArray();
}

bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
for (ConnectionRequestInfo& info : m_requests){
if (info.requestId == requestId && info.targetTenantId == tenantId
&& info.status == ITenantConnectionRequest::CRS_PENDING){
info.status = ITenantConnectionRequest::CRS_REJECTED;
info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}

bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
{
for (ConnectionRequestInfo& info : m_requests){
if (info.requestId == requestId && info.sourceTenantId == tenantId
&& info.status == ITenantConnectionRequest::CRS_PENDING){
info.status = ITenantConnectionRequest::CRS_CANCELED;
info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}

ConnectionRequests GetConnectionRequests(const QByteArray& tenantId) const
{
ConnectionRequests result;
for (const ConnectionRequestInfo& info : m_requests){
if (info.sourceTenantId == tenantId || info.targetTenantId == tenantId){
result.append(info);
}
}
return result;
}

// --- Connections ---

TenantConnections GetConnections(const QByteArray& tenantId) const
{
TenantConnections result;
for (const TenantConnectionInfo& conn : m_connections){
if (conn.status == ITenantConnectionRequest::CS_ACTIVE
&& (conn.tenantAId == tenantId || conn.tenantBId == tenantId)){
result.append(conn);
}
}
return result;
}

bool RemoveConnection(const QByteArray& connectionId, const QByteArray& /*tenantId*/)
{
for (TenantConnectionInfo& conn : m_connections){
if (conn.connectionId == connectionId && conn.status == ITenantConnectionRequest::CS_ACTIVE){
conn.status = ITenantConnectionRequest::CS_REMOVED;
conn.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
// Archive related relationships
for (int i = 0; i < m_relationshipIds.size(); ++i){
if (m_relationshipConnectionIds.value(m_relationshipIds[i]) == connectionId){
m_archivedRelationshipIds.append(m_relationshipIds[i]);
}
}
return true;
}
}
return false;
}

// --- Relationship Proposals ---

QByteArray CreateRelationshipProposal(
const QByteArray& /*proposalId*/,
const QByteArray& connectionId,
const QByteArray& initiatorTenantId,
const QByteArray& counterpartyTenantId)
{
if (connectionId.isEmpty() || initiatorTenantId.isEmpty()){
return QByteArray();
}
bool found = false;
for (const TenantConnectionInfo& conn : m_connections){
if (conn.connectionId == connectionId && conn.status == ITenantConnectionRequest::CS_ACTIVE){
found = true;
break;
}
}
if (!found){
return QByteArray();
}

RelationshipProposalInfo stored;
stored.proposalId = QByteArray::number(++m_counter);
stored.connectionId = connectionId;
stored.initiatorTenantId = initiatorTenantId;
stored.counterpartyTenantId = counterpartyTenantId;
stored.proposalType = ITenantRelationshipProposalInfo::RPT_CREATE;
stored.status = ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR;
stored.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
m_proposals.append(stored);
return stored.proposalId;
}

QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (RelationshipProposalInfo& p : m_proposals){
if (p.proposalId != proposalId){
continue;
}
if (p.counterpartyTenantId != tenantId){
return QByteArray();
}
if (p.status != ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR){
return QByteArray();
}
p.status = ITenantRelationshipProposalInfo::RPS_APPLIED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

// Create relationship ID
QByteArray relId = QByteArray::number(++m_counter);
m_relationshipIds.append(relId);
m_relationshipConnectionIds.insert(relId, p.connectionId);
m_relationshipTenantIds.insert(relId, qMakePair(p.initiatorTenantId, p.counterpartyTenantId));
return relId;
}
return QByteArray();
}

bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (RelationshipProposalInfo& p : m_proposals){
if (p.proposalId == proposalId && p.counterpartyTenantId == tenantId
&& p.status == ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR){
p.status = ITenantRelationshipProposalInfo::RPS_REJECTED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}

bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
{
for (RelationshipProposalInfo& p : m_proposals){
if (p.proposalId == proposalId && p.initiatorTenantId == tenantId
&& (p.status == ITenantRelationshipProposalInfo::RPS_PENDING
|| p.status == ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR)){
p.status = ITenantRelationshipProposalInfo::RPS_CANCELED;
p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
return true;
}
}
return false;
}

QByteArrayList GetRelationshipProposalIds(const QByteArray& tenantId) const
{
QByteArrayList result;
for (const RelationshipProposalInfo& p : m_proposals){
if (p.initiatorTenantId == tenantId || p.counterpartyTenantId == tenantId){
result.append(p.proposalId);
}
}
return result;
}

// --- Relationships ---

QByteArrayList GetTenantRelationshipIds(const QByteArray& tenantId) const
{
QByteArrayList result;
for (const QByteArray& relId : m_relationshipIds){
if (m_archivedRelationshipIds.contains(relId)){
continue;
}
auto pair = m_relationshipTenantIds.value(relId);
if (pair.first == tenantId || pair.second == tenantId){
result.append(relId);
}
}
return result;
}

bool RemoveTenantRelationship(const QByteArray& /*tenantId*/, const QByteArray& relationshipId)
{
if (m_relationshipIds.contains(relationshipId) && !m_archivedRelationshipIds.contains(relationshipId)){
m_archivedRelationshipIds.append(relationshipId);
return true;
}
return false;
}

// --- Internal data ---
QList<TenantConnectionCodeInfo> m_codes;
QList<ConnectionRequestInfo> m_requests;
QList<TenantConnectionInfo> m_connections;
QList<RelationshipProposalInfo> m_proposals;
QByteArrayList m_relationshipIds;
QByteArrayList m_archivedRelationshipIds;
QMap<QByteArray, QByteArray> m_relationshipConnectionIds;
QMap<QByteArray, QPair<QByteArray, QByteArray>> m_relationshipTenantIds;
int m_counter = 0;

private:
QString GenerateCode() const
{
return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8).toUpper();
}
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

// Connection Code
void testGetConnectionCode_AutoCreates();
void testRegenerateConnectionCode_ChangesCode();
void testSetAllowConnectionsByCode();

// Connection Requests
void testCreateConnectionRequest_Success();
void testCreateConnectionRequest_InvalidCode_Fails();
void testCreateConnectionRequest_SelfConnect_Fails();
void testCreateConnectionRequest_DisabledByCode_Fails();
void testCreateConnectionRequest_DuplicateConnection_Fails();
void testApproveConnectionRequest_CreatesConnection();
void testRejectConnectionRequest_Success();
void testCancelConnectionRequest_Success();
void testApproveConnectionRequest_NonPending_Fails();
void testApproveConnectionRequest_WrongTenant_Fails();

// Connections
void testGetConnections_ReturnsActive();
void testRemoveConnection_CascadesRelationships();

// Relationship Proposals
void testCreateRelationshipProposal_Success();
void testCreateRelationshipProposal_NoConnection_Fails();
void testApproveRelationshipProposal_CreatesRelationship();
void testRejectRelationshipProposal_Success();
void testCancelRelationshipProposal_Success();
void testApproveRelationshipProposal_WrongTenant_Fails();

// Relationships
void testGetTenantRelationships_ReturnsActive();
void testRemoveTenantRelationship_Archives();

private:
imtauth::CMockConnectionManager* m_managerPtr = nullptr;
};
