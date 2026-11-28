// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantApiPlanActionComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtassistgql/CAssistantApiRequestBuilder.h>


namespace imtassistgql
{


namespace
{


imtassist::CParameterSchema::Field CreateField(
			const QString& name,
			imtassist::CParameterSchema::FieldType type,
			bool isRequired,
			const QString& description,
			int maxLength)
{
	imtassist::CParameterSchema::Field retVal;
	retVal.name = name;
	retVal.type = type;
	retVal.isRequired = isRequired;
	retVal.description = description;
	retVal.maxLength = maxLength;

	return retVal;
}


bool ReadPath(const QJsonValue& root, const QString& path, QJsonValue& value)
{
	value = root;
	const QStringList segments = path.split(QLatin1Char('.'), Qt::SkipEmptyParts);
	for (const QString& segment: segments){
		bool isIndex = false;
		const int index = segment.toInt(&isIndex);
		if (isIndex && value.isArray()){
			value = value.toArray().at(index);
		}
		else if (value.isObject()){
			value = value.toObject().value(segment);
		}
		else{
			return false;
		}

		if (value.isUndefined()){
			return false;
		}
	}

	return true;
}


} // anonymous namespace


CAssistantApiPlanActionComp::CAssistantApiPlanActionComp()
	:m_parameterSchema("api.planChanges.parameters", 1),
	m_resultSchema("api.planChanges.result", 1)
{
	m_parameterSchema.AddField(CreateField("summary", imtassist::CParameterSchema::FT_STRING, true, "What the plan does, in one sentence for the user", 500));

	std::shared_ptr<imtassist::CParameterSchema> stepSchemaPtr = std::make_shared<imtassist::CParameterSchema>("api.planChanges.step", 1);
	stepSchemaPtr->AddField(CreateField("command", imtassist::CParameterSchema::FT_STRING, true, "Name of a mutation from api.listCommands", 200));
	stepSchemaPtr->AddField(CreateField("inputJson", imtassist::CParameterSchema::FT_STRING, false, "Value of the argument as JSON, as described by api.describeCommand. \"$stepN.path\" refers to the result of step N", MAX_INPUT_LENGTH));
	stepSchemaPtr->AddField(CreateField("description", imtassist::CParameterSchema::FT_STRING, true, "What this step changes, for the user", 300));
	m_parameterSchema.AddObjectList("steps", true, stepSchemaPtr, MAX_STEPS);

	m_resultSchema.AddInteger("appliedSteps", true, 0, MAX_STEPS);
	m_resultSchema.AddString("resultsJson", true, MAX_RESULTS_LENGTH + 100);
	m_resultSchema.AddId("documentActionId", false);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CAssistantApiPlanActionComp::GetActionId() const
{
	return "api.planChanges";
}


QString CAssistantApiPlanActionComp::GetDescription() const
{
	return
				"Propose changes as a plan of mutations of the public API (one or more steps). The user sees every step "
				"and confirms or rejects the whole plan; nothing changes before. Read the current data with api.query first; "
				"update commands usually replace the whole object, so send all its values. After the confirmation the steps run "
				"in order and stop at the first failure. Every step needs the \"id\" of its object in the input; the confirmation is "
				"refused if one of these objects was changed after the preview.";
}


imtassist::IAssistantAction::ActionKind CAssistantApiPlanActionComp::GetActionKind() const
{
	return AK_CHANGE;
}


const imtassist::CParameterSchema& CAssistantApiPlanActionComp::GetParameterSchema() const
{
	return m_parameterSchema;
}


const imtassist::CParameterSchema& CAssistantApiPlanActionComp::GetResultSchema() const
{
	return m_resultSchema;
}


bool CAssistantApiPlanActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const
{
	if (!m_apiCatalogCompPtr.IsValid() || (GetGqlContext(context) == nullptr)){
		return false;
	}

	// Every step must be a mutation allowed by the product; the handlers check the permissions of the user.
	const QJsonArray steps = parameters.value("steps").toArray();
	for (const QJsonValue& stepValue: steps){
		const QString commandName = stepValue.toObject().value("command").toString();
		const CSdlApiCatalog::Command* commandPtr = m_apiCatalogCompPtr->GetCatalog().FindCommand(commandName);
		if ((commandPtr == nullptr) || (commandPtr->kind != CSdlApiCatalog::CK_MUTATION) || !m_apiCatalogCompPtr->IsCommandAllowed(commandName)){
			return false;
		}
	}

	return true;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantApiPlanActionComp::PrepareChange(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& parameters,
			ChangePreview& preview,
			QString& userError)
{
	QList<Step> steps;
	if (!ReadSteps(parameters, steps, userError)){
		return ES_FAILED;
	}

	// Each step must form a valid request already; references are resolved at the application.
	for (int i = 0; i < steps.size(); ++i){
		QByteArray query;
		QJsonObject variables;
		QString error;
		if (!CAssistantApiRequestBuilder::BuildRequest(*m_apiCatalogCompPtr, steps[i].command, steps[i].input, QStringList(), query, variables, error)){
			userError = QStringLiteral("Step %1: %2").arg(i + 1).arg(error);

			return ES_FAILED;
		}
	}

	QByteArray targetStates;
	if (!ReadTargetStates(steps, targetStates, userError)){
		return ES_FAILED;
	}

	preview.summary = parameters.value("summary").toString().trimmed();
	preview.targetLabel = QStringLiteral("%1 changes").arg(steps.size());
	preview.targetId = "api.planChanges";

	for (int i = 0; i < steps.size(); ++i){
		// The complete input is shown as it will be sent, so the user sees everything the step really does.
		QJsonObject change;
		change["field"] = QStringLiteral("%1. %2 (%3)").arg(i + 1).arg(steps[i].description, steps[i].command);
		change["newValue"] = steps[i].inputText;
		preview.changes.append(change);
	}

	preview.impact = (GetDocumentActionJournal() != nullptr)
				? QStringLiteral("The steps run one after another and stop at the first failure. Applied steps are not undone automatically; they can be rolled back afterwards if they only changed documents with revisions.")
				: QStringLiteral("The steps run one after another and stop at the first failure. Applied steps are not undone automatically.");
	preview.stateToken = CreateStateToken(parameters, targetStates);

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantApiPlanActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			const QByteArray& stateToken,
			Result& result,
			QString& userError)
{
	QList<Step> steps;
	if (!ReadSteps(parameters, steps, userError)){
		return ES_FAILED;
	}

	// The objects of the steps must be in the state the user saw in the preview.
	QByteArray targetStates;
	if (!ReadTargetStates(steps, targetStates, userError)){
		return ES_FAILED;
	}

	if (CreateStateToken(parameters, targetStates) != stateToken){
		return ES_STALE;
	}

	QByteArray documentActionId;
	if (!BeginDocumentAction(context, parameters.value("summary").toString().trimmed(), documentActionId)){
		userError = QStringLiteral("The changes could not be recorded for a rollback; nothing was changed.");

		return ES_FAILED;
	}

	QList<QJsonObject> stepResults;
	QJsonArray results;
	for (int i = 0; i < steps.size(); ++i){
		QString error;
		QJsonValue input;
		QByteArray query;
		QJsonObject variables;
		QJsonObject data;

		bool isApplied =
					ResolveReferences(steps[i].input, stepResults, input, error) &&
					CAssistantApiRequestBuilder::BuildRequest(*m_apiCatalogCompPtr, steps[i].command, input, QStringList(), query, variables, error);
		if (isApplied && !ExecuteGql(context, query, variables, data, error, documentActionId)){
			isApplied = false;
			error = QStringLiteral("refused or failed");
		}

		if (!isApplied){
			SendWarningMessage(0, QStringLiteral("Assistant plan: step %1 (%2) failed: %3").arg(i + 1).arg(steps[i].command, error), "AssistantApi");

			FinishDocumentAction(context, documentActionId, i, steps.size());

			userError = (i == 0)
						? QStringLiteral("The first change (%1) failed; nothing was changed.").arg(steps[i].command)
						: QStringLiteral("%1 of %2 changes were applied; change %3 (%4) failed; the remaining changes were not applied.")
									.arg(i).arg(steps.size()).arg(i + 1).arg(steps[i].command);
			if ((i > 0) && !documentActionId.isEmpty()){
				userError += QStringLiteral(" The applied changes are the document action %1 (changes.rollback).").arg(QString::fromUtf8(documentActionId));
			}

			return ES_FAILED;
		}

		int omittedItems = 0;
		const QJsonObject filtered = CAssistantApiRequestBuilder::FilterResult(*m_apiCatalogCompPtr, data, omittedItems).toObject();
		stepResults.append(filtered);
		results.append(filtered);
	}

	QString resultsJson = QString::fromUtf8(QJsonDocument(results).toJson(QJsonDocument::Compact));
	if (resultsJson.size() > MAX_RESULTS_LENGTH){
		resultsJson = resultsJson.left(MAX_RESULTS_LENGTH) + QStringLiteral("...");
	}

	FinishDocumentAction(context, documentActionId, steps.size(), steps.size());

	result.data["appliedSteps"] = steps.size();
	result.data["resultsJson"] = resultsJson;
	result.text = QStringLiteral("%1 changes were applied.").arg(steps.size());
	if (!documentActionId.isEmpty()){
		result.data["documentActionId"] = QString::fromUtf8(documentActionId);
		result.text += QStringLiteral(" Document action %1 (changes.rollback).").arg(QString::fromUtf8(documentActionId));
	}

	return ES_SUCCEEDED;
}


bool CAssistantApiPlanActionComp::ResolveReferences(const QJsonValue& value, const QList<QJsonObject>& stepResults, QJsonValue& resolved, QString& error)
{
	static const QRegularExpression referenceExpression(QStringLiteral("^\\$step(\\d+)\\.(.+)$"));

	if (value.isString()){
		const QRegularExpressionMatch match = referenceExpression.match(value.toString());
		if (!match.hasMatch()){
			resolved = value;

			return true;
		}

		const int stepNumber = match.captured(1).toInt();
		if ((stepNumber < 1) || (stepNumber > stepResults.size())){
			error = QStringLiteral("The reference '%1' refers to a step that has not run before").arg(value.toString());

			return false;
		}

		if (!ReadPath(stepResults.at(stepNumber - 1), match.captured(2), resolved)){
			error = QStringLiteral("The reference '%1' is not in the result of step %2").arg(value.toString()).arg(stepNumber);

			return false;
		}

		return true;
	}

	if (value.isObject()){
		QJsonObject object;
		const QJsonObject source = value.toObject();
		for (QJsonObject::const_iterator iter = source.constBegin(); iter != source.constEnd(); ++iter){
			QJsonValue item;
			if (!ResolveReferences(iter.value(), stepResults, item, error)){
				return false;
			}

			object.insert(iter.key(), item);
		}

		resolved = object;

		return true;
	}

	if (value.isArray()){
		QJsonArray array;
		const QJsonArray source = value.toArray();
		for (const QJsonValue& sourceItem: source){
			QJsonValue item;
			if (!ResolveReferences(sourceItem, stepResults, item, error)){
				return false;
			}

			array.append(item);
		}

		resolved = array;

		return true;
	}

	resolved = value;

	return true;
}


// private methods

bool CAssistantApiPlanActionComp::ReadSteps(const QJsonObject& parameters, QList<Step>& steps, QString& error) const
{
	const QJsonArray stepsArray = parameters.value("steps").toArray();
	if (stepsArray.isEmpty()){
		error = QStringLiteral("The plan has no steps.");

		return false;
	}

	for (int i = 0; i < stepsArray.size(); ++i){
		const QJsonObject stepObject = stepsArray.at(i).toObject();

		Step step;
		step.command = stepObject.value("command").toString();
		step.description = stepObject.value("description").toString().trimmed();
		step.inputText = stepObject.value("inputJson").toString().trimmed();

		const CSdlApiCatalog::Command* commandPtr = m_apiCatalogCompPtr->GetCatalog().FindCommand(step.command);
		if ((commandPtr == nullptr) || (commandPtr->kind != CSdlApiCatalog::CK_MUTATION) || !m_apiCatalogCompPtr->IsCommandAllowed(step.command)){
			error = QStringLiteral("Step %1: '%2' is not a mutation available to the assistant.").arg(i + 1).arg(step.command);

			return false;
		}

		if (!step.inputText.isEmpty()){
			QJsonParseError parseError;
			const QJsonDocument document = QJsonDocument::fromJson(step.inputText.toUtf8(), &parseError);
			if (parseError.error != QJsonParseError::NoError){
				error = QStringLiteral("Step %1: inputJson is not valid JSON.").arg(i + 1);

				return false;
			}

			step.input = document.isObject() ? QJsonValue(document.object()) : QJsonValue(document.array());
			step.inputText = QString::fromUtf8(document.toJson(QJsonDocument::Compact));
		}

		steps.append(step);
	}

	return true;
}


bool CAssistantApiPlanActionComp::ReadTargetStates(const QList<Step>& steps, QByteArray& targetStates, QString& error) const
{
	static const QRegularExpression referenceExpression(QStringLiteral("^\\$step(\\d+)\\.(.+)$"));

	targetStates.clear();

	const imtdb::IDocumentActionJournal* journalPtr = GetDocumentActionJournal();
	const int delegateCount = m_delegatesCompPtr.GetCount();

	for (int i = 0; i < steps.size(); ++i){
		const QJsonValue idValue = steps[i].input.toObject().value("id");
		if (!idValue.isString()){
			error = QStringLiteral("Step %1: the input has no \"id\" of the object; changes of the object after the preview could not be detected.").arg(i + 1);

			return false;
		}

		// A new object or an object of an earlier step of the plan has no state to check.
		const QString targetId = idValue.toString();
		if (targetId.isEmpty() || referenceExpression.match(targetId).hasMatch()){
			continue;
		}

		if ((journalPtr == nullptr) || (delegateCount == 0)){
			error = QStringLiteral("Step %1: changes of the object '%2' after the preview could not be detected; the plan cannot be applied.").arg(i + 1).arg(targetId);

			return false;
		}

		const QByteArray documentId = targetId.toUtf8();
		targetStates += documentId;
		targetStates += ':';

		bool isFound = false;
		for (int delegateIndex = 0; delegateIndex < delegateCount; ++delegateIndex){
			const imtdb::ISqlDatabaseObjectDelegate* delegatePtr = m_delegatesCompPtr[delegateIndex];
			if (delegatePtr == nullptr){
				continue;
			}

			imtdb::IDocumentActionJournal::DocumentState state;
			if (!journalPtr->GetDocumentState(delegatePtr->GetTableScheme(), delegatePtr->GetTableName(), documentId, state)){
				error = QStringLiteral("Step %1: the current state of the object '%2' could not be read.").arg(i + 1).arg(targetId);

				return false;
			}

			if (state.exists){
				targetStates += delegatePtr->GetTableScheme();
				targetStates += '.';
				targetStates += delegatePtr->GetTableName();
				targetStates += '/';
				targetStates += QByteArray::number(state.revision);
				targetStates += state.isRemoved ? 'R' : 'A';
				isFound = true;

				break;
			}
		}

		if (!isFound){
			targetStates += QByteArrayLiteral("absent");
		}

		targetStates += '\n';
	}

	return true;
}


QByteArray CAssistantApiPlanActionComp::CreateStateToken(const QJsonObject& parameters, const QByteArray& targetStates)
{
	// The token binds the confirmation to exactly these steps and to the revisions of their objects.
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(QJsonDocument(parameters).toJson(QJsonDocument::Compact));
	hash.addData(QByteArrayLiteral("\n"));
	hash.addData(targetStates);

	return hash.result().toHex();
}


} // namespace imtassistgql


