// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtassist/CAssistantRequestContext.h>
#include <imtassist/CParameterSchema.h>


namespace imtassist
{


/**
	Domain function that the assistant may call.

	Actions are registered by the application and implemented through its existing
	services and permission checks. The assistant core validates parameters and
	results against the declared schemas and never executes anything that is not
	registered.

	Changing actions are split into two steps:
	- PrepareChange() describes the change without performing it. The core stores
	  the description as a preview bound to the user, the tenant and the parameters.
	- ApplyChange() performs exactly this change after the user has confirmed the
	  preview. It must check identity, tenant, permissions and the state token
	  again, because they can change between preview and confirmation.

	\ingroup imtassist
*/
class IAssistantAction: virtual public istd::IPolymorphic
{
public:
	enum ActionKind
	{
		/**
			Reading action, executed directly.
		*/
		AK_READ,

		/**
			Changing action, executed only after explicit confirmation of a preview.
		*/
		AK_CHANGE
	};

	enum ExecutionStatus
	{
		ES_SUCCEEDED,
		ES_FAILED,
		ES_ACCESS_DENIED,
		/**
			The target was changed since the preview was prepared.
		*/
		ES_STALE
	};

	/**
		Structured UI representation of a result, rendered by a registered component.
	*/
	struct Representation
	{
		QByteArray type;
		int version = 0;
		QJsonObject data;

		bool IsEmpty() const
		{
			return type.isEmpty();
		}
	};

	struct Result
	{
		/**
			Result data, validated against the result schema.
		*/
		QJsonObject data;

		/**
			Optional short text for the user, created by the application.
		*/
		QString text;

		/**
			Optional UI representation of the result.
		*/
		Representation representation;
	};

	/**
		Description of a prepared change, shown to the user before confirmation.
	*/
	struct ChangePreview
	{
		/**
			Short description of the change.
		*/
		QString summary;

		/**
			Display name of the target object.
		*/
		QString targetLabel;

		/**
			ID of the target object.
		*/
		QByteArray targetId;

		/**
			List of changes, each an object with "field", "oldValue" and "newValue".
		*/
		QJsonArray changes;

		/**
			Effects of the change beyond the listed fields.
		*/
		QString impact;

		/**
			Opaque token of the target state at preparation time, for example a revision.
			Passed to ApplyChange() so that a changed target is rejected as stale.
		*/
		QByteArray stateToken;
	};

	/**
		Stable name of the action, for example "orders.setStatus".
	*/
	virtual QByteArray GetActionId() const = 0;

	/**
		Description for the provider.
	*/
	virtual QString GetDescription() const = 0;

	virtual ActionKind GetActionKind() const = 0;

	/**
		Versioned schema of the parameters.
	*/
	virtual const CParameterSchema& GetParameterSchema() const = 0;

	/**
		Versioned schema of the result data.
	*/
	virtual const CParameterSchema& GetResultSchema() const = 0;

	/**
		Check whether the user of the context may call this action with these parameters,
		including object permissions. Called before every execution, preparation and application.
	*/
	virtual bool CheckAccess(const CAssistantRequestContext& context, const QJsonObject& parameters) const = 0;

	/**
		Execute a reading action.
		\param userError    Error text for the user, without confidential details.
	*/
	virtual ExecutionStatus Execute(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) = 0;

	/**
		Describe a change without performing it.
	*/
	virtual ExecutionStatus PrepareChange(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				ChangePreview& preview,
				QString& userError) = 0;

	/**
		Perform a confirmed change.
		\param stateToken   State token of the confirmed preview. The action must
		                    return ES_STALE if the target state no longer matches.
	*/
	virtual ExecutionStatus ApplyChange(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				const QByteArray& stateToken,
				Result& result,
				QString& userError) = 0;
};


} // namespace imtassist


