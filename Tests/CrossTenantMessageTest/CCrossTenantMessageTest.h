// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/ICrossTenantMessageHandler.h>


namespace imtauth
{


/**
	Self-contained mock projection replicating CCrossTenantMessageProjectionComp:
	a JSON field whitelist/blacklist applied to the payload.
*/
class CMockMessageProjection
{
public:
	QByteArray ProjectPayload(
				CrossTenantMessageType /*messageType*/,
				const QString& /*customType*/,
				const QByteArray& sourcePayload,
				bool& isValid) const
	{
		isValid = true;

		if (m_allowedFields.isEmpty() && m_blockedFields.isEmpty()){
			return sourcePayload;
		}

		QJsonParseError parseError;
		QJsonDocument document = QJsonDocument::fromJson(sourcePayload, &parseError);
		if (parseError.error != QJsonParseError::NoError || !document.isObject()){
			return sourcePayload;
		}

		const QJsonObject sourceObject = document.object();
		QJsonObject projectedObject;

		if (!m_allowedFields.isEmpty()){
			for (const QString& field : m_allowedFields){
				if (m_blockedFields.contains(field)){
					continue;
				}
				if (sourceObject.contains(field)){
					projectedObject.insert(field, sourceObject.value(field));
				}
			}
		}
		else {
			for (auto it = sourceObject.constBegin(); it != sourceObject.constEnd(); ++it){
				if (m_blockedFields.contains(it.key())){
					continue;
				}
				projectedObject.insert(it.key(), it.value());
			}
		}

		return QJsonDocument(projectedObject).toJson(QJsonDocument::Compact);
	}

	QStringList m_allowedFields;
	QSet<QString> m_blockedFields;
};


/**
	Mock domain handler that materializes an "order request" for order messages,
	demonstrating the Order -> OrderRequest scenario (RFC §15).
*/
class CMockOrderHandler
{
public:
	bool CanHandle(CrossTenantMessageType messageType, const QString& /*customType*/) const
	{
		return messageType == CTMT_ORDER_REQUEST;
	}

	bool HandleMessage(const CrossTenantMessageInfo& /*messageInfo*/, QByteArray& targetObjectId, QString& /*errorMessage*/)
	{
		targetObjectId = QByteArray("orderRequest-") + QByteArray::number(++m_counter);
		m_handledCount++;
		return true;
	}

	int m_counter = 0;
	int m_handledCount = 0;
};


/**
	Self-contained mock replicating the broker semantics of
	CCrossTenantMessageBrokerComp (relationship validation, projection and
	handler dispatch) without bringing up the component framework.
*/
class CMockMessageBroker
{
public:
	void RegisterRelationship(const QByteArray& sourceTenantId, const QByteArray& targetTenantId, const QByteArray& relationshipId)
	{
		m_relationships.append(QByteArray(sourceTenantId + '|' + targetTenantId + '|' + relationshipId));
	}

	bool ValidateRelationship(const QByteArray& sourceTenantId, const QByteArray& targetTenantId, const QByteArray& relationshipId) const
	{
		return m_relationships.contains(QByteArray(sourceTenantId + '|' + targetTenantId + '|' + relationshipId));
	}

	QByteArray SendMessage(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& relationshipId,
				CrossTenantMessageType messageType,
				const QByteArray& payload,
				const QByteArray& sourceObjectId = QByteArray(),
				const QString& customType = QString(),
				DocumentShareAccessMode accessMode = DSAM_READ_ONLY)
	{
		if (sourceTenantId.isEmpty() || targetTenantId.isEmpty() || relationshipId.isEmpty()){
			return QByteArray();
		}
		if (sourceTenantId == targetTenantId){
			return QByteArray();
		}
		if (!ValidateRelationship(sourceTenantId, targetTenantId, relationshipId)){
			return QByteArray();
		}

		QByteArray projectedPayload = payload;
		if (m_projectionPtr != nullptr){
			bool isValid = true;
			projectedPayload = m_projectionPtr->ProjectPayload(messageType, customType, payload, isValid);
			if (!isValid){
				return QByteArray();
			}
		}

		CrossTenantMessageInfo info;
		info.messageId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.relationshipId = relationshipId;
		info.sourceObjectId = sourceObjectId;
		info.messageType = messageType;
		info.customType = customType;
		info.payload = projectedPayload;
		info.status = CTMS_VALIDATED;
		info.accessMode = accessMode;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.updatedAt = info.createdAt;

		if (m_handlerPtr != nullptr && m_handlerPtr->CanHandle(messageType, customType)){
			QByteArray targetObjectId;
			QString errorMessage;
			if (m_handlerPtr->HandleMessage(info, targetObjectId, errorMessage)){
				info.targetObjectId = targetObjectId;
				info.status = CTMS_PROCESSED;
			}
			else {
				info.status = CTMS_FAILED;
				info.errorMessage = errorMessage;
			}
		}

		m_messages.append(info);
		return info.messageId;
	}

	CrossTenantMessageInfo GetMessage(const QByteArray& messageId) const
	{
		for (const CrossTenantMessageInfo& info : m_messages){
			if (info.messageId == messageId){
				return info;
			}
		}
		return CrossTenantMessageInfo();
	}

	CrossTenantMessages GetIncomingMessages(const QByteArray& targetTenantId) const
	{
		CrossTenantMessages result;
		for (const CrossTenantMessageInfo& info : m_messages){
			if (info.targetTenantId == targetTenantId){
				result.append(info);
			}
		}
		return result;
	}

	CrossTenantMessages GetOutgoingMessages(const QByteArray& sourceTenantId) const
	{
		CrossTenantMessages result;
		for (const CrossTenantMessageInfo& info : m_messages){
			if (info.sourceTenantId == sourceTenantId){
				result.append(info);
			}
		}
		return result;
	}

	QList<CrossTenantMessageInfo> m_messages;
	QList<QByteArray> m_relationships;
	CMockMessageProjection* m_projectionPtr = nullptr;
	CMockOrderHandler* m_handlerPtr = nullptr;
	int m_counter = 0;
};


} // namespace imtauth


class CCrossTenantMessageTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testMessageInfo_RoundTrip();
	void testMessageInfo_CopyFromAndClone();
	void testMessageInfo_ResetDataDefaults();

	// Projection
	void testProjection_PassThroughWhenUnconfigured();
	void testProjection_WhitelistKeepsAllowedFields();
	void testProjection_BlacklistRemovesBlockedFields();

	// Broker semantics
	void testSendMessage_RequiresRelationship();
	void testSendMessage_MissingFields_Fails();
	void testSendMessage_SameTenant_Fails();
	void testSendMessage_AppliesProjection();
	void testSendMessage_DispatchesToHandler();
	void testSendMessage_InboxOutbox();

private:
	imtauth::CMockMessageBroker* m_brokerPtr = nullptr;
	imtauth::CMockMessageProjection* m_projectionPtr = nullptr;
	imtauth::CMockOrderHandler* m_handlerPtr = nullptr;
};
