// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAdministrationAddUserToGroupActionComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>


namespace imtassistgql
{


namespace
{


const QByteArray s_editGroupsPermission = QByteArrayLiteral("/Administration/Groups/Edit/Change");


QByteArrayList GetDirectIds(const QJsonArray& assignments)
{
	QByteArrayList retVal;
	for (const QJsonValue& value: assignments){
		const QJsonObject assignment = value.toObject();
		const QByteArray id = assignment.value("id").toString().toUtf8();
		if (!id.isEmpty() && assignment.value("direct").toBool() && !retVal.contains(id)){
			retVal.append(id);
		}
	}

	return retVal;
}


QJsonArray CreateDirectAssignments(const QByteArrayList& ids)
{
	QJsonArray retVal;
	for (const QByteArray& id: ids){
		QJsonObject assignment;
		assignment["id"] = QString::fromUtf8(id);
		assignment["direct"] = true;
		retVal.append(assignment);
	}

	return retVal;
}


} // anonymous namespace


CAdministrationAddUserToGroupActionComp::CAdministrationAddUserToGroupActionComp()
	:m_parameterSchema("administration.addUserToGroup.parameters", 1),
	m_resultSchema("administration.addUserToGroup.result", 1)
{
	m_parameterSchema.AddId("userId", true);
	m_parameterSchema.AddId("groupId", true);

	m_resultSchema.AddId("groupId", true);
	m_resultSchema.AddId("userId", true);
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CAdministrationAddUserToGroupActionComp::GetActionId() const
{
	return "administration.addUserToGroup";
}


QString CAdministrationAddUserToGroupActionComp::GetDescription() const
{
	return "Add a user to a user group of the current organization. Takes the IDs found by administration.searchUsers and administration.searchGroups. The user confirms the change before it is applied.";
}


imtassist::IAssistantAction::ActionKind CAdministrationAddUserToGroupActionComp::GetActionKind() const
{
	return AK_CHANGE;
}


const imtassist::CParameterSchema& CAdministrationAddUserToGroupActionComp::GetParameterSchema() const
{
	return m_parameterSchema;
}


const imtassist::CParameterSchema& CAdministrationAddUserToGroupActionComp::GetResultSchema() const
{
	return m_resultSchema;
}


bool CAdministrationAddUserToGroupActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& /*parameters*/) const
{
	return HasPermission(context, s_editGroupsPermission) && !GetProductId(context).isEmpty();
}


imtassist::IAssistantAction::ExecutionStatus CAdministrationAddUserToGroupActionComp::PrepareChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			ChangePreview& preview,
			QString& userError)
{
	const QByteArray userId = parameters.value("userId").toString().toUtf8();
	const QByteArray groupId = parameters.value("groupId").toString().toUtf8();

	Group group;
	QString error;
	if (!ReadGroup(context, groupId, group, error)){
		SendWarningMessage(0, QStringLiteral("Assistant action '%1': the group could not be read: %2").arg(QString::fromUtf8(GetActionId()), error), "AssistantAdministration");
		userError = QStringLiteral("The group was not found.");

		return ES_FAILED;
	}

	QString userName;
	if (!ReadUserName(context, userId, userName, error)){
		SendWarningMessage(0, QStringLiteral("Assistant action '%1': the user could not be read: %2").arg(QString::fromUtf8(GetActionId()), error), "AssistantAdministration");
		userError = QStringLiteral("The user was not found.");

		return ES_FAILED;
	}

	if (group.userIds.contains(userId)){
		userError = QStringLiteral("The user is already a member of the group.");

		return ES_FAILED;
	}

	QJsonObject change;
	change["field"] = QStringLiteral("Members");
	change["oldValue"] = QStringLiteral("%1 users").arg(group.userIds.size());
	change["newValue"] = QStringLiteral("+ %1").arg(userName);

	preview.summary = QStringLiteral("Add the user '%1' to the group '%2'").arg(userName, group.name);
	preview.targetLabel = group.name;
	preview.targetId = group.id;
	preview.changes.append(change);
	preview.impact = QStringLiteral("The user gets the roles and permissions of the group.");
	preview.stateToken = group.CreateStateToken();

	return ES_SUCCEEDED;
}


