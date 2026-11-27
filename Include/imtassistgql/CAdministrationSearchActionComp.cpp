// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAdministrationSearchActionComp.h>


// ImtCore includes
#include <imtassist/CStandardRepresentations.h>


namespace imtassistgql
{


namespace
{


struct KindInfo
{
	QByteArray actionId;
	QString description;
	QByteArray permission;
	QByteArray query;
	QByteArray nameField;
	QByteArray descriptionField;
	QString title;
	QString resultText;
};


KindInfo GetKindInfo(int kind)
{
	KindInfo retVal;

	switch (kind){
	case 1:
		retVal.actionId = "administration.searchGroups";
		retVal.description = "Search the user groups of the current organization by a part of their name. Returns IDs and names.";
		retVal.permission = "/Administration/Groups/View";
		retVal.query = "query AssistantSearchGroups($input: GroupsListInput) { GroupsList(input: $input) { items { id name description } } }";
		retVal.nameField = "name";
		retVal.descriptionField = "description";
		retVal.title = QStringLiteral("Groups");
		retVal.resultText = QStringLiteral("%1 groups found.");
		break;
	case 2:
		retVal.actionId = "administration.searchRoles";
		retVal.description = "Search the roles of the current organization by a part of their name. Returns IDs and names.";
		retVal.permission = "/Administration/Roles/View";
		retVal.query = "query AssistantSearchRoles($input: RolesListInput) { RolesList(input: $input) { items { id roleName roleDescription } } }";
		retVal.nameField = "roleName";
		retVal.descriptionField = "roleDescription";
		retVal.title = QStringLiteral("Roles");
		retVal.resultText = QStringLiteral("%1 roles found.");
		break;
	default:
		retVal.actionId = "administration.searchUsers";
		retVal.description = "Search the users of the current organization by a part of their name or login. Returns IDs, names and logins.";
		retVal.permission = "/Administration/Users/View";
		retVal.query = "query AssistantSearchUsers($input: UsersListInput) { UsersList(input: $input) { items { id name userId } } }";
		retVal.nameField = "name";
		retVal.descriptionField = "userId";
		retVal.title = QStringLiteral("Users");
		retVal.resultText = QStringLiteral("%1 users found.");
		break;
	}

	return retVal;
}


} // anonymous namespace


CAdministrationSearchActionComp::CAdministrationSearchActionComp()
	:m_parameterSchema("administration.search.parameters", 1),
	m_resultSchema("administration.search.result", 1)
{
	m_parameterSchema.AddString("text", false, MAX_TEXT_LENGTH);
	m_parameterSchema.AddInteger("limit", false, 1, MAX_LIMIT);

	std::shared_ptr<imtassist::CParameterSchema> itemSchemaPtr = std::make_shared<imtassist::CParameterSchema>("administration.search.item", 1);
	itemSchemaPtr->AddId("id", true);
	itemSchemaPtr->AddString("name", true, 500);
	itemSchemaPtr->AddString("description", false, 1000);

	m_resultSchema.AddObjectList("items", true, itemSchemaPtr, MAX_LIMIT);
	m_resultSchema.AddBoolean("hasMore", true);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CAdministrationSearchActionComp::GetActionId() const
{
	return GetKindInfo(GetKind()).actionId;
}


QString CAdministrationSearchActionComp::GetDescription() const
{
	return GetKindInfo(GetKind()).description;
}


imtassist::IAssistantAction::ActionKind CAdministrationSearchActionComp::GetActionKind() const
{
	return AK_READ;
}


const imtassist::CParameterSchema& CAdministrationSearchActionComp::GetParameterSchema() const
{
	return m_parameterSchema;
}


const imtassist::CParameterSchema& CAdministrationSearchActionComp::GetResultSchema() const
{
	return m_resultSchema;
}


bool CAdministrationSearchActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& /*parameters*/) const
{
	return HasPermission(context, GetKindInfo(GetKind()).permission);
}


imtassist::IAssistantAction::ExecutionStatus CAdministrationSearchActionComp::Execute(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			Result& result,
			QString& userError)
{
	const KindInfo info = GetKindInfo(GetKind());

	QJsonObject viewParams;
	viewParams["offset"] = 0;
	viewParams["count"] = int(MAX_RESPONSE_ITEMS);

	QJsonObject input;
	input["viewParams"] = viewParams;

	const QByteArray productId = GetProductId(context);
	if (!productId.isEmpty()){
		input["productId"] = QString::fromUtf8(productId);
	}

	QJsonObject variables;
	variables["input"] = input;

	QJsonObject data;
	QString error;
	if (!ExecuteGql(context, info.query, variables, data, error)){
		SendWarningMessage(0, QStringLiteral("Assistant action '%1' failed: %2").arg(QString::fromUtf8(info.actionId), error), "AssistantAdministration");
		userError = QStringLiteral("The list could not be read.");

		return ES_FAILED;
	}

	const QString text = parameters.value("text").toString().trimmed();
	const int limit = parameters.contains("limit") ? parameters.value("limit").toInt() : int(DEFAULT_LIMIT);

	QJsonArray items;
	QJsonArray listItems;
	bool hasMore = false;

	const QJsonArray sourceItems = data.value("items").toArray();
	for (const QJsonValue& sourceValue: sourceItems){
		const QJsonObject sourceItem = sourceValue.toObject();
		const QString id = sourceItem.value("id").toString();
		const QString name = sourceItem.value(QString::fromUtf8(info.nameField)).toString();
		const QString description = sourceItem.value(QString::fromUtf8(info.descriptionField)).toString().left(1000);
		if (id.isEmpty()){
			continue;
		}

		if (!text.isEmpty() && !name.contains(text, Qt::CaseInsensitive) && !description.contains(text, Qt::CaseInsensitive)){
			continue;
		}

		if (items.size() >= limit){
			hasMore = true;

			break;
		}

		QJsonObject item;
		item["id"] = id;
		item["name"] = name.left(500);
		if (!description.isEmpty()){
			item["description"] = description;
		}

		items.append(item);
		listItems.append(imtassist::CStandardRepresentations::CreateSelectableListItem(id.toUtf8(), name, description));
	}

	result.data["items"] = items;
	result.data["hasMore"] = hasMore;
	result.text = info.resultText.arg(items.size());
	result.representation.type = imtassist::CStandardRepresentations::s_selectableListType;
	result.representation.version = imtassist::CStandardRepresentations::SELECTABLE_LIST_VERSION;
	result.representation.data = imtassist::CStandardRepresentations::CreateSelectableListData(listItems, info.title);

	return ES_SUCCEEDED;
}


// protected methods

CAdministrationSearchActionComp::Kind CAdministrationSearchActionComp::GetKind() const
{
	const QByteArray kind = m_kindAttrPtr.IsValid() ? *m_kindAttrPtr : QByteArray();
	if (kind == "groups"){
		return K_GROUPS;
	}

	if (kind == "roles"){
		return K_ROLES;
	}

	return K_USERS;
}


// reimplemented (icomp::CComponentBase)

void CAdministrationSearchActionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const QByteArray kind = m_kindAttrPtr.IsValid() ? *m_kindAttrPtr : QByteArray();
	if ((kind != "users") && (kind != "groups") && (kind != "roles")){
		SendErrorMessage(0, QStringLiteral("Unknown kind of the administration search; users are searched"), "AssistantAdministration");
	}
}


} // namespace imtassistgql


