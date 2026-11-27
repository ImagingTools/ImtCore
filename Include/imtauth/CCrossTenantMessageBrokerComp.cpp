// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossTenantMessageBrokerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ContractState.h>
#include <imtauth/TenantEvents.h>
#include <imtauth/CollectionSelection.h>


namespace imtauth
{


bool CCrossTenantMessageBrokerComp::ValidateLink(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& contractId,
		const QByteArray& linkId) const
{
	IContractInfoUniquePtr contractPtr = m_contractProviderCompPtr->GetContract(contractId);
	if (!contractPtr.IsValid()){
		return false;
	}

	const IContractInfo::Links links = contractPtr->GetLinks();
	const int linkIndex = FindContractLink(links, linkId);
	if (linkIndex < 0){
		return false;
	}

	const IContractInfo::Link& link = links[linkIndex];

	return link.Connects(sourceTenantId, targetTenantId) && link.isMessagingAllowed && IsLinkInForce(*contractPtr, link, QDateTime::currentDateTimeUtc());
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
		const QByteArray& contractId,
		const QByteArray& linkId,
		CrossTenantMessageType messageType,
		const QByteArray& payload,
		const QByteArray& sourceObjectId,
		const QString& customType,
		const QString& expiresAt,
		QString* errorMessagePtr)
{
	auto refuse = [this, errorMessagePtr](const QString& reason){
		SendErrorMessage(0, reason, "CCrossTenantMessageBrokerComp");
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = reason;
		}

		return QByteArray();
	};

	if (!m_messageCollectionCompPtr.IsValid() || !m_messageFactoryCompPtr.IsValid()){
		return refuse(QStringLiteral("Message collection or factory not configured"));
	}

	if (sourceTenantId.isEmpty() || targetTenantId.isEmpty() || contractId.isEmpty() || linkId.isEmpty()){
		return refuse(QStringLiteral("Source tenant, target tenant, contract and link are required"));
	}

	if (sourceTenantId == targetTenantId){
		return refuse(QStringLiteral("Cross-tenant message cannot reference the same tenant as source and target"));
	}

	if (!ValidateLink(sourceTenantId, targetTenantId, contractId, linkId)){
		return refuse(QStringLiteral("No link of a contract in force allows this message"));
	}

	QByteArray projectedPayload = payload;
	if (m_projectionCompPtr.IsValid()){
		bool isValid = true;
		projectedPayload = m_projectionCompPtr->ProjectPayload(messageType, customType, payload, isValid);
		if (!isValid){
			return refuse(QStringLiteral("Payload rejected by the projection layer"));
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
	info.contractId = contractId;
	info.linkId = linkId;
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

	TenantEvent::Publish(this, {
				{TenantEvent::s_type, TenantEvent::s_messageCreated},
				{TenantEvent::s_messageId, messageId}});

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


CrossTenantMessages CCrossTenantMessageBrokerComp::CollectMessages(const QByteArray& columnId, const QByteArray& value, const std::function<bool(const CrossTenantMessageInfo&)>& predicate) const
{
	CrossTenantMessages result;
	if (!m_messageCollectionCompPtr.IsValid()){
		return result;
	}

	// A collection that does not know the filter returns every id, the predicate still decides
	for (const QByteArray& id : CollectionSelection::SelectIds(*m_messageCollectionCompPtr, CollectionSelection::ColumnEquals(columnId, value))){
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
	return CollectMessages("TargetTenantId", targetTenantId, [&targetTenantId](const CrossTenantMessageInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


CrossTenantMessages CCrossTenantMessageBrokerComp::GetOutgoingMessages(const QByteArray& sourceTenantId) const
{
	return CollectMessages("SourceTenantId", sourceTenantId, [&sourceTenantId](const CrossTenantMessageInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


CrossTenantMessages CCrossTenantMessageBrokerComp::GetMessagesByContract(const QByteArray& contractId) const
{
	return CollectMessages("ContractId", contractId, [&contractId](const CrossTenantMessageInfo& info){
		return info.contractId == contractId;
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

	const CrossTenantMessageStatus previousStatus = info.status;
	info.status = status;
	info.errorMessage = errorMessage;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	if (!StoreMessageInfo(messageId, info)){
		return false;
	}

	if (previousStatus != status){
		TenantEvent::Publish(this, {
					{TenantEvent::s_type, TenantEvent::s_messageStatusChanged},
					{TenantEvent::s_messageId, messageId}});
	}

	return true;
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
