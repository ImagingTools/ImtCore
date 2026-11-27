// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantService.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QUuid>


namespace imtassist
{


namespace
{


const int s_maxClarificationOptions = 10;
const int s_maxClarificationOptionLength = 200;


bool IsValidReplyText(const QString& text)
{
	return !text.trimmed().isEmpty() && (text.size() <= CAssistantService::MAX_REPLY_TEXT_LENGTH);
}


bool AreValidChanges(const QJsonArray& changes)
{
	for (const QJsonValue& change: changes){
		if (!change.isObject()){
			return false;
		}
	}

	return true;
}


} // namespace


CAssistantService::CAssistantService()
{
}


void CAssistantService::SetEnabled(bool isEnabled)
{
	QMutexLocker lock(&m_mutex);

	m_isEnabled = isEnabled;
}


bool CAssistantService::IsEnabled() const
{
	QMutexLocker lock(&m_mutex);

	return IsAssistantEnabled();
}


void CAssistantService::SetProvider(IAssistantProvider* providerPtr)
{
	QMutexLocker lock(&m_mutex);

	m_providerPtr = providerPtr;
}


void CAssistantService::SetTenantPolicy(const IAssistantTenantPolicy* policyPtr)
{
	QMutexLocker lock(&m_mutex);

	m_tenantPolicyPtr = policyPtr;
}


void CAssistantService::SetActionRegistry(const IAssistantActionRegistry* registryPtr)
{
	QMutexLocker lock(&m_mutex);

	m_actionRegistryPtr = registryPtr;
}


void CAssistantService::SetRepresentationRegistry(const IUiRepresentationRegistry* registryPtr)
{
	QMutexLocker lock(&m_mutex);

	m_representationRegistryPtr = registryPtr;
}


void CAssistantService::SetAuditLog(IAssistantAuditLog* auditLogPtr)
{
	QMutexLocker lock(&m_mutex);

	m_auditLogPtr = auditLogPtr;
}


void CAssistantService::SetProviderTimeout(int timeoutMs)
{
	QMutexLocker lock(&m_mutex);

	m_providerTimeoutMs = qMax(1, timeoutMs);
}


void CAssistantService::SetMaxStepsPerTurn(int maxSteps)
{
	QMutexLocker lock(&m_mutex);

	m_maxStepsPerTurn = qMax(1, maxSteps);
}


int CAssistantService::GetMaxStepsPerTurn() const
{
	QMutexLocker lock(&m_mutex);

	return m_maxStepsPerTurn;
}


CActionPreviewStore& CAssistantService::GetPreviewStore()
{
	return m_previewStore;
}


// reimplemented (imtassist::IAssistantService)

bool CAssistantService::IsAvailable(const CAssistantRequestContext& context) const
{
	if (!context.IsValid()){
		return false;
	}

	QMutexLocker lock(&m_mutex);

	return IsAssistantEnabledForTenant(context.GetTenantId());
}


QByteArray CAssistantService::StartConversation(const CAssistantRequestContext& context)
{
	if (!context.IsValid()){
		Log(IAssistantAuditLog::ET_REQUEST_REJECTED, context, "InvalidContext");

		return QByteArray();
	}

	QMutexLocker lock(&m_mutex);

	if (!IsAssistantEnabledForTenant(context.GetTenantId())){
		lock.unlock();
		Log(IAssistantAuditLog::ET_REQUEST_REJECTED, context, "AssistantDisabled");

		return QByteArray();
	}

	// Limit the number of conversations of one principal: drop the first idle one.
	int count = 0;
	QByteArray idleId;
	for (QMap<QByteArray, Conversation>::const_iterator iter = m_conversations.constBegin(); iter != m_conversations.constEnd(); ++iter){
		if (IsConversationOf(iter.value(), context)){
			++count;
			if (idleId.isEmpty() && !iter.value().isRunning){
				idleId = iter.key();
			}
		}
	}

	if (count >= MAX_CONVERSATIONS_PER_PRINCIPAL){
		if (idleId.isEmpty()){
			return QByteArray();
		}

		m_conversations.remove(idleId);
	}

	QByteArray conversationId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	Conversation conversation;
	conversation.userId = context.GetUserId();
	conversation.tenantId = context.GetTenantId();

	m_conversations.insert(conversationId, conversation);

	return conversationId;
}


AssistantMessages CAssistantService::SendUserMessage(
			const CAssistantRequestContext& context,
			const QByteArray& conversationId,
			const QString& text)
{
	AssistantMessages retVal;

	if (!context.IsValid()){
		Log(IAssistantAuditLog::ET_REQUEST_REJECTED, context, "InvalidContext");
		retVal.append(CreateError(context, conversationId, "InvalidContext", "The request has no authenticated user."));

		return retVal;
	}

	IAssistantProvider* providerPtr = nullptr;
	int timeoutMs = 0;
	int maxSteps = 0;
	CCancellationToken cancellationToken;
	IAssistantProvider::Messages providerMessages;
	{
		QMutexLocker lock(&m_mutex);

		providerPtr = m_providerPtr;
		timeoutMs = m_providerTimeoutMs;
		maxSteps = m_maxStepsPerTurn;

		QByteArray rejectionCode;
		QString rejectionText;
		if (!IsAssistantEnabledForTenant(context.GetTenantId())){
			rejectionCode = "AssistantDisabled";
			rejectionText = "The assistant is not enabled.";
		}
		else if ((providerPtr == nullptr) || !providerPtr->IsConfigured()){
			rejectionCode = "ProviderNotConfigured";
			rejectionText = "No assistant provider is configured.";
		}
		else if (text.trimmed().isEmpty() || (text.size() > MAX_USER_TEXT_LENGTH)){
			rejectionCode = "InvalidInput";
			rejectionText = "The message is empty or too long.";
		}
		else{
			QMap<QByteArray, Conversation>::iterator iter = m_conversations.find(conversationId);
			if ((iter == m_conversations.end()) || !IsConversationOf(iter.value(), context)){
				rejectionCode = "ConversationNotFound";
				rejectionText = "The conversation does not exist.";
			}
			else if (iter.value().isRunning){
				rejectionCode = "Busy";
				rejectionText = "Another request of this conversation is being processed.";
			}
			else{
				iter.value().isRunning = true;
				iter.value().cancellationToken = CCancellationToken();
				cancellationToken = iter.value().cancellationToken;
				providerMessages = iter.value().providerMessages;
			}
		}

		if (!rejectionCode.isEmpty()){
			lock.unlock();
			Log(IAssistantAuditLog::ET_REQUEST_REJECTED, context, rejectionCode);
			retVal.append(CreateError(context, conversationId, rejectionCode, rejectionText));

			return retVal;
		}
	}

	Log(IAssistantAuditLog::ET_REQUEST_STARTED, context, "ok");

	CAssistantMessage userMessage(CAssistantMessage::MK_USER_TEXT, conversationId, context.GetCorrelationId());
	userMessage.SetText(text);
	retVal.append(userMessage);

	IAssistantProvider::Message userProviderMessage;
	userProviderMessage.role = IAssistantProvider::Message::R_USER;
	userProviderMessage.text = text;
	providerMessages.append(userProviderMessage);

	// Context released by the user for this request only. It is not kept in the
	// conversation, so that context removed by the user is not sent again.
	IAssistantProvider::Messages contextMessages;
	for (const CAssistantRequestContext::ContextItem& item: context.GetContextItems()){
		IAssistantProvider::Message contextMessage;
		contextMessage.role = IAssistantProvider::Message::R_CONTEXT;
		contextMessage.actionId = item.kind;
		contextMessage.text = item.label;
		contextMessage.data = item.data;
		contextMessages.append(contextMessage);
	}

	IAssistantProvider::ToolDescriptions tools = CreateToolDescriptions();

	bool isFinished = false;
	for (int step = 0; (step < maxSteps) && !isFinished; ++step){
		if (cancellationToken.IsCanceled()){
			break;
		}

		IAssistantProvider::Request request;
		request.correlationId = context.GetCorrelationId();
		request.tenantId = context.GetTenantId();
		request.messages = contextMessages + providerMessages;
		request.tools = tools;
		request.timeoutMs = timeoutMs;

		IAssistantProvider::Reply reply = providerPtr->Complete(request, cancellationToken);

		if (cancellationToken.IsCanceled()){
			Log(IAssistantAuditLog::ET_PROVIDER_CALL, context, "Canceled");

			break;
		}

		QByteArray errorCode;
		QString errorText;
		switch (reply.status){
		case IAssistantProvider::RS_OK:
			break;
		case IAssistantProvider::RS_TIMEOUT:
			errorCode = "Timeout";
			errorText = "The assistant did not answer in time.";
			break;
		case IAssistantProvider::RS_CANCELED:
			errorCode = "Canceled";
			errorText = "The request was cancelled.";
			break;
		case IAssistantProvider::RS_NOT_CONFIGURED:
			errorCode = "ProviderNotConfigured";
			errorText = "No assistant provider is configured.";
			break;
		case IAssistantProvider::RS_INVALID_RESPONSE:
			errorCode = "InvalidResponse";
			errorText = "The assistant returned an invalid answer.";
			break;
		default:
			errorCode = "ProviderError";
			errorText = "The assistant is currently not available.";
			break;
		}

		Log(IAssistantAuditLog::ET_PROVIDER_CALL, context, errorCode.isEmpty() ? QByteArray("ok") : errorCode);

		if (!errorCode.isEmpty()){
			retVal.append(CreateError(context, conversationId, errorCode, errorText));
			isFinished = true;

			break;
		}

		switch (reply.kind){
		case IAssistantProvider::RK_TEXT:
			if (!IsValidReplyText(reply.text)){
				retVal.append(CreateError(context, conversationId, "InvalidResponse", "The assistant returned an invalid answer."));
			}
			else{
				CAssistantMessage message(CAssistantMessage::MK_ASSISTANT_TEXT, conversationId, context.GetCorrelationId());
				message.SetText(reply.text);
				retVal.append(message);

				IAssistantProvider::Message assistantMessage;
				assistantMessage.role = IAssistantProvider::Message::R_ASSISTANT;
				assistantMessage.text = reply.text;
				providerMessages.append(assistantMessage);
			}
			isFinished = true;
			break;

		case IAssistantProvider::RK_CLARIFICATION:
			{
				bool areOptionsValid = (reply.options.size() <= s_maxClarificationOptions);
				for (const QString& option: reply.options){
					if (option.trimmed().isEmpty() || (option.size() > s_maxClarificationOptionLength)){
						areOptionsValid = false;
					}
				}

				if (!IsValidReplyText(reply.text) || !areOptionsValid){
					retVal.append(CreateError(context, conversationId, "InvalidResponse", "The assistant returned an invalid answer."));
				}
				else{
					CAssistantMessage message(CAssistantMessage::MK_CLARIFICATION, conversationId, context.GetCorrelationId());
					message.SetText(reply.text);
					message.SetOptions(reply.options);
					retVal.append(message);

					IAssistantProvider::Message assistantMessage;
					assistantMessage.role = IAssistantProvider::Message::R_ASSISTANT;
					assistantMessage.text = reply.text;
					providerMessages.append(assistantMessage);
				}
				isFinished = true;
			}
			break;

		case IAssistantProvider::RK_ACTION_CALL:
			isFinished = (ProcessActionCall(context, conversationId, reply, providerMessages, retVal) == SR_FINISHED);
			break;

		default:
			retVal.append(CreateError(context, conversationId, "InvalidResponse", "The assistant returned an invalid answer."));
			isFinished = true;
			break;
		}
	}

	if (cancellationToken.IsCanceled()){
		bool hasCanceledError = false;
		for (const CAssistantMessage& message: retVal){
			if ((message.GetKind() == CAssistantMessage::MK_ERROR) && (message.GetCode() == "Canceled")){
				hasCanceledError = true;
			}
		}

		if (!hasCanceledError){
			retVal.append(CreateError(context, conversationId, "Canceled", "The request was cancelled."));
		}
	}
	else if (!isFinished){
		Log(IAssistantAuditLog::ET_REQUEST_REJECTED, context, "TooManySteps");
		retVal.append(CreateError(context, conversationId, "TooManySteps", "The request could not be completed."));
	}

	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Conversation>::iterator iter = m_conversations.find(conversationId);
	if ((iter != m_conversations.end()) && IsConversationOf(iter.value(), context)){
		Conversation& conversation = iter.value();
		conversation.isRunning = false;
		conversation.history.append(retVal);
		conversation.providerMessages = providerMessages;

		while (conversation.history.size() > MAX_HISTORY_SIZE){
			conversation.history.removeFirst();
		}

		while (conversation.providerMessages.size() > MAX_HISTORY_SIZE){
			conversation.providerMessages.removeFirst();
		}
	}

	return retVal;
}


bool CAssistantService::CancelRequest(const CAssistantRequestContext& context, const QByteArray& conversationId)
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Conversation>::iterator iter = m_conversations.find(conversationId);
	if ((iter == m_conversations.end()) || !IsConversationOf(iter.value(), context) || !iter.value().isRunning){
		return false;
	}

