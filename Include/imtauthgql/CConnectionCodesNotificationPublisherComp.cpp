// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectionCodesNotificationPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CConnectionCodesNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
if (m_commandIdsAttrPtr.IsValid()){
QByteArray commandId = m_commandIdsAttrPtr[0];
if (!commandId.isEmpty() && gqlRequest.GetCommandId() == commandId){
return true;
}
}

return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CConnectionCodesNotificationPublisherComp::OnComponentCreated()
{
BaseClass::OnComponentCreated();

// Initialize caches so that subsequent OnModelChanged() calls can
// detect new entries (those not present in the cache).
if (m_connectionRequestManagerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
QMutexLocker locker(&m_cacheMutex);

// Cache connection requests from collection
if (m_requestCollectionCompPtr.IsValid()){
for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
imtbase::IObjectCollection::DataPtr dataPtr;
if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
const imtauth::ITenantConnectionRequestInfo* reqPtr =
dynamic_cast<const imtauth::ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
if (reqPtr != nullptr){
CachedConnectionRequest entry;
entry.sourceTenantId = reqPtr->GetSourceTenantId();
entry.targetTenantId = reqPtr->GetTargetTenantId();
entry.status = reqPtr->GetStatus();
m_cachedRequests.insert(reqPtr->GetRequestId(), entry);
}
}
}
}

// Cache connections from collection
if (m_connectionCollectionCompPtr.IsValid()){
for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
imtbase::IObjectCollection::DataPtr dataPtr;
if (m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
const imtauth::ITenantConnectionInfo* connPtr =
dynamic_cast<const imtauth::ITenantConnectionInfo*>(dataPtr.GetPtr());
if (connPtr != nullptr){
CachedConnection entry;
entry.tenantAId = connPtr->GetTenantAId();
entry.tenantBId = connPtr->GetTenantBId();
entry.status = connPtr->GetStatus();
m_cachedConnections.insert(connPtr->GetConnectionId(), entry);
}
}
}
}

// Cache relationship proposal IDs
QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
for (const QByteArray& tenantId : tenantIds){
QByteArrayList proposalIds =
m_connectionRequestManagerCompPtr->GetRelationshipProposalIds(tenantId);
for (const QByteArray& proposalId : proposalIds){
CachedRelationshipProposal entry;
entry.initiatorTenantId = tenantId;
entry.counterpartyTenantId = QByteArray();
entry.connectionId = QByteArray();
entry.status = imtauth::ITenantRelationshipProposalInfo::RPS_PENDING;
m_cachedProposals.insert(proposalId, entry);
}
}
}

if (m_connectionRequestManagerModelCompPtr.IsValid()){
BaseClass2::RegisterModel(m_connectionRequestManagerModelCompPtr.GetPtr(), 0);
}
}


