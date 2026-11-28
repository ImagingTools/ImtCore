// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtassist/IAssistantController.h>
#include <imtassist/IAssistantProvider.h>
#include <imtassist/IAssistantTenantPolicy.h>
#include <imtassist/IAssistantActionRegistry.h>
#include <imtassist/IUiRepresentationRegistry.h>
#include <imtassist/IAssistantAuditLog.h>
#include <imtassist/CActionPreviewStore.h>


namespace imtassist
{


/**
	Default implementation of the assistant service.

	The service is disabled by default. While it is disabled or has no configured
	provider, no request is sent to a provider.

	One turn runs a loop of at most GetMaxStepsPerTurn() provider calls:
	- a text or clarification reply ends the turn;
	- a call of a reading action is validated, executed, and its result is passed
	  back to the provider as untrusted data;
	- a call of a changing action is validated and prepared; the preview ends the
	  turn and waits for the user. The provider can never confirm a preview.

	Conversations are kept in memory for the running session only.
	The service does not own the provider, the registries and the audit log.

	\ingroup imtassist
*/
class CAssistantService: virtual public IAssistantController
{
public:
	enum
	{
		DEFAULT_MAX_STEPS_PER_TURN = 5,
		DEFAULT_PROVIDER_TIMEOUT_MS = 30000,
		MAX_USER_TEXT_LENGTH = 8000,
		MAX_REPLY_TEXT_LENGTH = 16000,
		MAX_HISTORY_SIZE = 200,
		MAX_CONVERSATIONS_PER_PRINCIPAL = 20
	};

	CAssistantService();

	void SetEnabled(bool isEnabled);
	bool IsEnabled() const;

	void SetProvider(IAssistantProvider* providerPtr);
	void SetTenantPolicy(const IAssistantTenantPolicy* policyPtr);
	void SetActionRegistry(const IAssistantActionRegistry* registryPtr);
	void SetRepresentationRegistry(const IUiRepresentationRegistry* registryPtr);
	void SetAuditLog(IAssistantAuditLog* auditLogPtr);

	void SetProviderTimeout(int timeoutMs);
	void SetMaxStepsPerTurn(int maxSteps);
	int GetMaxStepsPerTurn() const;

	CActionPreviewStore& GetPreviewStore();

	// reimplemented (imtassist::IAssistantController)
	virtual bool IsAvailable(const CAssistantRequestContext& context) const override;
	virtual QByteArray StartConversation(const CAssistantRequestContext& context) override;
	virtual AssistantMessages SendUserMessage(
				const CAssistantRequestContext& context,
				const QByteArray& conversationId,
				const QString& text) override;
	virtual bool CancelRequest(const CAssistantRequestContext& context, const QByteArray& conversationId) override;
	virtual AssistantMessages ConfirmPreview(const CAssistantRequestContext& context, const QByteArray& previewId) override;
	virtual AssistantMessages RejectPreview(const CAssistantRequestContext& context, const QByteArray& previewId) override;
	virtual AssistantMessages GetHistory(const CAssistantRequestContext& context, const QByteArray& conversationId) const override;
	virtual bool CloseConversation(const CAssistantRequestContext& context, const QByteArray& conversationId) override;

protected:
	/**
		Check whether the assistant is enabled server-wide. Called with the internal lock held;
		an override must not call back into the service.
	*/
	virtual bool IsAssistantEnabled() const;

	/**
		Check whether the assistant is enabled for the tenant: enabled server-wide
		and allowed by the tenant policy, if one is set. Called with the internal lock held.
	*/
	bool IsAssistantEnabledForTenant(const QByteArray& tenantId) const;

private:
	struct Conversation
	{
		QByteArray userId;
		QByteArray tenantId;
		AssistantMessages history;
		IAssistantProvider::Messages providerMessages;
		bool isRunning = false;
		CCancellationToken cancellationToken;
	};

	enum StepResult
	{
		SR_CONTINUE,
		SR_FINISHED
	};

	bool IsConversationOf(const Conversation& conversation, const CAssistantRequestContext& context) const;
	StepResult ProcessActionCall(
				const CAssistantRequestContext& context,
				const QByteArray& conversationId,
				const IAssistantProvider::Reply& reply,
				IAssistantProvider::Messages& providerMessages,
				AssistantMessages& messages);
	static void AppendFailedToolResult(
				IAssistantProvider::Messages& providerMessages,
				const QByteArray& actionId,
				const QByteArray& callId,
				const QByteArray& code);
	IAssistantProvider::ToolDescriptions CreateToolDescriptions() const;
	CAssistantMessage CreateError(
				const CAssistantRequestContext& context,
				const QByteArray& conversationId,
				const QByteArray& code,
				const QString& text) const;
	CAssistantMessage CreateExecutedMessage(
				const CAssistantRequestContext& context,
				const CActionPreviewStore::Preview& preview) const;
	void AppendToConversation(
				const CAssistantRequestContext& context,
				const QByteArray& conversationId,
				const AssistantMessages& messages,
				const IAssistantProvider::Messages& providerMessages);
	void Log(
				IAssistantAuditLog::EventType type,
				const CAssistantRequestContext& context,
				const QByteArray& outcome,
				const QByteArray& actionId = QByteArray(),
				const QByteArray& previewId = QByteArray()) const;

	static QByteArray GetExecutionStatusCode(IAssistantAction::ExecutionStatus status);

private:
	mutable QMutex m_mutex;
	bool m_isEnabled = false;
	IAssistantProvider* m_providerPtr = nullptr;
	const IAssistantTenantPolicy* m_tenantPolicyPtr = nullptr;
	const IAssistantActionRegistry* m_actionRegistryPtr = nullptr;
	const IUiRepresentationRegistry* m_representationRegistryPtr = nullptr;
	IAssistantAuditLog* m_auditLogPtr = nullptr;
	int m_providerTimeoutMs = DEFAULT_PROVIDER_TIMEOUT_MS;
	int m_maxStepsPerTurn = DEFAULT_MAX_STEPS_PER_TURN;
	CActionPreviewStore m_previewStore;
	QMap<QByteArray, Conversation> m_conversations;
};


} // namespace imtassist


