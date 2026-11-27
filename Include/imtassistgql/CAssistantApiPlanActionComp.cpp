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
				"in order and stop at the first failure.";
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

	preview.summary = parameters.value("summary").toString().trimmed();
	preview.targetLabel = QStringLiteral("%1 changes").arg(steps.size());
	preview.targetId = "api.planChanges";

	for (int i = 0; i < steps.size(); ++i){
		// The input is shown as it will be sent, so the user sees what the step really does.
		QString inputText = steps[i].inputText;
		if (inputText.size() > MAX_PREVIEW_INPUT_LENGTH){
			inputText = inputText.left(MAX_PREVIEW_INPUT_LENGTH) + QStringLiteral("...");
		}

		QJsonObject change;
		change["field"] = QStringLiteral("%1. %2 (%3)").arg(i + 1).arg(steps[i].description, steps[i].command);
		change["newValue"] = inputText;
		preview.changes.append(change);
	}

	preview.impact = QStringLiteral("The steps run one after another and stop at the first failure. Applied steps are not undone automatically.");
	preview.stateToken = CreateStateToken(parameters);

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CAssistantApiPlanActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			const QByteArray& stateToken,
			Result& result,
			QString& userError)
{
	if (CreateStateToken(parameters) != stateToken){
		return ES_STALE;
	}

	QList<Step> steps;
	if (!ReadSteps(parameters, steps, userError)){
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
		if (isApplied && !ExecuteGql(context, query, variables, data, error)){
			isApplied = false;
			error = QStringLiteral("refused or failed");
		}

		if (!isApplied){
			SendWarningMessage(0, QStringLiteral("Assistant plan: step %1 (%2) failed: %3").arg(i + 1).arg(steps[i].command, error), "AssistantApi");

			userError = (i == 0)
						? QStringLiteral("The first change (%1) failed; nothing was changed.").arg(steps[i].command)
						: QStringLiteral("%1 of %2 changes were applied; change %3 (%4) failed; the remaining changes were not applied.")
									.arg(i).arg(steps.size()).arg(i + 1).arg(steps[i].command);

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

	result.data["appliedSteps"] = steps.size();
	result.data["resultsJson"] = resultsJson;
	result.text = QStringLiteral("%1 changes were applied.").arg(steps.size());

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


QByteArray CAssistantApiPlanActionComp::CreateStateToken(const QJsonObject& parameters)
{
	// The plan carries no state of its targets; the token binds the confirmation to exactly these steps.
	return QCryptographicHash::hash(QJsonDocument(parameters).toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex();
}


} // namespace imtassistgql


