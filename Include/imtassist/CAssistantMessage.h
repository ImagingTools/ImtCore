// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QStringList>

// ImtCore includes
#include <imtassist/IAssistantAction.h>


namespace imtassist
{


/**
	Typed, versioned message exchanged between the assistant and the UI.

	The UI must validate the kind and version before rendering. Success of a change
	is only reported by an MK_ACTION_EXECUTED message with status "succeeded", which
	is created from the server response of the action, never from model text.

	\ingroup imtassist
*/
class CAssistantMessage
{
public:
	enum
	{
		/**
			Current version of the message format.
		*/
		FORMAT_VERSION = 1
	};

	enum MessageKind
	{
		MK_USER_TEXT,
		MK_ASSISTANT_TEXT,
		MK_CLARIFICATION,
		/**
			Result of a reading action, optionally with a UI representation.
		*/
		MK_ACTION_RESULT,
		/**
			Prepared change waiting for confirmation.
		*/
		MK_ACTION_PREVIEW,
		/**
			Outcome of a confirmed change as reported by the server.
		*/
		MK_ACTION_EXECUTED,
		MK_ERROR,
		/**
			Status information, for example that a preview was rejected.
		*/
		MK_STATUS
	};

	CAssistantMessage() = default;
	CAssistantMessage(MessageKind kind, const QByteArray& conversationId, const QByteArray& correlationId);

	QByteArray GetMessageId() const;
	MessageKind GetKind() const;
	QByteArray GetConversationId() const;
	QByteArray GetCorrelationId() const;
	QDateTime GetCreatedAt() const;

	QString GetText() const;
	void SetText(const QString& text);

	/**
		Options of a clarification.
	*/
	QStringList GetOptions() const;
	void SetOptions(const QStringList& options);

	QByteArray GetActionId() const;
	void SetActionId(const QByteArray& actionId);

	QByteArray GetPreviewId() const;
	void SetPreviewId(const QByteArray& previewId);

	/**
		Error code for MK_ERROR, status code for MK_STATUS and MK_ACTION_EXECUTED.
	*/
	QByteArray GetCode() const;
	void SetCode(const QByteArray& code);

	QJsonObject GetData() const;
	void SetData(const QJsonObject& data);

	IAssistantAction::Representation GetRepresentation() const;
	void SetRepresentation(const IAssistantAction::Representation& representation);

	QJsonObject ToJson() const;

	static QString GetKindName(MessageKind kind);

private:
	QByteArray m_messageId;
	MessageKind m_kind = MK_STATUS;
	QByteArray m_conversationId;
	QByteArray m_correlationId;
	QDateTime m_createdAt;
	QString m_text;
	QStringList m_options;
	QByteArray m_actionId;
	QByteArray m_previewId;
	QByteArray m_code;
	QJsonObject m_data;
	IAssistantAction::Representation m_representation;
};


typedef QList<CAssistantMessage> AssistantMessages;


} // namespace imtassist


