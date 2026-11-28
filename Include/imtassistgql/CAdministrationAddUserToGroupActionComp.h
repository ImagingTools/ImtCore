// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassistgql/CGqlAssistantActionCompBase.h>


namespace imtassistgql
{


/**
	Changing assistant action of the administration: \c administration.addUserToGroup.
	Adds a user to a user group of the organization of the request after the user confirmed
	the preview. The group is read by \c GroupItem and written by \c GroupUpdate of the
	application with the rights of the user; the handlers check permissions, grants and the
	organization. The preview carries a state token of the group: a group changed in the
	meantime is not overwritten.

	\ingroup imtassistgql
*/
class CAdministrationAddUserToGroupActionComp: public CGqlAssistantActionCompBase
{
public:
	typedef CGqlAssistantActionCompBase BaseClass;

	I_BEGIN_COMPONENT(CAdministrationAddUserToGroupActionComp);
		I_ASSIGN(m_groupTypeIdAttrPtr, "GroupTypeId", "Object type ID of the user groups", true, "Group");
	I_END_COMPONENT;

	CAdministrationAddUserToGroupActionComp();

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

protected:
	/**
		Group as read by GroupItem: name, description and the direct assignments.
	*/
	struct Group
	{
		QByteArray id;
		QString name;
		QString description;
		QByteArrayList roleIds;
		QByteArrayList userIds;
		QByteArrayList parentGroupIds;

		QByteArray CreateStateToken() const;
	};

	bool ReadGroup(const imtassist::CAssistantRequestContext& context, const QByteArray& groupId, Group& group, QString& error) const;
	bool ReadUserName(const imtassist::CAssistantRequestContext& context, const QByteArray& userId, QString& userName, QString& error) const;

private:
	I_ATTR(QByteArray, m_groupTypeIdAttrPtr);

	imtassist::CParameterSchema m_parameterSchema;
	imtassist::CParameterSchema m_resultSchema;
};


} // namespace imtassistgql


