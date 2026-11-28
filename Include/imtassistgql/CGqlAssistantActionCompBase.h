// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IChangeSetJournal.h>
#include <imtbase/TenantType.h>
#include <imtassist/IAssistantAction.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtassistgql
{


/**
	Base of assistant actions that use the existing GraphQL API of the application.
	A request is executed by the GraphQL handlers of the application with the authenticated
	GraphQL context of the assistant request, so the handlers check permissions and the
	isolation of the tenants exactly as for a request of the user interface: the assistant
	can do nothing the user could not do in the application.

	Without the GraphQL context of the user (e.g. a demo principal) the actions refuse access.

	\ingroup imtassistgql
*/
class CGqlAssistantActionCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtassist::IAssistantAction
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	enum
	{
		MAX_RESPONSE_ITEMS = 500
	};

	I_BEGIN_BASE_COMPONENT(CGqlAssistantActionCompBase);
		I_REGISTER_INTERFACE(imtassist::IAssistantAction);
		I_ASSIGN_MULTI_0(m_requestHandlersCompPtr, "RequestHandlers", "GraphQL handlers of the application that process the requests of the action", true);
		I_ASSIGN(m_changeSetJournalCompPtr, "ChangeSetJournal", "Journal of the change sets: with it the confirmed changes can be rolled back (changes.rollback)", false, "ChangeSetJournal");
	I_END_COMPONENT;

	// reimplemented (imtassist::IAssistantAction)
	virtual ExecutionStatus Execute(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) override;
	virtual ExecutionStatus PrepareChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				ChangePreview& preview,
				QString& userError) override;
	virtual ExecutionStatus ApplyChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				const QByteArray& stateToken,
				Result& result,
				QString& userError) override;

protected:
	/**
		Get the GraphQL context of the request; nullptr if missing or of another user or tenant.
	*/
	static const imtgql::IGqlContext* GetGqlContext(const imtassist::CAssistantRequestContext& context);

	/**
		Check that the user of the request holds the permission (administrators hold all).
		The handlers check the permissions again; this check only avoids offering calls that fail.
	*/
	static bool HasPermission(const imtassist::CAssistantRequestContext& context, const QByteArray& permission);

	/**
		Execute a GraphQL request with the context of the user.
		User input is passed only in \c variables, never in the query text.
		\param data			Payload of the command.
		\param errorMessage	Technical error for the log, without the text of the handler.
		\param changeSetId	Change set the handlers write into (header imtbase::s_changeSetHeaderId); empty for none.
	*/
	bool ExecuteGql(
				const imtassist::CAssistantRequestContext& context,
				const QByteArray& query,
				const QJsonObject& variables,
				QJsonObject& data,
				QString& errorMessage,
				const QByteArray& changeSetId = QByteArray()) const;

	/**
		Begin the change set of a confirmed change. Without a journal the change goes ahead without one (empty ID).
		
eturn False if the journal could not create the change set; then nothing may be changed.
	*/
	bool BeginChangeSet(
				const imtassist::CAssistantRequestContext& context,
				const QString& summary,
				QByteArray& changeSetId,
				const QByteArray& origin = QByteArrayLiteral("assistant"),
				const QByteArray& referenceId = QByteArray()) const;

	/**
		Finish the change set after \c appliedCount of \c totalCount changes were applied.
	*/
	void FinishChangeSet(const imtassist::CAssistantRequestContext& context, const QByteArray& changeSetId, int appliedCount, int totalCount) const;

	/**
		Tenant of the data of the request, as the operation context of the handlers has it: the organization,
		or the user for a personal space. The change sets of the journal belong to it.
	*/
	static QByteArray GetDataTenantId(const imtassist::CAssistantRequestContext& context, imtbase::TenantType* tenantTypePtr = nullptr);

	/**
		Journal of the change sets, nullptr if not configured.
	*/
	imtbase::IChangeSetJournal* GetChangeSetJournal() const;

	/**
		Product of the request, for the commands that take a \c productId.
	*/
	static QByteArray GetProductId(const imtassist::CAssistantRequestContext& context);

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_requestHandlersCompPtr);
	I_REF(imtbase::IChangeSetJournal, m_changeSetJournalCompPtr);
};


} // namespace imtassistgql


