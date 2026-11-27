// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassistgql/CGqlAssistantActionCompBase.h>


namespace imtassistgql
{


/**
	Reading assistant action of the administration: search the users, groups or roles of the
	organization of the request (attribute \c Kind: \c users, \c groups or \c roles).
	Action IDs: \c administration.searchUsers, \c administration.searchGroups, \c administration.searchRoles.

	The lists are read by the commands \c UsersList, \c GroupsList and \c RolesList of the
	application with the rights of the user, and filtered by the text in the action.
	Only IDs, names and short descriptions are passed to the model, no e-mail addresses.

	\ingroup imtassistgql
*/
class CAdministrationSearchActionComp: public CGqlAssistantActionCompBase
{
public:
	typedef CGqlAssistantActionCompBase BaseClass;

	enum
	{
		DEFAULT_LIMIT = 20,
		MAX_LIMIT = 100,
		MAX_TEXT_LENGTH = 200
	};

	I_BEGIN_COMPONENT(CAdministrationSearchActionComp);
		I_ASSIGN(m_kindAttrPtr, "Kind", "What is searched: users, groups or roles", true, "users");
	I_END_COMPONENT;

	CAdministrationSearchActionComp();

	// reimplemented (imtassist::IAssistantAction)
	virtual QByteArray GetActionId() const override;
	virtual QString GetDescription() const override;
	virtual ActionKind GetActionKind() const override;
	virtual const imtassist::CParameterSchema& GetParameterSchema() const override;
	virtual const imtassist::CParameterSchema& GetResultSchema() const override;
	virtual bool CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const override;
	virtual ExecutionStatus Execute(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) override;

protected:
	enum Kind
	{
		K_USERS,
		K_GROUPS,
		K_ROLES
	};

	Kind GetKind() const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_kindAttrPtr);

	imtassist::CParameterSchema m_parameterSchema;
	imtassist::CParameterSchema m_resultSchema;
};


} // namespace imtassistgql