	iter.value().cancellationToken.Cancel();

	lock.unlock();
	Log(IAssistantAuditLog::ET_REQUEST_CANCELED, context, "ok");

	return true;
}


AssistantMessages CAssistantService::ConfirmPreview(const CAssistantRequestContext& context, const QByteArray& previewId)
{
	AssistantMessages retVal;

	if (!context.IsValid()){
		Log(IAssistantAuditLog::ET_PREVIEW_CONFIRMED, context, "InvalidContext", QByteArray(), previewId);
		retVal.append(CreateError(context, QByteArray(), "InvalidContext", "The request has no authenticated user."));

		return retVal;
	}

	const IAssistantActionRegistry* registryPtr = nullptr;
	const IUiRepresentationRegistry* representationRegistryPtr = nullptr;
	{
		QMutexLocker lock(&m_mutex);

		if (!IsAssistantEnabledForTenant(context.GetTenantId())){
			lock.unlock();
			Log(IAssistantAuditLog::ET_PREVIEW_CONFIRMED, context, "AssistantDisabled", QByteArray(), previewId);
			retVal.append(CreateError(context, QByteArray(), "AssistantDisabled", "The assistant is not enabled."));

			return retVal;
		}

		registryPtr = m_actionRegistryPtr;
		representationRegistryPtr = m_representationRegistryPtr;
	}

	CActionPreviewStore::Preview preview;
	CActionPreviewStore::ClaimResult claimResult = m_previewStore.ClaimForExecution(previewId, context, preview);

	QByteArray errorCode;
	QString errorText;
	switch (claimResult){
	case CActionPreviewStore::CR_CLAIMED:
		break;
	case CActionPreviewStore::CR_EXPIRED:
		errorCode = "PreviewExpired";
		errorText = "The proposed change has expired. Please prepare it again.";
		break;
	case CActionPreviewStore::CR_REJECTED:
		errorCode = "PreviewRejected";
		errorText = "The proposed change was rejected.";
		break;
	case CActionPreviewStore::CR_IN_PROGRESS:
		errorCode = "PreviewInProgress";
		errorText = "The proposed change is being executed.";
		break;
	case CActionPreviewStore::CR_ALREADY_COMPLETED:
		{
			Log(IAssistantAuditLog::ET_PREVIEW_CONFIRMED, context, "alreadyProcessed", preview.actionId, previewId);

			CAssistantMessage statusMessage(CAssistantMessage::MK_STATUS, preview.conversationId, context.GetCorrelationId());
			statusMessage.SetPreviewId(previewId);
			statusMessage.SetActionId(preview.actionId);
			statusMessage.SetCode("alreadyProcessed");
			retVal.append(statusMessage);
			retVal.append(CreateExecutedMessage(context, preview));
		}
		return retVal;
	default:
		errorCode = "PreviewNotFound";
		errorText = "The proposed change does not exist.";
		break;
	}

	if (!errorCode.isEmpty()){
		Log(IAssistantAuditLog::ET_PREVIEW_CONFIRMED, context, errorCode, preview.actionId, previewId);
		retVal.append(CreateError(context, preview.conversationId, errorCode, errorText));

		return retVal;
	}

	IAssistantAction* actionPtr = (registryPtr != nullptr) ? registryPtr->GetAction(preview.actionId) : nullptr;

	IAssistantAction::ExecutionStatus status = IAssistantAction::ES_FAILED;
	IAssistantAction::Result result;
	QString userError;
	QByteArray outcome;

	if ((actionPtr == nullptr) || (actionPtr->GetActionKind() != IAssistantAction::AK_CHANGE)){
		status = IAssistantAction::ES_FAILED;
		outcome = "UnknownAction";
	}
	else if ((actionPtr->GetParameterSchema().GetVersion() != preview.schemaVersion) || !actionPtr->GetParameterSchema().Validate(preview.parameters)){
		status = IAssistantAction::ES_STALE;
		outcome = "PreviewStale";
	}
	else if (!actionPtr->CheckAccess(context, preview.parameters)){
		status = IAssistantAction::ES_ACCESS_DENIED;
		outcome = "AccessDenied";
	}
	else{
		status = actionPtr->ApplyChange(context, preview.parameters, preview.change.stateToken, result, userError);
		outcome = GetExecutionStatusCode(status);

		if (status == IAssistantAction::ES_SUCCEEDED){
			// The change was applied; invalid result data is dropped, but the success is reported.
			if (!actionPtr->GetResultSchema().Validate(result.data)){
				result.data = QJsonObject();
				outcome = "succeededWithInvalidResult";
			}

			if (!result.representation.IsEmpty()){
				bool isValid = (representationRegistryPtr != nullptr) && representationRegistryPtr->ValidateRepresentation(
							result.representation.type,
							result.representation.version,
							result.representation.data);
				if (!isValid){
					result.representation = IAssistantAction::Representation();
				}
			}
		}
		else{
			result = IAssistantAction::Result();
		}
	}

	m_previewStore.CompleteExecution(previewId, status, result, userError);

	preview.state = CActionPreviewStore::PS_COMPLETED;
	preview.executionStatus = status;
	preview.result = result;
	preview.userError = userError;

	Log(IAssistantAuditLog::ET_PREVIEW_CONFIRMED, context, outcome, preview.actionId, previewId);

	retVal.append(CreateExecutedMessage(context, preview));

	IAssistantProvider::Message toolMessage;
	toolMessage.role = IAssistantProvider::Message::R_TOOL_RESULT;
	toolMessage.actionId = preview.actionId;
	toolMessage.data["status"] = QString::fromUtf8(GetExecutionStatusCode(status));
	if (status == IAssistantAction::ES_SUCCEEDED){
		toolMessage.data["result"] = result.data;
	}

	AppendToConversation(context, preview.conversationId, retVal, IAssistantProvider::Messages() << toolMessage);

	return retVal;
}


