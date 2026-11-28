#include <controlsgallerygql/CDemoSetTaskStatusActionComp.h>


namespace controlsgallerygql
{


CDemoSetTaskStatusActionComp::CDemoSetTaskStatusActionComp()
	:m_parameterSchema("tasks.setStatus.parameters", 1),
	m_resultSchema("tasks.setStatus.result", 1)
{
	QStringList statuses = QStringList() << "open" << "inProgress" << "done";

	m_parameterSchema.AddId("taskId", true);
	m_parameterSchema.AddEnum("status", true, statuses);

	m_resultSchema.AddId("taskId", true);
	m_resultSchema.AddEnum("status", true, statuses);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CDemoSetTaskStatusActionComp::GetActionId() const
{
	return "tasks.setStatus";
}


QString CDemoSetTaskStatusActionComp::GetDescription() const
{
	return "Change the status of one task of the current organization. Requires the confirmation of the user.";
}


imtassist::IAssistantAction::ActionKind CDemoSetTaskStatusActionComp::GetActionKind() const
{
	return AK_CHANGE;
}


const imtassist::CParameterSchema& CDemoSetTaskStatusActionComp::GetParameterSchema() const
{
	return m_parameterSchema;
}


const imtassist::CParameterSchema& CDemoSetTaskStatusActionComp::GetResultSchema() const
{
	return m_resultSchema;
}


bool CDemoSetTaskStatusActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const
{
	if (!context.IsValid() || !m_taskStoreCompPtr.IsValid()){
		return false;
	}

	if (m_readOnlyUserIdsAttrPtr.FindValue(context.GetUserId()) >= 0){
		return false;
	}

	// The task must exist in the organization of the request (fail closed).
	IDemoTaskStore::Task task;

	return m_taskStoreCompPtr->GetTask(context.GetTenantId(), parameters.value("taskId").toString().toUtf8(), task);
}


imtassist::IAssistantAction::ExecutionStatus CDemoSetTaskStatusActionComp::Execute(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			Result& /*result*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


imtassist::IAssistantAction::ExecutionStatus CDemoSetTaskStatusActionComp::PrepareChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			ChangePreview& preview,
			QString& userError)
{
	if (!CheckAccess(context, parameters)){
		return ES_ACCESS_DENIED;
	}

	IDemoTaskStore::Task task;
	if (!m_taskStoreCompPtr->GetTask(context.GetTenantId(), parameters.value("taskId").toString().toUtf8(), task)){
		userError = QStringLiteral("The task does not exist.");

		return ES_FAILED;
	}

	QString status = parameters.value("status").toString();

	QJsonObject change;
	change["field"] = QStringLiteral("status");
	change["oldValue"] = task.status;
	change["newValue"] = status;

	preview.summary = QString("Change the status of '%1' to '%2'.").arg(task.title, status);
	preview.targetId = task.id;
	preview.targetLabel = task.title;
	preview.changes.append(change);
	preview.impact = (status == "done") ? QStringLiteral("The task no longer appears among the open tasks.") : QStringLiteral("The task appears in another list.");
	preview.stateToken = QByteArray::number(task.revision);

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CDemoSetTaskStatusActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			const QByteArray& stateToken,
			Result& result,
			QString& userError)
{
	if (!CheckAccess(context, parameters)){
		return ES_ACCESS_DENIED;
	}

	bool isNumber = false;
	int expectedRevision = stateToken.toInt(&isNumber);
	if (!isNumber){
		return ES_STALE;
	}

	IDemoTaskStore::Task changedTask;
	IDemoTaskStore::ChangeResult changeResult = m_taskStoreCompPtr->SetTaskStatus(
				context.GetTenantId(),
				parameters.value("taskId").toString().toUtf8(),
				parameters.value("status").toString(),
				expectedRevision,
				changedTask);

	if (changeResult == IDemoTaskStore::CR_STALE){
		return ES_STALE;
	}

	if (changeResult != IDemoTaskStore::CR_CHANGED){
		userError = QStringLiteral("The task does not exist.");

		return ES_FAILED;
	}

	result.data["taskId"] = QString::fromUtf8(changedTask.id);
	result.data["status"] = changedTask.status;
	result.text = QString("'%1' is now '%2'.").arg(changedTask.title, changedTask.status);

	return ES_SUCCEEDED;
}


} // namespace controlsgallerygql


