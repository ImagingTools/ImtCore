// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <functional>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QQueue>

// ImtCore includes
#include <imtassist/IAssistantProvider.h>


namespace imtassist
{


/**
	Deterministic provider for development and tests. It never contacts a network.

	A reply is selected in this order:
	1. the next scripted reply, if any;
	2. the tool result reply, if the latest message of the request is a tool result and it is set;
	3. the first rule whose keyword is contained in the latest user message, if the
	   latest message of the request is a user message;
	4. the default reply.

	String parameters of an action call of the form \c $context.<kind>.<field> are
	replaced by the field of the released context item of that kind. If the request
	has no such item, a clarification with the question of the rule is returned instead.

	All received requests are recorded, so tests can check what was sent.

	\ingroup imtassist
*/
class CFakeAssistantProvider: virtual public IAssistantProvider
{
public:
	typedef std::function<void(const Request& request)> CompletionHook;

	CFakeAssistantProvider();

	void SetConfigured(bool isConfigured);

	/**
		Add a reply that is returned by the next call.
	*/
	void AddScriptedReply(const Reply& reply);

	/**
		Add a rule: a user message containing the keyword (case-insensitive) is answered with the reply.
	*/
	void AddRule(const QString& keyword, const Reply& reply, const QString& missingContextQuestion = QString());

	/**
		Set the reply to a request whose latest message is a tool result. An empty reply text disables it.
	*/
	void SetToolResultReply(const Reply& reply);

	void SetDefaultReply(const Reply& reply);

	/**
		Set a function called during every completion, for example to cancel the request.
	*/
	void SetCompletionHook(const CompletionHook& hook);

	QList<Request> GetReceivedRequests() const;
	int GetCallCount() const;

	static Reply CreateTextReply(const QString& text);
	static Reply CreateClarificationReply(const QString& question, const QStringList& options = QStringList());
	static Reply CreateActionCallReply(const QByteArray& actionId, const QJsonObject& parameters);
	static Reply CreateErrorReply(ReplyStatus status);

	// reimplemented (imtassist::IAssistantProvider)
	virtual bool IsConfigured() const override;
	virtual Reply Complete(const Request& request, const CCancellationToken& cancellationToken) override;

private:
	Reply SelectReply(const Request& request);
	static Reply ResolveContextPlaceholders(const Request& request, const Reply& reply, const QString& missingContextQuestion);

private:
	struct Rule
	{
		QString keyword;
		Reply reply;
		QString missingContextQuestion;
	};

	mutable QMutex m_mutex;
	bool m_isConfigured = true;
	QQueue<Reply> m_scriptedReplies;
	QList<Rule> m_rules;
	Reply m_defaultReply;
	Reply m_toolResultReply;
	bool m_hasToolResultReply = false;
	CompletionHook m_completionHook;
	QList<Request> m_receivedRequests;
};


} // namespace imtassist


