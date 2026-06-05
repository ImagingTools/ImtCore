// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossTenantMessageBrokerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


static bool IsWithinValidityWindow(const QString& validFrom, const QString& validUntil)
{
	const QDateTime now = QDateTime::currentDateTimeUtc();

	if (!validFrom.isEmpty()){
		QDateTime from = QDateTime::fromString(validFrom, Qt::ISODateWithMs);
		if (from.isValid() && from > now){
			return false;
		}
	}

	if (!validUntil.isEmpty()){
		QDateTime until = QDateTime::fromString(validUntil, Qt::ISODateWithMs);
		if (until.isValid() && until < now){
			return false;
		}
	}

	return true;
}


bool CCrossTenantMessageBrokerComp::ValidateRelationship(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& relationshipId) const
{
	if (!m_tenantManagerCompPtr.IsValid()){
		// No tenant manager wired: relationship validation is skipped so the
		// broker can still operate in reduced/standalone compositions.
		return true;
	}

	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
	if (!tenantPtr.IsValid()){
		return false;
	}

	QByteArrayList relIds = tenantPtr->GetRelationshipIds();
	if (!relIds.contains(relationshipId)){
		return false;
	}

	// Relationship ID is associated with this tenant — allow access
	Q_UNUSED(targetTenantId);
	return true;
}


bool CCrossTenantMessageBrokerComp::StoreMessageInfo(const QByteArray& messageId, const CrossTenantMessageInfo& info)
{
	if (!m_messageCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_messageCollectionCompPtr->GetObjectData(messageId, dataPtr)){
		return false;
	}

	ICrossTenantMessageData* messagePtr = dynamic_cast<ICrossTenantMessageData*>(dataPtr.GetPtr());
	if (messagePtr == nullptr){
		return false;
	}

	messagePtr->SetMessageInfo(info);

	return m_messageCollectionCompPtr->SetObjectData(messageId, *messagePtr);
}


void CCrossTenantMessageBrokerComp::DispatchToHandlers(CrossTenantMessageInfo& info)
{
	for (int i = 0; i < m_handlersCompPtr.GetCount(); ++i){
		ICrossTenantMessageHandler* handlerPtr = m_handlersCompPtr[i];
		if (handlerPtr == nullptr){
			continue;
		}
		if (!handlerPtr->CanHandle(info.messageType, info.customType)){
			continue;
		}

		QByteArray targetObjectId;
		QString errorMessage;
		if (handlerPtr->HandleMessage(info, targetObjectId, errorMessage)){
			info.targetObjectId = targetObjectId;
			info.status = CTMS_PROCESSED;
			info.errorMessage.clear();
		}
		else {
			info.status = CTMS_FAILED;
			info.errorMessage = errorMessage;
		}

		info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		return;
	}
}