AssistantMessages CAssistantService::RejectPreview(const CAssistantRequestContext& context, const QByteArray& previewId)
{
	AssistantMessages retVal;

	CActionPreviewStore::Preview preview;
	if (!m_previewStore.GetPreview(previewId, context, preview)){
		Log(IAssistantAuditLog::ET_PREVIEW_REJECTED, context, "PreviewNotFound", QByteArray(), previewId);
		retVal.append(CreateError(context, QByteArray(), "PreviewNotFound", "The proposed change does not exist."));

		return retVal;
	}

	if (!m_previewStore.RejectPreview(previewId, context)){
		Log(IAssistantAuditLog::ET_PREVIEW_REJECTED, context, "PreviewNotPending", preview.actionId, previewId);

		// The preview is no longer pending: report its actual state, never a rejection that did not happen.
		CActionPreviewStore::Preview current;
		if (!m_previewStore.GetPreview(previewId, context, current)){
			retVal.append(CreateError(context, preview.conversationId, "PreviewNotFound", "The proposed change does not exist."));
		}
		else if (current.state == CActionPreviewStore::PS_COMPLETED){
			retVal.append(CreateExecutedMessage(context, current));
		}
		else if (current.state == CActionPreviewStore::PS_EXECUTING){
			retVal.append(CreateError(context, preview.conversationId, "PreviewInProgress", "The proposed change is being executed."));
		}
		else{
			retVal.append(CreateError(context, preview.conversationId, "PreviewRejected", "The proposed change was rejected."));
		}

		return retVal;
	}

	Log(IAssistantAuditLog::ET_PREVIEW_REJECTED, context, "ok", preview.actionId, previewId);

	CAssistantMessage statusMessage(CAssistantMessage::MK_STATUS, preview.conversationId, context.GetCorrelationId());
	statusMessage.SetPreviewId(previewId);
	statusMessage.SetActionId(preview.actionId);
	statusMessage.SetCode("previewRejected");
	retVal.append(statusMessage);

	IAssistantProvider::Message toolMessage;
	toolMessage.role = IAssistantProvider::Message::R_TOOL_RESULT;
	toolMessage.actionId = preview.actionId;
	toolMessage.data["status"] = QStringLiteral("rejectedByUser");

	AppendToConversation(context, preview.conversationId, retVal, IAssistantProvider::Messages() << toolMessage);

	return retVal;
}


