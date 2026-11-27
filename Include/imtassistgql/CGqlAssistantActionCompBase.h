// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
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
		\param errorMessage	Error of the handler, for the log; not shown to the user as is.
	*/
	bool ExecuteGql(
				const imtassist::CAssistantRequestContext& context,
				const QByteArray& query,
				const QJsonObject& variables,
				QJsonObject& data,
				QString& errorMessage) const;

	/**
		Product of the request, for the commands that take a \c productId.
	*/
	static QByteArray GetProductId(const imtassist::CAssistantRequestContext& context);

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_requestHandlersCompPtr);
};


} // namespace imtassistgql


