// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtassistgql/CGqlAssistantActionCompBase.h>
#include <imtassistgql/IAssistantApiCatalog.h>


namespace imtassistgql
{


/**
	Changing assistant action \c api.planChanges: a plan of mutations of the public API that the
	user confirms as a whole. The preview lists every step with its command and its input as it
	will be sent. After the confirmation the steps run one after another with the rights of the
	user and stop at the first failure; the outcome says which steps were applied. There is no
	automatic undo.

	The object of a step is the \c id of its input. The preview records the revision of every object
	in the tables of \c Delegates (read through the document action journal) and the confirmation is
	refused as stale if one of them was changed, removed or created after the preview. A step without
	\c id is refused, and so is a step on an existing object if the journal or the delegates are missing:
	later changes of its object could not be detected.

	A string value \c "$stepN.path" in the input of a step is replaced by the value at \c path in the
	result of step N, e.g. the ID of an object created by an earlier step.

	\ingroup imtassistgql
*/
class CAssistantApiPlanActionComp: public CGqlAssistantActionCompBase
{
public:
	typedef CGqlAssistantActionCompBase BaseClass;

	enum
	{
		MAX_STEPS = 20,
		MAX_INPUT_LENGTH = 16000,
		MAX_RESULTS_LENGTH = 16000
	};

	I_BEGIN_COMPONENT(CAssistantApiPlanActionComp);
		I_ASSIGN(m_apiCatalogCompPtr, "ApiCatalog", "Public API of the application with the policy of the product", true, "ApiCatalog");
		I_ASSIGN_MULTI_0(m_delegatesCompPtr, "Delegates", "Document delegates of the tables with revisions: the objects of a plan are checked for changes after the preview", false);
	I_END_COMPONENT;

	CAssistantApiPlanActionComp();

	// reimplemented (imtassist::IAssistantAction)
	virtual QByteArray GetActionId() const override;
	virtual QString GetDescription() const override;
	virtual ActionKind GetActionKind() const override;
	virtual const imtassist::CParameterSchema& GetParameterSchema() const override;
	virtual const imtassist::CParameterSchema& GetResultSchema() const override;
	virtual bool CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const override;
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

	/**
		Replace the references "$stepN.path" in a value by the results of earlier steps.
		\return False if a reference cannot be resolved.
	*/
	static bool ResolveReferences(const QJsonValue& value, const QList<QJsonObject>& stepResults, QJsonValue& resolved, QString& error);

private:
	struct Step
	{
		QString command;
		QJsonValue input;
		QString inputText;
		QString description;
	};

	bool ReadSteps(const QJsonObject& parameters, QList<Step>& steps, QString& error) const;

	/**
		Read the current state of the objects of the steps.
		\return False if a step cannot be checked for later changes of its object.
	*/
	bool ReadTargetStates(const QList<Step>& steps, QByteArray& targetStates, QString& error) const;
	static QByteArray CreateStateToken(const QJsonObject& parameters, const QByteArray& targetStates);

private:
	I_REF(IAssistantApiCatalog, m_apiCatalogCompPtr);
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_delegatesCompPtr);

	imtassist::CParameterSchema m_parameterSchema;
	imtassist::CParameterSchema m_resultSchema;
};


} // namespace imtassistgql