AssistantMessages CAssistantService::GetHistory(const CAssistantRequestContext& context, const QByteArray& conversationId) const
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Conversation>::const_iterator iter = m_conversations.constFind(conversationId);
	if ((iter == m_conversations.constEnd()) || !IsConversationOf(iter.value(), context)){
		return AssistantMessages();
	}

	return iter.value().history;
}


bool CAssistantService::CloseConversation(const CAssistantRequestContext& context, const QByteArray& conversationId)
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Conversation>::iterator iter = m_conversations.find(conversationId);
	if ((iter == m_conversations.end()) || !IsConversationOf(iter.value(), context)){
		return false;
	}

	iter.value().cancellationToken.Cancel();
	m_conversations.erase(iter);

	return true;
}


// protected methods

bool CAssistantService::IsAssistantEnabled() const
{
	return m_isEnabled;
}


bool CAssistantService::IsAssistantEnabledForTenant(const QByteArray& tenantId) const
{
	if (!IsAssistantEnabled()){
		return false;
	}

	return (m_tenantPolicyPtr == nullptr) || m_tenantPolicyPtr->IsAssistantEnabledForTenant(tenantId);
}


// private methods

bool CAssistantService::IsConversationOf(const Conversation& conversation, const CAssistantRequestContext& context) const
{
	return context.IsValid() && (conversation.userId == context.GetUserId()) && (conversation.tenantId == context.GetTenantId());
}