imtassist::IAssistantAction::ExecutionStatus CAdministrationAddUserToGroupActionComp::ApplyChange(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			const QByteArray& stateToken,
			Result& result,
			QString& userError)
{
	const QByteArray userId = parameters.value("userId").toString().toUtf8();
	const QByteArray groupId = parameters.value("groupId").toString().toUtf8();
	const QByteArray productId = GetProductId(context);

	Group group;
	QString error;
	if (!ReadGroup(context, groupId, group, error)){
		SendWarningMessage(0, QStringLiteral("Assistant action '%1': the group could not be read: %2").arg(QString::fromUtf8(GetActionId()), error), "AssistantAdministration");
		userError = QStringLiteral("The group was not found.");

		return ES_FAILED;
	}

	if (group.CreateStateToken() != stateToken){
		return ES_STALE;
	}

	QByteArrayList userIds = group.userIds;
	userIds.append(userId);

	QJsonObject item;
	item["id"] = QString::fromUtf8(group.id);
	item["productId"] = QString::fromUtf8(productId);
	item["name"] = group.name;
	item["description"] = group.description;
	item["roles"] = CreateDirectAssignments(group.roleIds);
	item["users"] = CreateDirectAssignments(userIds);
	item["parentGroups"] = CreateDirectAssignments(group.parentGroupIds);

	QJsonObject input;
	input["id"] = QString::fromUtf8(group.id);
	input["typeId"] = QString::fromUtf8(*m_groupTypeIdAttrPtr);
	input["productId"] = QString::fromUtf8(productId);
	input["name"] = group.name;
	input["description"] = group.description;
	input["item"] = item;

	QJsonObject variables;
	variables["input"] = input;

	QJsonObject data;
	if (!ExecuteGql(context, "mutation AssistantAddUserToGroup($input: GroupDataInput) { GroupUpdate(input: $input) { id } }", variables, data, error)){
		SendWarningMessage(0, QStringLiteral("Assistant action '%1' failed: %2").arg(QString::fromUtf8(GetActionId()), error), "AssistantAdministration");
		userError = QStringLiteral("The group could not be changed.");

		return ES_FAILED;
	}

	result.data["groupId"] = QString::fromUtf8(group.id);
	result.data["userId"] = QString::fromUtf8(userId);
	result.text = QStringLiteral("The user was added to the group '%1'.").arg(group.name);

	return ES_SUCCEEDED;
}


// protected methods

QByteArray CAdministrationAddUserToGroupActionComp::Group::CreateStateToken() const
{
	QByteArrayList roles = roleIds;
	QByteArrayList users = userIds;
	QByteArrayList parents = parentGroupIds;
	std::sort(roles.begin(), roles.end());
	std::sort(users.begin(), users.end());
	std::sort(parents.begin(), parents.end());

	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(name.toUtf8());
	hash.addData(QByteArrayView("\n"));
	hash.addData(description.toUtf8());
	hash.addData(QByteArrayView("\n"));
	hash.addData(roles.join(','));
	hash.addData(QByteArrayView("\n"));
	hash.addData(users.join(','));
	hash.addData(QByteArrayView("\n"));
	hash.addData(parents.join(','));

	return hash.result().toHex();
}


bool CAdministrationAddUserToGroupActionComp::ReadGroup(
			const imtassist::CAssistantRequestContext& context,
			const QByteArray& groupId,
			Group& group,
			QString& error) const
{
	QJsonObject input;
	input["id"] = QString::fromUtf8(groupId);
	input["productId"] = QString::fromUtf8(GetProductId(context));

	QJsonObject variables;
	variables["input"] = input;

	QJsonObject data;
	if (!ExecuteGql(
				context,
				"query AssistantReadGroup($input: GroupItemInput) { GroupItem(input: $input) { id name description roles { id direct } users { id direct } parentGroups { id direct } } }",
				variables,
				data,
				error)){
		return false;
	}

	group.id = data.value("id").toString().toUtf8();
	if (group.id.isEmpty()){
		group.id = groupId;
	}

	if (group.id != groupId){
		error = QStringLiteral("Another group was returned");

		return false;
	}

	group.name = data.value("name").toString();
	group.description = data.value("description").toString();
	group.roleIds = GetDirectIds(data.value("roles").toArray());
	group.userIds = GetDirectIds(data.value("users").toArray());
	group.parentGroupIds = GetDirectIds(data.value("parentGroups").toArray());

	if (group.name.isEmpty()){
		error = QStringLiteral("The group has no name");

		return false;
	}

	return true;
}


bool CAdministrationAddUserToGroupActionComp::ReadUserName(
			const imtassist::CAssistantRequestContext& context,
			const QByteArray& userId,
			QString& userName,
			QString& error) const
{
	QJsonObject input;
	input["id"] = QString::fromUtf8(userId);
	input["productId"] = QString::fromUtf8(GetProductId(context));

	QJsonObject variables;
	variables["input"] = input;

	QJsonObject data;
	if (!ExecuteGql(context, "query AssistantReadUser($input: UserItemInput) { UserItem(input: $input) { id name } }", variables, data, error)){
		return false;
	}

	userName = data.value("name").toString();
	if (userName.isEmpty()){
		error = QStringLiteral("The user has no name");

		return false;
	}

	return true;
}


} // namespace imtassistgql


