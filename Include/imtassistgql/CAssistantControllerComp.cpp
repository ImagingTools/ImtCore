// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantControllerComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtassistgql
{


namespace
{


bool IsValidContextKind(const QString& kind)
{
	if (kind.isEmpty() || (kind.size() > CAssistantControllerComp::MAX_CONTEXT_KIND_LENGTH)){
		return false;
	}

	for (const QChar& character: kind){
		bool isAllowed = (character.isLetterOrNumber() && (character.unicode() < 128)) || (character == '.') || (character == '_') || (character == '-');
		if (!isAllowed){
			return false;
		}
	}

	return true;
}


} // namespace


const QByteArray CAssistantControllerComp::s_startConversationCommandId = QByteArrayLiteral("AssistantStartConversation");
const QByteArray CAssistantControllerComp::s_sendMessageCommandId = QByteArrayLiteral("AssistantSendMessage");
const QByteArray CAssistantControllerComp::s_confirmPreviewCommandId = QByteArrayLiteral("AssistantConfirmPreview");
const QByteArray CAssistantControllerComp::s_rejectPreviewCommandId = QByteArrayLiteral("AssistantRejectPreview");
const QByteArray CAssistantControllerComp::s_cancelRequestCommandId = QByteArrayLiteral("AssistantCancelRequest");
const QByteArray CAssistantControllerComp::s_getHistoryCommandId = QByteArrayLiteral("AssistantGetHistory");
const QByteArray CAssistantControllerComp::s_closeConversationCommandId = QByteArrayLiteral("AssistantCloseConversation");


// protected methods

bool CAssistantControllerComp::CreateRequestContext(const imtgql::CGqlRequest& gqlRequest, imtassist::CAssistantRequestContext& context) const
{
	QByteArray userId;
	QByteArray tenantId;

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		userId = gqlContextPtr->GetUserId();
		tenantId = gqlContextPtr->GetTenantId();
	}

	if (userId.isEmpty() && m_allowDemoPrincipalAttrPtr.IsValid() && *m_allowDemoPrincipalAttrPtr){
		userId = m_demoUserIdAttrPtr.IsValid() ? *m_demoUserIdAttrPtr : QByteArray();
		tenantId = m_demoTenantIdAttrPtr.IsValid() ? *m_demoTenantIdAttrPtr : QByteArray();
	}

	context.SetUserId(userId);
	context.SetTenantId(tenantId);
	context.SetCorrelationId(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));

	return context.IsValid();
}


bool CAssistantControllerComp::ReadContextItems(
			const imtgql::CGqlParamObject& inputParams,
			imtassist::CAssistantRequestContext::ContextItems& items,
			QString& errorMessage) const
{
	QList<const imtgql::CGqlParamObject*> itemParams = inputParams.GetParamArgumentObjectPtrList("contextItems");
	if (itemParams.size() > MAX_CONTEXT_ITEMS){
		errorMessage = QStringLiteral("Too many context items");

		return false;
	}

	for (const imtgql::CGqlParamObject* itemParamPtr: itemParams){
		if (itemParamPtr == nullptr){
			errorMessage = QStringLiteral("Invalid context item");

			return false;
		}

		imtassist::CAssistantRequestContext::ContextItem item;

		QString kind = itemParamPtr->GetParamArgumentValue("kind").toString();
		if (!IsValidContextKind(kind)){
			errorMessage = QStringLiteral("Invalid kind of a context item");

			return false;
		}

		item.kind = kind.toUtf8();
		item.label = itemParamPtr->GetParamArgumentValue("label").toString().left(MAX_CONTEXT_LABEL_LENGTH);

		QByteArray dataJson = itemParamPtr->GetParamArgumentValue("dataJson").toString().toUtf8();
		if (dataJson.size() > MAX_CONTEXT_DATA_SIZE){
			errorMessage = QStringLiteral("Data of a context item is too large");

			return false;
		}

		if (!dataJson.isEmpty()){
			QJsonParseError parseError;
			QJsonDocument document = QJsonDocument::fromJson(dataJson, &parseError);
			if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
				errorMessage = QStringLiteral("Data of a context item is not a JSON object");

				return false;
			}

			item.data = document.object();
		}

		items.append(item);
	}

	return true;
}