CAssistantService::StepResult CAssistantService::ProcessActionCall(
			const CAssistantRequestContext& context,
			const QByteArray& conversationId,
			const IAssistantProvider::Reply& reply,
			IAssistantProvider::Messages& providerMessages,
			AssistantMessages& messages)
{
	const IAssistantActionRegistry* registryPtr = nullptr;
	const IUiRepresentationRegistry* representationRegistryPtr = nullptr;
	{
		QMutexLocker lock(&m_mutex);

		registryPtr = m_actionRegistryPtr;
		representationRegistryPtr = m_representationRegistryPtr;
	}

	IAssistantAction* actionPtr = (registryPtr != nullptr) ? registryPtr->GetAction(reply.actionId) : nullptr;
	if (actionPtr == nullptr){
		Log(IAssistantAuditLog::ET_ACTION_REJECTED, context, "UnknownAction");

		CAssistantMessage error = CreateError(context, conversationId, "UnknownAction", "The assistant requested a function that is not available.");
		messages.append(error);

		return SR_FINISHED;
	}

	QByteArray actionId = actionPtr->GetActionId();

	if (!actionPtr->GetParameterSchema().Validate(reply.parameters)){
		Log(IAssistantAuditLog::ET_ACTION_REJECTED, context, "InvalidParameters", actionId);

		CAssistantMessage error = CreateError(context, conversationId, "InvalidParameters", "The assistant requested a function with invalid parameters.");
		error.SetActionId(actionId);
		messages.append(error);

		return SR_FINISHED;
	}

	if (!actionPtr->CheckAccess(context, reply.parameters)){
		Log(IAssistantAuditLog::ET_ACTION_REJECTED, context, "AccessDenied", actionId);

		CAssistantMessage error = CreateError(context, conversationId, "AccessDenied", "You are not allowed to perform this function.");
		error.SetActionId(actionId);
		messages.append(error);

		return SR_FINISHED;
	}

	// IDs of 9 letters and digits are accepted by every supported provider API.
	QByteArray callId = reply.callId;
	if (callId.isEmpty() || (callId.size() > 128)){
		callId = QUuid::createUuid().toByteArray(QUuid::Id128).left(9);
	}

	IAssistantProvider::Message callMessage;
	callMessage.role = IAssistantProvider::Message::R_ASSISTANT;
	callMessage.actionId = actionId;
	callMessage.data = reply.parameters;
	callMessage.callId = callId;
	callMessage.providerState = reply.providerState;
	providerMessages.append(callMessage);

	if (actionPtr->GetActionKind() == IAssistantAction::AK_READ){
		IAssistantAction::Result result;
		QString userError;
		IAssistantAction::ExecutionStatus status = actionPtr->Execute(context, reply.parameters, result, userError);
		if (status != IAssistantAction::ES_SUCCEEDED){
			QByteArray code = (status == IAssistantAction::ES_ACCESS_DENIED) ? QByteArray("AccessDenied") : QByteArray("ActionFailed");
			Log(IAssistantAuditLog::ET_ACTION_EXECUTED, context, code, actionId);

			CAssistantMessage error = CreateError(context, conversationId, code, userError.isEmpty() ? QString("The function could not be executed.") : userError);
			error.SetActionId(actionId);
			messages.append(error);

			AppendFailedToolResult(providerMessages, actionId, callId, code);

			return SR_FINISHED;
		}

		if (!actionPtr->GetResultSchema().Validate(result.data)){
			Log(IAssistantAuditLog::ET_ACTION_EXECUTED, context, "InvalidResult", actionId);

			CAssistantMessage error = CreateError(context, conversationId, "ActionFailed", "The function returned an invalid result.");
			error.SetActionId(actionId);
			messages.append(error);

			AppendFailedToolResult(providerMessages, actionId, callId, "ActionFailed");

			return SR_FINISHED;
		}

		if (!result.representation.IsEmpty()){
			bool isValid = (representationRegistryPtr != nullptr) && representationRegistryPtr->ValidateRepresentation(
						result.representation.type,
						result.representation.version,
						result.representation.data);
			if (!isValid){
				Log(IAssistantAuditLog::ET_ACTION_REJECTED, context, "InvalidRepresentation", actionId);

				CAssistantMessage error = CreateError(context, conversationId, "InvalidRepresentation", "The result cannot be displayed.");
				error.SetActionId(actionId);
				messages.append(error);

				result.representation = IAssistantAction::Representation();
			}
		}

		Log(IAssistantAuditLog::ET_ACTION_EXECUTED, context, "ok", actionId);

		CAssistantMessage resultMessage(CAssistantMessage::MK_ACTION_RESULT, conversationId, context.GetCorrelationId());
		resultMessage.SetActionId(actionId);
		resultMessage.SetText(result.text);
		resultMessage.SetData(result.data);
		resultMessage.SetRepresentation(result.representation);
		messages.append(resultMessage);

		IAssistantProvider::Message toolMessage;
		toolMessage.role = IAssistantProvider::Message::R_TOOL_RESULT;
		toolMessage.actionId = actionId;
		toolMessage.callId = callId;
		toolMessage.data = result.data;
		providerMessages.append(toolMessage);

		return SR_CONTINUE;
	}

	IAssistantAction::ChangePreview change;
	QString userError;
	IAssistantAction::ExecutionStatus status = actionPtr->PrepareChange(context, reply.parameters, change, userError);
	if ((status != IAssistantAction::ES_SUCCEEDED) || !AreValidChanges(change.changes)){
		QByteArray code = (status == IAssistantAction::ES_ACCESS_DENIED) ? QByteArray("AccessDenied") : QByteArray("ActionFailed");
		Log(IAssistantAuditLog::ET_ACTION_REJECTED, context, code, actionId);

		CAssistantMessage error = CreateError(context, conversationId, code, userError.isEmpty() ? QString("The change could not be prepared.") : userError);
		error.SetActionId(actionId);
		messages.append(error);

		AppendFailedToolResult(providerMessages, actionId, callId, code);

		return SR_FINISHED;
	}

	CActionPreviewStore::Preview preview;
	preview.actionId = actionId;
	preview.schemaVersion = actionPtr->GetParameterSchema().GetVersion();
	preview.conversationId = conversationId;
	preview.parameters = reply.parameters;
	preview.change = change;

	QByteArray previewId = m_previewStore.AddPreview(context, preview);
	if (previewId.isEmpty() || !m_previewStore.GetPreview(previewId, context, preview)){
		messages.append(CreateError(context, conversationId, "InvalidContext", "The request has no authenticated user."));

		AppendFailedToolResult(providerMessages, actionId, callId, "InvalidContext");

		return SR_FINISHED;
	}

	Log(IAssistantAuditLog::ET_PREVIEW_CREATED, context, "ok", actionId, previewId);

	QJsonObject data;
	data["targetId"] = QString::fromUtf8(change.targetId);
	data["targetLabel"] = change.targetLabel;
	data["changes"] = change.changes;
	data["impact"] = change.impact;
	data["expiresAt"] = preview.expiresAt.toString(Qt::ISODate);

	CAssistantMessage previewMessage(CAssistantMessage::MK_ACTION_PREVIEW, conversationId, context.GetCorrelationId());
	previewMessage.SetActionId(actionId);
	previewMessage.SetPreviewId(previewId);
	previewMessage.SetText(change.summary);
	previewMessage.SetData(data);
	messages.append(previewMessage);

	IAssistantProvider::Message toolMessage;
	toolMessage.role = IAssistantProvider::Message::R_TOOL_RESULT;
	toolMessage.actionId = actionId;
	toolMessage.callId = callId;
	toolMessage.data["status"] = QStringLiteral("waitingForUserConfirmation");
	providerMessages.append(toolMessage);

	return SR_FINISHED;
}


