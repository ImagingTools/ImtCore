// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassistgql/CGqlAssistantActionCompBase.h>
#include <imtassistgql/IAssistantApiCatalog.h>


namespace imtassistgql
{


/**
	Reading assistant actions on the public API of the application (attribute \c Kind):
	- \c list: \c api.listCommands, the commands available to the assistant, optionally filtered by words;
	- \c describe: \c api.describeCommand, signature, comments, input and result types of a command;
	- \c query: \c api.query, executes a query with the rights of the user and returns its result.

	\ingroup imtassistgql
*/
class CAssistantApiActionComp: public CGqlAssistantActionCompBase
{
public:
	typedef CGqlAssistantActionCompBase BaseClass;

	enum
	{
		MAX_COMMANDS = 200,
		MAX_INPUT_LENGTH = 16000,
		MAX_RESULT_LENGTH = 32000,
		MAX_DESCRIPTION_LENGTH = 20000
	};

	I_BEGIN_COMPONENT(CAssistantApiActionComp);
		I_ASSIGN(m_apiCatalogCompPtr, "ApiCatalog", "Public API of the application with the policy of the product", true, "ApiCatalog");
		I_ASSIGN(m_kindAttrPtr, "Kind", "list, describe or query", true, "list");
	I_END_COMPONENT;

	CAssistantApiActionComp();

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

private:
	enum Kind
	{
		K_LIST,
		K_DESCRIBE,
		K_QUERY
	};

	Kind GetKind() const;

private:
	I_REF(IAssistantApiCatalog, m_apiCatalogCompPtr);
	I_ATTR(QByteArray, m_kindAttrPtr);

	imtassist::CParameterSchema m_listParameterSchema;
	imtassist::CParameterSchema m_listResultSchema;
	imtassist::CParameterSchema m_describeParameterSchema;
	imtassist::CParameterSchema m_describeResultSchema;
	imtassist::CParameterSchema m_queryParameterSchema;
	imtassist::CParameterSchema m_queryResultSchema;
};


} // namespace imtassistgql


