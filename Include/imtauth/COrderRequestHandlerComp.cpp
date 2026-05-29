// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COrderRequestHandlerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


bool COrderRequestHandlerComp::CanHandle(CrossTenantMessageType messageType, const QString& /*customType*/) const
{
	return messageType == CTMT_ORDER_REQUEST;
}


bool COrderRequestHandlerComp::HandleMessage(
		const CrossTenantMessageInfo& messageInfo,
		QByteArray& targetObjectId,
		QString& errorMessage)
{
	if (!m_orderRequestCollectionCompPtr.IsValid() || !m_orderRequestFactoryCompPtr.IsValid()){
		errorMessage = "Order request collection or factory not configured";
		return false;
	}

	IOrderRequestDataUniquePtr orderRequestPtr = m_orderRequestFactoryCompPtr.CreateInstance();
	if (!orderRequestPtr.IsValid()){
		errorMessage = "Failed to create order request instance";
		return false;
	}

	QJsonObject payloadObject = QJsonDocument::fromJson(messageInfo.payload).object();

	istd::CChangeNotifier changeNotifier(this);

	QByteArray orderRequestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	OrderRequestInfo info;
	info.orderRequestId = orderRequestId;
	info.messageId = messageInfo.messageId;
	info.sourceTenantId = messageInfo.sourceTenantId;
	info.targetTenantId = messageInfo.targetTenantId;
	info.relationshipId = messageInfo.relationshipId;
	info.sourceOrderId = messageInfo.sourceObjectId;
	info.articleNumber = payloadObject.value("articleNumber").toString();
	info.quantity = payloadObject.value("quantity").toInt();
	info.note = payloadObject.value("note").toString();
	info.status = ORS_RECEIVED;
	info.createdAt = now;
	info.updatedAt = now;

	orderRequestPtr->SetOrderRequestInfo(info);

	QByteArray storedId = m_orderRequestCollectionCompPtr->InsertNewObject("OrderRequest", QString(), QString(), orderRequestPtr.GetPtr(), orderRequestId);
	if (storedId.isEmpty()){
		errorMessage = "Failed to store order request";
		return false;
	}

	targetObjectId = orderRequestId;

	return true;
}


OrderRequestInfo COrderRequestHandlerComp::GetOrderRequest(const QByteArray& orderRequestId) const
{
	if (!m_orderRequestCollectionCompPtr.IsValid()){
		return OrderRequestInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_orderRequestCollectionCompPtr->GetObjectData(orderRequestId, dataPtr)){
		return OrderRequestInfo();
	}

	const IOrderRequestData* orderRequestPtr = dynamic_cast<const IOrderRequestData*>(dataPtr.GetPtr());
	if (orderRequestPtr == nullptr){
		return OrderRequestInfo();
	}

	return orderRequestPtr->GetOrderRequestInfo();
}


OrderRequests COrderRequestHandlerComp::CollectOrderRequests(const std::function<bool(const OrderRequestInfo&)>& predicate) const
{
	OrderRequests result;
	if (!m_orderRequestCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_orderRequestCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_orderRequestCollectionCompPtr->GetObjectData(id, dataPtr)){
			continue;
		}

		const IOrderRequestData* orderRequestPtr = dynamic_cast<const IOrderRequestData*>(dataPtr.GetPtr());
		if (orderRequestPtr == nullptr){
			continue;
		}

		OrderRequestInfo info = orderRequestPtr->GetOrderRequestInfo();
		if (predicate(info)){
			result.append(info);
		}
	}

	return result;
}


OrderRequests COrderRequestHandlerComp::GetOrderRequests(const QByteArray& targetTenantId) const
{
	return CollectOrderRequests([&targetTenantId](const OrderRequestInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


OrderRequests COrderRequestHandlerComp::GetOrderRequestsByRelationship(const QByteArray& relationshipId) const
{
	return CollectOrderRequests([&relationshipId](const OrderRequestInfo& info){
		return info.relationshipId == relationshipId;
	});
}


bool COrderRequestHandlerComp::ApplyStatusFeedback(
		const QByteArray& orderRequestId,
		OrderRequestStatus status,
		CrossTenantMessageType feedbackType,
		const QString& note)
{
	if (!m_orderRequestCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_orderRequestCollectionCompPtr->GetObjectData(orderRequestId, dataPtr)){
		return false;
	}

	IOrderRequestData* orderRequestPtr = dynamic_cast<IOrderRequestData*>(dataPtr.GetPtr());
	if (orderRequestPtr == nullptr){
		return false;
	}

	OrderRequestInfo info = orderRequestPtr->GetOrderRequestInfo();

	istd::CChangeNotifier changeNotifier(this);

	info.status = status;
	info.statusNote = note;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	orderRequestPtr->SetOrderRequestInfo(info);

	if (!m_orderRequestCollectionCompPtr->SetObjectData(orderRequestId, *orderRequestPtr)){
		return false;
	}

	if (m_brokerCompPtr.IsValid() && !info.sourceTenantId.isEmpty() && !info.relationshipId.isEmpty()){
		QJsonObject feedbackObject;
		feedbackObject.insert("orderRequestId", QString::fromUtf8(info.orderRequestId));
		feedbackObject.insert("sourceOrderId", QString::fromUtf8(info.sourceOrderId));
		feedbackObject.insert("status", static_cast<int>(status));
		feedbackObject.insert("note", note);

		const QByteArray feedbackPayload = QJsonDocument(feedbackObject).toJson(QJsonDocument::Compact);

		// Feedback travels in the reverse direction: this tenant (the original
		// target) becomes the source and the original source tenant becomes the
		// target. The order request ID is carried as the source object ID so the
		// source tenant can correlate the feedback to its own order.
		m_brokerCompPtr->SendMessage(
					info.targetTenantId,
					info.sourceTenantId,
					info.relationshipId,
					feedbackType,
					feedbackPayload,
					info.orderRequestId);
	}

	return true;
}


bool COrderRequestHandlerComp::ConfirmOrderRequest(const QByteArray& orderRequestId, const QString& note)
{
	return ApplyStatusFeedback(orderRequestId, ORS_CONFIRMED, CTMT_ORDER_CONFIRMATION, note);
}


bool COrderRequestHandlerComp::RejectOrderRequest(const QByteArray& orderRequestId, const QString& reason)
{
	return ApplyStatusFeedback(orderRequestId, ORS_REJECTED, CTMT_ORDER_REJECTION, reason);
}


bool COrderRequestHandlerComp::UpdateOrderRequestStatus(
		const QByteArray& orderRequestId,
		OrderRequestStatus status,
		const QString& note)
{
	return ApplyStatusFeedback(orderRequestId, status, CTMT_ORDER_STATUS_UPDATE, note);
}


} // namespace imtauth