void CAssistantService::AppendFailedToolResult(
			IAssistantProvider::Messages& providerMessages,
			const QByteArray& actionId,
			const QByteArray& callId,
			const QByteArray& code)
{
	IAssistantProvider::Message toolMessage;
	toolMessage.role = IAssistantProvider::Message::R_TOOL_RESULT;
	toolMessage.actionId = actionId;
	toolMessage.callId = callId;
	toolMessage.data["status"] = QStringLiteral("failed");
	toolMessage.data["code"] = QString::fromUtf8(code);
	providerMessages.append(toolMessage);
}


IAssistantProvider::ToolDescriptions CAssistantService::CreateToolDescriptions() const
{
	const IAssistantActionRegistry* registryPtr = nullptr;
	{
		QMutexLocker lock(&m_mutex);

		registryPtr = m_actionRegistryPtr;
	}

	IAssistantProvider::ToolDescriptions retVal;
	if (registryPtr == nullptr){
		return retVal;
	}

	for (const QByteArray& actionId: registryPtr->GetActionIds()){
		const IAssistantAction* actionPtr = registryPtr->GetAction(actionId);
		if (actionPtr == nullptr){
			continue;
		}

		IAssistantProvider::ToolDescription tool;
		tool.actionId = actionId;
		tool.description = actionPtr->GetDescription();
		tool.isChanging = (actionPtr->GetActionKind() == IAssistantAction::AK_CHANGE);
		tool.parameterSchema = actionPtr->GetParameterSchema().ToJsonSchema();
		retVal.append(tool);
	}

	return retVal;
}