void CConnectionCodesNotificationPublisherComp::OnComponentDestroyed()
{
BaseClass2::UnregisterAllModels();

BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CConnectionCodesNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
struct PendingNotification
{
QByteArray targetUserId;
sdl::V1_0::imtauth::EConnectionNotificationType notificationType;
QByteArray tenantId;
QByteArray relatedId;
};

QList<PendingNotification> pendingNotifications;

{
QMutexLocker locker(&m_cacheMutex);

if (!m_connectionRequestManagerCompPtr.IsValid() || !m_tenantManagerCompPtr.IsValid()){
return;
}

// --- Handle connection request changes ---
if (m_requestCollectionCompPtr.IsValid()){
QMap<QByteArray, CachedConnectionRequest> currentRequests;

for (const QByteArray& id : m_requestCollectionCompPtr->GetElementIds()){
imtbase::IObjectCollection::DataPtr dataPtr;
if (m_requestCollectionCompPtr->GetObjectData(id, dataPtr)){
const imtauth::ITenantConnectionRequestInfo* reqPtr =
dynamic_cast<const imtauth::ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
if (reqPtr != nullptr){
CachedConnectionRequest entry;
entry.sourceTenantId = reqPtr->GetSourceTenantId();
entry.targetTenantId = reqPtr->GetTargetTenantId();
entry.status = reqPtr->GetStatus();
currentRequests.insert(reqPtr->GetRequestId(), entry);
}
}
}

// Detect new requests (not in cache)
for (auto it = currentRequests.constBegin(); it != currentRequests.constEnd(); ++it){
const QByteArray& requestId = it.key();
const CachedConnectionRequest& current = it.value();

if (!m_cachedRequests.contains(requestId)){
// New request — notify target tenant owner
if (current.status == imtauth::ITenantConnectionRequestInfo::CRS_PENDING){
QByteArray ownerUserId = FindTenantOwnerUserId(current.targetTenantId);
if (!ownerUserId.isEmpty()){
pendingNotifications.append({
ownerUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestReceived,
current.targetTenantId,
requestId});
}
}
}
else{
const CachedConnectionRequest& cached = m_cachedRequests.value(requestId);
if (cached.status != current.status){
if (current.status == imtauth::ITenantConnectionRequestInfo::CRS_APPROVED){
// Notify source tenant owner that request was approved
QByteArray ownerUserId = FindTenantOwnerUserId(current.sourceTenantId);
if (!ownerUserId.isEmpty()){
pendingNotifications.append({
ownerUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestApproved,
current.sourceTenantId,
requestId});
}
}
else if (current.status == imtauth::ITenantConnectionRequestInfo::CRS_REJECTED){
// Notify source tenant owner that request was rejected
QByteArray ownerUserId = FindTenantOwnerUserId(current.sourceTenantId);
if (!ownerUserId.isEmpty()){
pendingNotifications.append({
ownerUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestRejected,
current.sourceTenantId,
requestId});
}
}
}
}
}

// Update request cache
m_cachedRequests = currentRequests;
}

// --- Handle connection changes ---
if (m_connectionCollectionCompPtr.IsValid()){
QMap<QByteArray, CachedConnection> currentConnections;

for (const QByteArray& id : m_connectionCollectionCompPtr->GetElementIds()){
imtbase::IObjectCollection::DataPtr dataPtr;
if (m_connectionCollectionCompPtr->GetObjectData(id, dataPtr)){
const imtauth::ITenantConnectionInfo* connPtr =
dynamic_cast<const imtauth::ITenantConnectionInfo*>(dataPtr.GetPtr());
if (connPtr != nullptr){
CachedConnection entry;
entry.tenantAId = connPtr->GetTenantAId();
entry.tenantBId = connPtr->GetTenantBId();
entry.status = connPtr->GetStatus();
currentConnections.insert(connPtr->GetConnectionId(), entry);
}
}
}

// Detect removed connections (in cache but not current, or status changed to Removed)
for (auto it = m_cachedConnections.constBegin(); it != m_cachedConnections.constEnd(); ++it){
const QByteArray& connectionId = it.key();
const CachedConnection& cached = it.value();

if (!currentConnections.contains(connectionId)
|| currentConnections.value(connectionId).status == imtauth::ITenantConnectionInfo::CS_REMOVED){
if (cached.status == imtauth::ITenantConnectionInfo::CS_ACTIVE){
// Connection removed — notify both tenant owners
QByteArray ownerAUserId = FindTenantOwnerUserId(cached.tenantAId);
if (!ownerAUserId.isEmpty()){
pendingNotifications.append({
ownerAUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRemoved,
cached.tenantAId,
connectionId});
}

QByteArray ownerBUserId = FindTenantOwnerUserId(cached.tenantBId);
if (!ownerBUserId.isEmpty() && ownerBUserId != ownerAUserId){
pendingNotifications.append({
ownerBUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRemoved,
cached.tenantBId,
connectionId});
}
}
}
}

// Update connection cache
m_cachedConnections = currentConnections;
}

// --- Handle relationship proposal changes ---
{
QMap<QByteArray, CachedRelationshipProposal> currentProposals;

QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
for (const QByteArray& tenantId : tenantIds){
QByteArrayList proposalIds =
m_connectionRequestManagerCompPtr->GetRelationshipProposalIds(tenantId);
for (const QByteArray& proposalId : proposalIds){
CachedRelationshipProposal entry;
entry.initiatorTenantId = tenantId;
entry.counterpartyTenantId = QByteArray();
entry.connectionId = QByteArray();
entry.status = imtauth::ITenantRelationshipProposalInfo::RPS_PENDING;
currentProposals.insert(proposalId, entry);
}
}

// Detect new proposals
for (auto it = currentProposals.constBegin(); it != currentProposals.constEnd(); ++it){
const QByteArray& proposalId = it.key();
const CachedRelationshipProposal& current = it.value();

if (!m_cachedProposals.contains(proposalId)){
// New proposal — notify tenant owner
QByteArray ownerUserId = FindTenantOwnerUserId(current.initiatorTenantId);
if (!ownerUserId.isEmpty()){
pendingNotifications.append({
ownerUserId,
sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalReceived,
current.initiatorTenantId,
proposalId});
}
}
}

// Update proposal cache
m_cachedProposals = currentProposals;
}
}

// Publish notifications outside the cache lock
for (const PendingNotification& notification : std::as_const(pendingNotifications)){
PublishNotification(
notification.targetUserId,
notification.notificationType,
notification.tenantId,
notification.relatedId);
}
}


// private methods

void CConnectionCodesNotificationPublisherComp::PublishNotification(
const QByteArray& targetUserId,
sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
const QByteArray& tenantId,
const QByteArray& relatedId) const
{
sdl::V1_0::imtauth::CConnectionNotification notification;
notification.notificationType = notificationType;
notification.tenantId = tenantId;

// Set the appropriate related ID based on notification type
switch (notificationType){
case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestReceived:
case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestApproved:
case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestRejected:
notification.requestId = relatedId;
break;
case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRemoved:
notification.connectionId = relatedId;
break;
case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalReceived:
case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalApproved:
case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalRejected:
notification.proposalId = relatedId;
break;
}

QJsonObject jsonObject;
if (!notification.WriteToJsonObject(jsonObject)){
Q_ASSERT(false);
return;
}

QJsonDocument jsonDoc;
jsonDoc.setObject(jsonObject);
QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

const QByteArray commandId = m_commandIdsAttrPtr.IsValid() && m_commandIdsAttrPtr.GetCount() > 0
? m_commandIdsAttrPtr[0]
: QByteArray("OnConnectionNotification");

// Filter: only push to subscribers whose context userId matches targetUserId
PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
if (contextPtr != nullptr){
const QByteArray contextUserId = contextPtr->GetUserId();
return contextUserId == targetUserId;
}
return false;
});
}


QByteArray CConnectionCodesNotificationPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
{
if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
return QByteArray();
}

imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
if (tenantPtr.IsValid()){
return tenantPtr->GetOwnerId();
}

return QByteArray();
}


} // namespace imtauthgql
