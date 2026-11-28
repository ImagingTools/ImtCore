// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtassist/IAssistantLogicController.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace imtassistgql
{


/**
	GraphQL controller of the assistant.

	Commands (all take an \c input object):
	- \c AssistantGetStatus: no fields; returns \c available, whether the assistant is enabled
	  for the tenant of the request (used to show or hide the entry points of the assistant);
	- \c AssistantStartConversation: no fields; returns \c conversationId and \c messagesJson;
	- \c AssistantSendMessage: \c conversationId, \c text, optional \c contextItems
	  (list of \c kind, \c label, \c dataJson); returns \c conversationId and \c messagesJson;
	- \c AssistantConfirmPreview, \c AssistantRejectPreview: \c previewId; returns \c messagesJson;
	- \c AssistantCancelRequest: \c conversationId; returns \c canceled;
	- \c AssistantGetHistory: \c conversationId; returns \c conversationId and \c messagesJson;
	- \c AssistantCloseConversation: \c conversationId; returns \c closed.

	\c messagesJson is a JSON array of typed, versioned assistant messages (see imtassist::CAssistantMessage).
	User and tenant are taken from the authenticated GraphQL context. Access to the commands
	is gated by the command permissions configured for the component.

	\ingroup imtassistgql
*/
class CAssistantControllerComp: public imtservergql::CPermissibleGqlRequestHandlerComp
{
public:
	typedef imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_COMPONENT(CAssistantControllerComp);
		I_ASSIGN(m_assistantLogicControllerCompPtr, "AssistantLogicController", "Assistant logic controller", true, "AssistantLogicController");
		I_ASSIGN(m_allowDemoPrincipalAttrPtr, "AllowDemoPrincipal", "Process requests without an authenticated user as DemoUserId/DemoTenantId. Only for demo applications without authentication; never enable in production", true, false);
		I_ASSIGN(m_demoUserIdAttrPtr, "DemoUserId", "User ID used for requests without an authenticated user if AllowDemoPrincipal is set", false, "demo-user");
		I_ASSIGN(m_demoTenantIdAttrPtr, "DemoTenantId", "Tenant ID used for requests without an authenticated user if AllowDemoPrincipal is set", false, "demo-tenant");
	I_END_COMPONENT;

	static const QByteArray s_getStatusCommandId;
	static const QByteArray s_startConversationCommandId;
	static const QByteArray s_sendMessageCommandId;
	static const QByteArray s_confirmPreviewCommandId;
	static const QByteArray s_rejectPreviewCommandId;
	static const QByteArray s_cancelRequestCommandId;
	static const QByteArray s_getHistoryCommandId;
	static const QByteArray s_closeConversationCommandId;

	enum
	{
		MAX_CONTEXT_ITEMS = 10,
		MAX_CONTEXT_DATA_SIZE = 16384,
		MAX_CONTEXT_LABEL_LENGTH = 200,
		MAX_CONTEXT_KIND_LENGTH = 64
	};

protected:
	/**
		Create the request context from the authenticated GraphQL context.
		\return False if the request has no authenticated user (and the demo principal is not allowed).
	*/
	bool CreateRequestContext(const imtgql::CGqlRequest& gqlRequest, imtassist::CAssistantRequestContext& context) const;

	/**
		Read the context items released by the user. Invalid items make the request fail.
	*/
	bool ReadContextItems(
				const imtgql::CGqlParamObject& inputParams,
				imtassist::CAssistantRequestContext::ContextItems& items,
				QString& errorMessage) const;

	static QJsonObject CreateMessagesResponse(const QByteArray& conversationId, const imtassist::AssistantMessages& messages);

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtassist::IAssistantLogicController, m_assistantLogicControllerCompPtr);
	I_ATTR(bool, m_allowDemoPrincipalAttrPtr);
	I_ATTR(QByteArray, m_demoUserIdAttrPtr);
	I_ATTR(QByteArray, m_demoTenantIdAttrPtr);
};


} // namespace imtassistgql


