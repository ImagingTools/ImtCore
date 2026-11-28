// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CGqlAssistantActionCompBase.h>


// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/imtgql.h>
#include <imtservergql/imtservergql.h>


namespace imtassistgql
{


// reimplemented (imtassist::IAssistantAction)

imtassist::IAssistantAction::ExecutionStatus CGqlAssistantActionCompBase::Execute(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			Result& /*result*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


imtassist::IAssistantAction::ExecutionStatus CGqlAssistantActionCompBase::PrepareChange(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			ChangePreview& /*preview*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


imtassist::IAssistantAction::ExecutionStatus CGqlAssistantActionCompBase::ApplyChange(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			const QByteArray& /*stateToken*/,
			Result& /*result*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


// protected methods

const imtgql::IGqlContext* CGqlAssistantActionCompBase::GetGqlContext(const imtassist::CAssistantRequestContext& context)
{
	const imtgql::IGqlContext* gqlContextPtr = dynamic_cast<const imtgql::IGqlContext*>(context.GetTransportContext());
	if ((gqlContextPtr == nullptr) || !context.IsValid()){
		return nullptr;
	}

	if ((gqlContextPtr->GetUserId() != context.GetUserId()) || (gqlContextPtr->GetTenantId() != context.GetTenantId())){
		return nullptr;
	}

	return gqlContextPtr;
}


bool CGqlAssistantActionCompBase::HasPermission(const imtassist::CAssistantRequestContext& context, const QByteArray& permission)
{
	const imtgql::IGqlContext* gqlContextPtr = GetGqlContext(context);
	if (gqlContextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return false;
	}

	return userInfoPtr->IsAdmin() || imtauth::HasPermission(imtservergql::GetUserPermissions(*gqlContextPtr), permission);
}


bool CGqlAssistantActionCompBase::ExecuteGql(
			const imtassist::CAssistantRequestContext& context,
			const QByteArray& query,
			const QJsonObject& variables,
			QJsonObject& data,
			QString& errorMessage,
			const QByteArray& documentActionId) const
{
	data = QJsonObject();

	const imtgql::IGqlContext* gqlContextPtr = GetGqlContext(context);
	if (gqlContextPtr == nullptr){
		errorMessage = QStringLiteral("The request has no context of the user");

		return false;
	}

	QJsonObject body;
	body["query"] = QString::fromUtf8(query);
	body["variables"] = variables;

	imtgql::CGqlRequest request;
	qsizetype errorPosition = -1;
	if (!request.ParseQuery(QJsonDocument(body).toJson(QJsonDocument::Compact), errorPosition)){
		errorMessage = QStringLiteral("The request of the action could not be parsed");

		return false;
	}

	istd::IChangeableUniquePtr clonePtr = gqlContextPtr->CloneMe();
	istd::TUniqueInterfacePtr<imtgql::IGqlContext> requestContextPtr;
	if (!clonePtr.IsValid() || !requestContextPtr.MoveCastedPtr(std::move(clonePtr))){
		errorMessage = QStringLiteral("The context of the user could not be copied");

		return false;
	}

	request.SetGqlContext(imtgql::IGqlContextSharedPtr(std::move(requestContextPtr)));
	if (!documentActionId.isEmpty()){
		// Only on the copy of the context: the operation context of the handlers takes it over
		request.SetHeader(imtbase::s_documentActionHeaderId, documentActionId);
	}

	const QByteArray commandId = request.GetCommandId();

	for (int index = 0; index < m_requestHandlersCompPtr.GetCount(); ++index){
		const imtgql::IGqlRequestHandler* handlerPtr = m_requestHandlersCompPtr[index];
		if ((handlerPtr == nullptr) || !handlerPtr->IsRequestSupported(request)){
			continue;
		}

		QString handlerError;
		const QJsonObject response = handlerPtr->CreateResponse(request, handlerError);
		if (response.isEmpty() || response.contains(QStringLiteral("errors"))){
			// The text of the handler may contain names or values; the handler logs it itself.
			errorMessage = QStringLiteral("The command '%1' was refused or failed").arg(QString::fromUtf8(commandId));

			return false;
		}

		QJsonObject payload = response.contains(QStringLiteral("data")) ? response.value(QStringLiteral("data")).toObject() : response;
		const QString commandKey = QString::fromUtf8(commandId);
		if ((payload.size() == 1) && payload.value(commandKey).isObject()){
			payload = payload.value(commandKey).toObject();
		}

		data = payload;

		return true;
	}

	errorMessage = QStringLiteral("No handler for the command '%1'").arg(QString::fromUtf8(commandId));

	return false;
}


QByteArray CGqlAssistantActionCompBase::GetProductId(const imtassist::CAssistantRequestContext& context)
{
	const imtgql::IGqlContext* gqlContextPtr = GetGqlContext(context);

	return (gqlContextPtr != nullptr) ? gqlContextPtr->GetProductId() : QByteArray();
}


bool CGqlAssistantActionCompBase::BeginDocumentAction(
			const imtassist::CAssistantRequestContext& context,
			const QString& summary,
			QByteArray& documentActionId,
			const QByteArray& origin,
			const QByteArray& referenceId) const
{
	documentActionId.clear();

	if (!m_documentActionJournalCompPtr.IsValid()){
		return true;
	}

	documentActionId = m_documentActionJournalCompPtr->BeginDocumentAction(GetDataTenantId(context), context.GetUserId(), origin, summary, referenceId);
	if (documentActionId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Assistant action '%1': the document action could not be created").arg(QString::fromUtf8(GetActionId())), "AssistantDocumentActions");

		return false;
	}

	return true;
}


void CGqlAssistantActionCompBase::FinishDocumentAction(
			const imtassist::CAssistantRequestContext& context,
			const QByteArray& documentActionId,
			int appliedCount,
			int totalCount) const
{
	if (documentActionId.isEmpty() || !m_documentActionJournalCompPtr.IsValid()){
		return;
	}

	imtdb::IDocumentActionJournal::DocumentActionState state = imtdb::IDocumentActionJournal::DAS_PARTIALLY_APPLIED;
	if (appliedCount <= 0){
		state = imtdb::IDocumentActionJournal::DAS_FAILED;
	}
	else if (appliedCount >= totalCount){
		state = imtdb::IDocumentActionJournal::DAS_APPLIED;
	}

	if (!m_documentActionJournalCompPtr->SetDocumentActionState(GetDataTenantId(context), documentActionId, state)){
		SendErrorMessage(0, QStringLiteral("Assistant action '%1': the document action could not be finished").arg(QString::fromUtf8(GetActionId())), "AssistantDocumentActions");
	}
}


QByteArray CGqlAssistantActionCompBase::GetDataTenantId(const imtassist::CAssistantRequestContext& context, imtbase::TenantType* tenantTypePtr)
{
	imtbase::TenantType tenantType = imtbase::TT_ORGANIZATION;
	QByteArray tenantId = context.GetTenantId();

	const imtgql::IGqlContext* gqlContextPtr = GetGqlContext(context);
	if (gqlContextPtr != nullptr){
		tenantId = imtgql::GetDataTenantId(*gqlContextPtr, tenantType);
	}

	if (tenantTypePtr != nullptr){
		*tenantTypePtr = tenantType;
	}

	return tenantId;
}


imtdb::IDocumentActionJournal* CGqlAssistantActionCompBase::GetDocumentActionJournal() const
{
	return m_documentActionJournalCompPtr.IsValid() ? m_documentActionJournalCompPtr.GetPtr() : nullptr;
}


} // namespace imtassistgql


