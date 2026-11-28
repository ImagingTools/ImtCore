// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CFakeAssistantProvider.h>


// Qt includes
#include <QtCore/QMutexLocker>


namespace imtassist
{


CFakeAssistantProvider::CFakeAssistantProvider()
	:m_defaultReply(CreateTextReply("OK"))
{
}


void CFakeAssistantProvider::SetConfigured(bool isConfigured)
{
	QMutexLocker lock(&m_mutex);

	m_isConfigured = isConfigured;
}


void CFakeAssistantProvider::AddScriptedReply(const Reply& reply)
{
	QMutexLocker lock(&m_mutex);

	m_scriptedReplies.enqueue(reply);
}


void CFakeAssistantProvider::AddRule(const QString& keyword, const Reply& reply, const QString& missingContextQuestion)
{
	QMutexLocker lock(&m_mutex);

	Rule rule;
	rule.keyword = keyword;
	rule.reply = reply;
	rule.missingContextQuestion = missingContextQuestion;

	m_rules.append(rule);
}


void CFakeAssistantProvider::SetToolResultReply(const Reply& reply)
{
	QMutexLocker lock(&m_mutex);

	m_toolResultReply = reply;
	m_hasToolResultReply = !reply.text.isEmpty();
}


void CFakeAssistantProvider::SetDefaultReply(const Reply& reply)
{
	QMutexLocker lock(&m_mutex);

	m_defaultReply = reply;
}


void CFakeAssistantProvider::SetCompletionHook(const CompletionHook& hook)
{
	QMutexLocker lock(&m_mutex);

	m_completionHook = hook;
}


QList<IAssistantProvider::Request> CFakeAssistantProvider::GetReceivedRequests() const
{
	QMutexLocker lock(&m_mutex);

	return m_receivedRequests;
}


int CFakeAssistantProvider::GetCallCount() const
{
	QMutexLocker lock(&m_mutex);

	return m_receivedRequests.size();
}


IAssistantProvider::Reply CFakeAssistantProvider::CreateTextReply(const QString& text)
{
	Reply retVal;
	retVal.status = RS_OK;
	retVal.kind = RK_TEXT;
	retVal.text = text;

	return retVal;
}


IAssistantProvider::Reply CFakeAssistantProvider::CreateClarificationReply(const QString& question, const QStringList& options)
{
	Reply retVal;
	retVal.status = RS_OK;
	retVal.kind = RK_CLARIFICATION;
	retVal.text = question;
	retVal.options = options;

	return retVal;
}


IAssistantProvider::Reply CFakeAssistantProvider::CreateActionCallReply(const QByteArray& actionId, const QJsonObject& parameters)
{
	Reply retVal;
	retVal.status = RS_OK;
	retVal.kind = RK_ACTION_CALL;
	retVal.actionId = actionId;
	retVal.parameters = parameters;

	return retVal;
}


IAssistantProvider::Reply CFakeAssistantProvider::CreateErrorReply(ReplyStatus status)
{
	Reply retVal;
	retVal.status = status;

	return retVal;
}


// reimplemented (imtassist::IAssistantProvider)

bool CFakeAssistantProvider::IsConfigured() const
{
	QMutexLocker lock(&m_mutex);

	return m_isConfigured;
}


IAssistantProvider::Reply CFakeAssistantProvider::Complete(const Request& request, const CCancellationToken& cancellationToken)
{
	CompletionHook hook;
	{
		QMutexLocker lock(&m_mutex);

		m_receivedRequests.append(request);
		hook = m_completionHook;
	}

	if (hook){
		hook(request);
	}

	Reply reply = SelectReply(request);

	// A cancelled request consumes its reply, like a real request that was answered too late.
	if (cancellationToken.IsCanceled()){
		return CreateErrorReply(RS_CANCELED);
	}

	return reply;
}


// private methods

IAssistantProvider::Reply CFakeAssistantProvider::SelectReply(const Request& request)
{
	QMutexLocker lock(&m_mutex);

	if (!m_scriptedReplies.isEmpty()){
		return m_scriptedReplies.dequeue();
	}

	if (!request.messages.isEmpty() && (request.messages.last().role == Message::R_TOOL_RESULT) && m_hasToolResultReply){
		return m_toolResultReply;
	}

	if (!request.messages.isEmpty() && (request.messages.last().role == Message::R_USER)){
		QString userText = request.messages.last().text;
		for (const Rule& rule: m_rules){
			if (userText.contains(rule.keyword, Qt::CaseInsensitive)){
				return ResolveContextPlaceholders(request, rule.reply, rule.missingContextQuestion);
			}
		}
	}

	return m_defaultReply;
}


IAssistantProvider::Reply CFakeAssistantProvider::ResolveContextPlaceholders(
			const Request& request,
			const Reply& reply,
			const QString& missingContextQuestion)
{
	if (reply.kind != RK_ACTION_CALL){
		return reply;
	}

	static const QString prefix = QStringLiteral("$context.");

	Reply retVal = reply;
	for (QJsonObject::iterator iter = retVal.parameters.begin(); iter != retVal.parameters.end(); ++iter){
		QString value = iter.value().toString();
		if (!iter.value().isString() || !value.startsWith(prefix)){
			continue;
		}

		QStringList parts = value.mid(prefix.size()).split('.');
		if (parts.size() != 2){
			continue;
		}

		QJsonValue resolvedValue;
		bool isResolved = false;
		for (const Message& message: request.messages){
			if ((message.role == Message::R_CONTEXT) && (message.actionId == parts.at(0).toUtf8()) && message.data.contains(parts.at(1))){
				resolvedValue = message.data.value(parts.at(1));
				isResolved = true;
			}
		}

		if (!isResolved){
			return CreateClarificationReply(missingContextQuestion.isEmpty() ? QStringLiteral("Which object do you mean?") : missingContextQuestion);
		}

		iter.value() = resolvedValue;
	}

	return retVal;
}


} // namespace imtassist


