// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtassist/CAssistantMessage.h>
#include <imtassist/CAssistantRequestContext.h>


namespace imtassist
{


/**
	Provider-independent assistant service.

	Every call takes the request context derived from the authenticated session.
	Conversations, previews and their outcomes are bound to the user and tenant of
	the context that created them; another principal cannot see or use them.

	Error codes of MK_ERROR messages:
	- \c AssistantDisabled: the assistant is not enabled for the application;
	- \c ProviderNotConfigured: no configured provider;
	- \c InvalidContext: the request has no authenticated user;
	- \c InvalidInput: the user message is empty or too long;
	- \c ConversationNotFound, \c Busy (another request of the conversation is running);
	- \c ProviderError, \c Timeout, \c Canceled, \c InvalidResponse;
	- \c UnknownAction, \c InvalidParameters, \c AccessDenied, \c InvalidRepresentation, \c ActionFailed;
	- \c TooManySteps: the turn needed more provider calls than allowed;
	- \c PreviewNotFound, \c PreviewExpired, \c PreviewRejected, \c PreviewInProgress, \c PreviewStale.

	Status codes of MK_ACTION_EXECUTED messages: \c succeeded, \c failed, \c accessDenied, \c stale.

	\ingroup imtassist
*/
class IAssistantController: virtual public istd::IPolymorphic
{
public:
	/**
		Check whether the assistant can be used by the principal of the context:
		it is enabled for the tenant of the context.
		Used by clients to show or hide the entry points of the assistant.
	*/
	virtual bool IsAvailable(const CAssistantRequestContext& context) const = 0;

	/**
		Start a new conversation for the principal of the context.
		\return Conversation ID, or empty if the assistant is disabled or the context is invalid.
	*/
	virtual QByteArray StartConversation(const CAssistantRequestContext& context) = 0;

	/**
		Process a user message. The call blocks until the turn is finished.
		\return Messages created in this turn, starting with the user message.
	*/
	virtual AssistantMessages SendUserMessage(
				const CAssistantRequestContext& context,
				const QByteArray& conversationId,
				const QString& text) = 0;

	/**
		Cancel the running request of a conversation.
		A change that was already applied is not undone.
		\return True if a running request was found and cancelled.
	*/
	virtual bool CancelRequest(const CAssistantRequestContext& context, const QByteArray& conversationId) = 0;

	/**
		Confirm a prepared change. Executes it at most once.
	*/
	virtual AssistantMessages ConfirmPreview(const CAssistantRequestContext& context, const QByteArray& previewId) = 0;

	/**
		Reject a prepared change. Nothing is changed.
	*/
	virtual AssistantMessages RejectPreview(const CAssistantRequestContext& context, const QByteArray& previewId) = 0;

	/**
		Get the messages of a conversation of the running session.
	*/
	virtual AssistantMessages GetHistory(const CAssistantRequestContext& context, const QByteArray& conversationId) const = 0;

	/**
		Remove a conversation and its history.
	*/
	virtual bool CloseConversation(const CAssistantRequestContext& context, const QByteArray& conversationId) = 0;
};


} // namespace imtassist


