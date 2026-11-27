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
	2. the first rule whose keyword is contained in the latest user message, if the
	   latest message of the request is a user message;
	3. the default reply.

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
	void AddRule(const QString& keyword, const Reply& reply);

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

private:
	struct Rule
	{
		QString keyword;
		Reply reply;
	};

	mutable QMutex m_mutex;
	bool m_isConfigured = true;
	QQueue<Reply> m_scriptedReplies;
	QList<Rule> m_rules;
	Reply m_defaultReply;
	CompletionHook m_completionHook;
	QList<Request> m_receivedRequests;
};


} // namespace imtassist