QJsonObject CAssistantControllerComp::CreateMessagesResponse(const QByteArray& conversationId, const imtassist::AssistantMessages& messages)
{
	QJsonObject retVal;
	retVal["conversationId"] = QString::fromUtf8(conversationId);
	retVal["messagesJson"] = QString::fromUtf8(QJsonDocument(imtassist::CAssistantMessage::ToJsonArray(messages)).toJson(QJsonDocument::Compact));

	return retVal;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

QJsonObject CAssistantControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	imtassist::IAssistantService* servicePtr = m_assistantServiceCompPtr.GetPtr();
	if (servicePtr == nullptr){
		errorMessage = QStringLiteral("The assistant is not available");
		SendErrorMessage(0, QStringLiteral("No assistant service is configured"), "AssistantController");

		return QJsonObject();
	}

	imtassist::CAssistantRequestContext context;
	if (!CreateRequestContext(gqlRequest, context)){
		QJsonObject errors;
		errors["message"] = QStringLiteral("The request has no authenticated user");
		errors["type"] = QStringLiteral("Unauthorized");

		QJsonObject retVal;
		retVal["errors"] = errors;

		return retVal;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	imtgql::CGqlParamObject emptyInput;
	const imtgql::CGqlParamObject& input = (inputPtr != nullptr) ? *inputPtr : emptyInput;

	QByteArray conversationId = input.GetParamArgumentValue("conversationId").toString().toUtf8();
	QByteArray previewId = input.GetParamArgumentValue("previewId").toString().toUtf8();

	if (commandId == s_startConversationCommandId){
		conversationId = servicePtr->StartConversation(context);

		imtassist::AssistantMessages messages;
		if (conversationId.isEmpty()){
			imtassist::CAssistantMessage error(imtassist::CAssistantMessage::MK_ERROR, QByteArray(), context.GetCorrelationId());
			error.SetCode("AssistantDisabled");
			error.SetText(QStringLiteral("The assistant is not available."));
			messages.append(error);
		}

		return CreateMessagesResponse(conversationId, messages);
	}

	if (commandId == s_sendMessageCommandId){
		imtassist::CAssistantRequestContext::ContextItems items;
		if (!ReadContextItems(input, items, errorMessage)){
			return QJsonObject();
		}

		context.SetContextItems(items);

		QString text = input.GetParamArgumentValue("text").toString();

		return CreateMessagesResponse(conversationId, servicePtr->SendUserMessage(context, conversationId, text));
	}

	if (commandId == s_confirmPreviewCommandId){
		return CreateMessagesResponse(conversationId, servicePtr->ConfirmPreview(context, previewId));
	}

	if (commandId == s_rejectPreviewCommandId){
		return CreateMessagesResponse(conversationId, servicePtr->RejectPreview(context, previewId));
	}

	if (commandId == s_getHistoryCommandId){
		return CreateMessagesResponse(conversationId, servicePtr->GetHistory(context, conversationId));
	}

	if (commandId == s_cancelRequestCommandId){
		QJsonObject retVal;
		retVal["canceled"] = servicePtr->CancelRequest(context, conversationId);

		return retVal;
	}

	if (commandId == s_closeConversationCommandId){
		QJsonObject retVal;
		retVal["closed"] = servicePtr->CloseConversation(context, conversationId);

		return retVal;
	}

	errorMessage = QStringLiteral("Unsupported assistant command");

	return QJsonObject();
}


// reimplemented (icomp::CComponentBase)

void CAssistantControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_allowDemoPrincipalAttrPtr.IsValid() && *m_allowDemoPrincipalAttrPtr){
		SendWarningMessage(
					0,
					QStringLiteral("AllowDemoPrincipal is enabled: requests without an authenticated user are processed as a demo user. Never use this in production."),
					"AssistantController");
	}
}


} // namespace imtassistgql


