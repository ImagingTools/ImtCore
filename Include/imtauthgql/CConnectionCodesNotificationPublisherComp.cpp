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

if (m_connectionRequestManagerModelCompPtr.IsValid()){
BaseClass2::AddModel(m_connectionRequestManagerModelCompPtr.GetPtr());
}
}


void CConnectionCodesNotificationPublisherComp::OnComponentDestroyed()
{
BaseClass2::Reset();
BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CConnectionCodesNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
// When the connection request manager model changes, we could detect
// specific changes and publish notifications. For now, this is a no-op
// placeholder since detailed change tracking requires the manager to
// emit specific change flags, which will be implemented when the
// subscription infrastructure is fully wired.
}


// private methods

void CConnectionCodesNotificationPublisherComp::PublishNotification(
const QByteArray& targetUserId,
sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
const QByteArray& tenantId,
const QByteArray& relatedId) const
{
if (targetUserId.isEmpty()){
return;
}

sdl::V1_0::imtauth::CConnectionNotification notification;
notification.notificationType = notificationType;
notification.tenantId = tenantId;
notification.relatedId = relatedId;

QByteArray payload = notification.ToJson();
BaseClass::PublishToUser(targetUserId, payload);
}


QByteArray CConnectionCodesNotificationPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
{
if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
return QByteArray();
}

auto tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
if (!tenantPtr){
return QByteArray();
}

return tenantPtr->GetOwnerUserId();
}


} // namespace imtauthgql
