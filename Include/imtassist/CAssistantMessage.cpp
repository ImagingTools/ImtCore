// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantMessage.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QUuid>


namespace imtassist
{


CAssistantMessage::CAssistantMessage(MessageKind kind, const QByteArray& conversationId, const QByteArray& correlationId)
	:m_messageId(QUuid::createUuid().toByteArray(QUuid::WithoutBraces)),
	m_kind(kind),
	m_conversationId(conversationId),
	m_correlationId(correlationId),
	m_createdAt(QDateTime::currentDateTimeUtc())
{
}


QByteArray CAssistantMessage::GetMessageId() const
{
	return m_messageId;
}


CAssistantMessage::MessageKind CAssistantMessage::GetKind() const
{
	return m_kind;
}


QByteArray CAssistantMessage::GetConversationId() const
{
	return m_conversationId;
}


QByteArray CAssistantMessage::GetCorrelationId() const
{
	return m_correlationId;
}


QDateTime CAssistantMessage::GetCreatedAt() const
{
	return m_createdAt;
}


QString CAssistantMessage::GetText() const
{
	return m_text;
}


void CAssistantMessage::SetText(const QString& text)
{
	m_text = text;
}


QStringList CAssistantMessage::GetOptions() const
{
	return m_options;
}


void CAssistantMessage::SetOptions(const QStringList& options)
{
	m_options = options;
}


QByteArray CAssistantMessage::GetActionId() const
{
	return m_actionId;
}


void CAssistantMessage::SetActionId(const QByteArray& actionId)
{
	m_actionId = actionId;
}


QByteArray CAssistantMessage::GetPreviewId() const
{
	return m_previewId;
}


void CAssistantMessage::SetPreviewId(const QByteArray& previewId)
{
	m_previewId = previewId;
}


QByteArray CAssistantMessage::GetCode() const
{
	return m_code;
}


void CAssistantMessage::SetCode(const QByteArray& code)
{
	m_code = code;
}


QJsonObject CAssistantMessage::GetData() const
{
	return m_data;
}


void CAssistantMessage::SetData(const QJsonObject& data)
{
	m_data = data;
}


IAssistantAction::Representation CAssistantMessage::GetRepresentation() const
{
	return m_representation;
}


void CAssistantMessage::SetRepresentation(const IAssistantAction::Representation& representation)
{
	m_representation = representation;
}


QJsonObject CAssistantMessage::ToJson() const
{
	QJsonObject retVal;
	retVal["formatVersion"] = int(FORMAT_VERSION);
	retVal["id"] = QString::fromUtf8(m_messageId);
	retVal["kind"] = GetKindName(m_kind);
	retVal["conversationId"] = QString::fromUtf8(m_conversationId);
	retVal["correlationId"] = QString::fromUtf8(m_correlationId);
	retVal["createdAt"] = m_createdAt.toString(Qt::ISODateWithMs);

	if (!m_text.isEmpty()){
		retVal["text"] = m_text;
	}

	if (!m_options.isEmpty()){
		retVal["options"] = QJsonArray::fromStringList(m_options);
	}

	if (!m_actionId.isEmpty()){
		retVal["actionId"] = QString::fromUtf8(m_actionId);
	}

	if (!m_previewId.isEmpty()){
		retVal["previewId"] = QString::fromUtf8(m_previewId);
	}

	if (!m_code.isEmpty()){
		retVal["code"] = QString::fromUtf8(m_code);
	}

	if (!m_data.isEmpty()){
		retVal["data"] = m_data;
	}

	if (!m_representation.IsEmpty()){
		QJsonObject representation;
		representation["type"] = QString::fromUtf8(m_representation.type);
		representation["version"] = m_representation.version;
		representation["data"] = m_representation.data;

		retVal["representation"] = representation;
	}

	return retVal;
}


QString CAssistantMessage::GetKindName(MessageKind kind)
{
	switch (kind){
	case MK_USER_TEXT:
		return "userText";
	case MK_ASSISTANT_TEXT:
		return "assistantText";
	case MK_CLARIFICATION:
		return "clarification";
	case MK_ACTION_RESULT:
		return "actionResult";
	case MK_ACTION_PREVIEW:
		return "actionPreview";
	case MK_ACTION_EXECUTED:
		return "actionExecuted";
	case MK_ERROR:
		return "error";
	case MK_STATUS:
		return "status";
	}

	return QString();
}


} // namespace imtassist


