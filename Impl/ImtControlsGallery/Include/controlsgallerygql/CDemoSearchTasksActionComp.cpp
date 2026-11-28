#include <controlsgallerygql/CDemoSearchTasksActionComp.h>


// ImtCore includes
#include <imtassist/CStandardRepresentations.h>


namespace controlsgallerygql
{


CDemoSearchTasksActionComp::CDemoSearchTasksActionComp()
	:m_parameterSchema("tasks.search.parameters", 1),
	m_resultSchema("tasks.search.result", 1)
{
	QStringList statuses = QStringList() << "open" << "inProgress" << "done";

	m_parameterSchema.AddEnum("status", false, statuses);
	m_parameterSchema.AddString("text", false, 200);

	std::shared_ptr<imtassist::CParameterSchema> itemSchemaPtr = std::make_shared<imtassist::CParameterSchema>("tasks.search.item", 1);
	itemSchemaPtr->AddId("id", true);
	itemSchemaPtr->AddString("title", true, 500);
	itemSchemaPtr->AddEnum("status", true, statuses);

	m_resultSchema.AddObjectList("items", true, itemSchemaPtr, 200);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CDemoSearchTasksActionComp::GetActionId() const
{
	return "tasks.search";
}


QString CDemoSearchTasksActionComp::GetDescription() const
{
	return "Search the tasks of the current organization by status (open, inProgress, done) and text.";
}


imtassist::IAssistantAction::ActionKind CDemoSearchTasksActionComp::GetActionKind() const
{
	return AK_READ;
}


const imtassist::CParameterSchema& CDemoSearchTasksActionComp::GetParameterSchema() const
{
	return m_parameterSchema;
}


const imtassist::CParameterSchema& CDemoSearchTasksActionComp::GetResultSchema() const
{
	return m_resultSchema;
}


bool CDemoSearchTasksActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& /*parameters*/) const
{
	return context.IsValid() && m_taskStoreCompPtr.IsValid();
}


imtassist::IAssistantAction::ExecutionStatus CDemoSearchTasksActionComp::Execute(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			Result& result,
			QString& userError)
{
	if (!m_taskStoreCompPtr.IsValid()){
		userError = QStringLiteral("The tasks are not available.");

		return ES_FAILED;
	}

	QString status = parameters.value("status").toString();
	QString text = parameters.value("text").toString();

	QJsonArray items;
	QJsonArray listItems;
	for (const IDemoTaskStore::Task& task: m_taskStoreCompPtr->GetTasks(context.GetTenantId())){
		if (!status.isEmpty() && (task.status != status)){
			continue;
		}

		if (!text.isEmpty() && !task.title.contains(text, Qt::CaseInsensitive)){
			continue;
		}

		QJsonObject item;
		item["id"] = QString::fromUtf8(task.id);
		item["title"] = task.title;
		item["status"] = task.status;
		items.append(item);

		listItems.append(imtassist::CStandardRepresentations::CreateSelectableListItem(task.id, task.title, task.status));
	}

	result.data["items"] = items;
	result.text = QString("%1 tasks found.").arg(items.size());
	result.representation.type = imtassist::CStandardRepresentations::s_selectableListType;
	result.representation.version = imtassist::CStandardRepresentations::SELECTABLE_LIST_VERSION;
	result.representation.data = imtassist::CStandardRepresentations::CreateSelectableListData(listItems, QStringLiteral("Tasks"));

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CDemoSearchTasksActionComp::PrepareChange(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			ChangePreview& /*preview*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


imtassist::IAssistantAction::ExecutionStatus CDemoSearchTasksActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& /*context*/,
			const QJsonObject& /*parameters*/,
			const QByteArray& /*stateToken*/,
			Result& /*result*/,
			QString& /*userError*/)
{
	return ES_FAILED;
}


} // namespace controlsgallerygql