QByteArray CCrossTenantMessageBrokerComp::SendMessage(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& relationshipId,
		CrossTenantMessageType messageType,
		const QByteArray& payload,
		const QByteArray& sourceObjectId,
		const QString& customType,
		const QString& expiresAt,
		const QByteArray& contractId)
{
	if (!m_messageCollectionCompPtr.IsValid() || !m_messageFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Message collection or factory not configured", "CCrossTenantMessageBrokerComp");
		return QByteArray();
	}

	if (sourceTenantId.isEmpty() || targetTenantId.isEmpty() || relationshipId.isEmpty()){
		SendErrorMessage(0, "Source tenant, target tenant and relationship are required", "CCrossTenantMessageBrokerComp");
		return QByteArray();
	}

	if (sourceTenantId == targetTenantId){
		SendErrorMessage(0, "Cross-tenant message cannot reference the same tenant as source and target", "CCrossTenantMessageBrokerComp");
		return QByteArray();
	}

	if (!ValidateRelationship(sourceTenantId, targetTenantId, relationshipId)){
		SendErrorMessage(0, "No active relationship authorizes this cross-tenant message", "CCrossTenantMessageBrokerComp");
		return QByteArray();
	}

	QByteArray projectedPayload = payload;
	if (m_projectionCompPtr.IsValid()){
		bool isValid = true;
		projectedPayload = m_projectionCompPtr->ProjectPayload(messageType, customType, payload, isValid);
		if (!isValid){
			SendErrorMessage(0, "Payload rejected by the projection layer", "CCrossTenantMessageBrokerComp");
			return QByteArray();
		}
	}

	ICrossTenantMessageDataUniquePtr messagePtr = m_messageFactoryCompPtr.CreateInstance();
	if (!messagePtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray messageId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	CrossTenantMessageInfo info;
	info.messageId = messageId;
	info.sourceTenantId = sourceTenantId;
	info.targetTenantId = targetTenantId;
	info.relationshipId = relationshipId;
	info.contractId = contractId;
	info.sourceObjectId = sourceObjectId;
	info.messageType = messageType;
	info.customType = customType;
	info.payload = projectedPayload;
	info.status = CTMS_VALIDATED;
	info.createdAt = now;
	info.updatedAt = now;
	info.expiresAt = expiresAt;

	messagePtr->SetMessageInfo(info);

	QByteArray storedId = m_messageCollectionCompPtr->InsertNewObject("CrossTenantMessage", QString(), QString(), messagePtr.GetPtr(), messageId);
	if (storedId.isEmpty()){
		return QByteArray();
	}

	DispatchToHandlers(info);
	StoreMessageInfo(messageId, info);

	return messageId;
}


CrossTenantMessageInfo CCrossTenantMessageBrokerComp::GetMessage(const QByteArray& messageId) const
{
	if (!m_messageCollectionCompPtr.IsValid()){
		return CrossTenantMessageInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_messageCollectionCompPtr->GetObjectData(messageId, dataPtr)){
		return CrossTenantMessageInfo();
	}

	const ICrossTenantMessageData* messagePtr = dynamic_cast<const ICrossTenantMessageData*>(dataPtr.GetPtr());
	if (messagePtr == nullptr){
		return CrossTenantMessageInfo();
	}

	return messagePtr->GetMessageInfo();
}


CrossTenantMessages CCrossTenantMessageBrokerComp::CollectMessages(const std::function<bool(const CrossTenantMessageInfo&)>& predicate) const
{
	CrossTenantMessages result;
	if (!m_messageCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_messageCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_messageCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ICrossTenantMessageData* messagePtr = dynamic_cast<const ICrossTenantMessageData*>(dataPtr.GetPtr());
			if (messagePtr != nullptr){
				CrossTenantMessageInfo info = messagePtr->GetMessageInfo();
				if (predicate(info)){
					result.append(info);
				}
			}
		}
	}

	return result;
}


CrossTenantMessages CCrossTenantMessageBrokerComp::GetIncomingMessages(const QByteArray& targetTenantId) const
{
	return CollectMessages([&targetTenantId](const CrossTenantMessageInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


CrossTenantMessages CCrossTenantMessageBrokerComp::GetOutgoingMessages(const QByteArray& sourceTenantId) const
{
	return CollectMessages([&sourceTenantId](const CrossTenantMessageInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


CrossTenantMessages CCrossTenantMessageBrokerComp::GetMessagesByRelationship(const QByteArray& relationshipId) const
{
	return CollectMessages([&relationshipId](const CrossTenantMessageInfo& info){
		return info.relationshipId == relationshipId;
	});
}


bool CCrossTenantMessageBrokerComp::UpdateMessageStatus(
		const QByteArray& messageId,
		CrossTenantMessageStatus status,
		const QString& errorMessage)
{
	CrossTenantMessageInfo info = GetMessage(messageId);
	if (info.messageId.isEmpty()){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.status = status;
	info.errorMessage = errorMessage;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	return StoreMessageInfo(messageId, info);
}


bool CCrossTenantMessageBrokerComp::SetTargetObjectId(const QByteArray& messageId, const QByteArray& targetObjectId)
{
	CrossTenantMessageInfo info = GetMessage(messageId);
	if (info.messageId.isEmpty()){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.targetObjectId = targetObjectId;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	return StoreMessageInfo(messageId, info);
}


} // namespace imtauth