CAssistantMessage CAssistantService::CreateError(
			const CAssistantRequestContext& context,
			const QByteArray& conversationId,
			const QByteArray& code,
			const QString& text) const
{
	CAssistantMessage retVal(CAssistantMessage::MK_ERROR, conversationId, context.GetCorrelationId());
	retVal.SetCode(code);
	retVal.SetText(text);

	return retVal;
}


CAssistantMessage CAssistantService::CreateExecutedMessage(
			const CAssistantRequestContext& context,
			const CActionPreviewStore::Preview& preview) const
{
	CAssistantMessage retVal(CAssistantMessage::MK_ACTION_EXECUTED, preview.conversationId, context.GetCorrelationId());
	retVal.SetActionId(preview.actionId);
	retVal.SetPreviewId(preview.previewId);
	retVal.SetCode(GetExecutionStatusCode(preview.executionStatus));

	if (preview.executionStatus == IAssistantAction::ES_SUCCEEDED){
		retVal.SetText(preview.result.text);
		retVal.SetData(preview.result.data);
		retVal.SetRepresentation(preview.result.representation);
	}
	else if (preview.executionStatus == IAssistantAction::ES_STALE){
		retVal.SetText(preview.userError.isEmpty() ? QString("The target has changed. Please prepare the change again.") : preview.userError);
	}
	else if (preview.executionStatus == IAssistantAction::ES_ACCESS_DENIED){
		retVal.SetText(preview.userError.isEmpty() ? QString("You are not allowed to perform this change.") : preview.userError);
	}
	else{
		retVal.SetText(preview.userError.isEmpty() ? QString("The change could not be executed.") : preview.userError);
	}

	return retVal;
}


void CAssistantService::AppendToConversation(
			const CAssistantRequestContext& context,
			const QByteArray& conversationId,
			const AssistantMessages& messages,
			const IAssistantProvider::Messages& providerMessages)
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Conversation>::iterator iter = m_conversations.find(conversationId);
	if ((iter == m_conversations.end()) || !IsConversationOf(iter.value(), context)){
		return;
	}

	Conversation& conversation = iter.value();
	conversation.history.append(messages);
	while (conversation.history.size() > MAX_HISTORY_SIZE){
		conversation.history.removeFirst();
	}

	// A running turn writes its provider messages back when it ends; do not interfere with it.
	if (!conversation.isRunning){
		conversation.providerMessages.append(providerMessages);
		while (conversation.providerMessages.size() > MAX_HISTORY_SIZE){
			conversation.providerMessages.removeFirst();
		}
	}
}


void CAssistantService::Log(
			IAssistantAuditLog::EventType type,
			const CAssistantRequestContext& context,
			const QByteArray& outcome,
			const QByteArray& actionId,
			const QByteArray& previewId) const
{
	IAssistantAuditLog* auditLogPtr = nullptr;
	{
		QMutexLocker lock(&m_mutex);

		auditLogPtr = m_auditLogPtr;
	}

	if (auditLogPtr == nullptr){
		return;
	}

	IAssistantAuditLog::Event event;
	event.type = type;
	event.correlationId = context.GetCorrelationId();
	event.userId = context.GetUserId();
	event.tenantId = context.GetTenantId();
	event.actionId = actionId;
	event.previewId = previewId;
	event.outcome = outcome;

	auditLogPtr->LogEvent(event);
}


QByteArray CAssistantService::GetExecutionStatusCode(IAssistantAction::ExecutionStatus status)
{
	switch (status){
	case IAssistantAction::ES_SUCCEEDED:
		return "succeeded";
	case IAssistantAction::ES_ACCESS_DENIED:
		return "accessDenied";
	case IAssistantAction::ES_STALE:
		return "stale";
	default:
		return "failed";
	}
}


} // namespace imtassist


