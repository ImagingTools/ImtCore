// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtassist/IAssistantAction.h>
#include <imtassist/IAssistantKnowledgeBase.h>


namespace imtassist
{


/**
	Reading assistant actions on the knowledge base of the product (attribute \c Kind):
	- \c search: \c knowledge.search, the sections matching a text, with snippets;
	- \c read: \c knowledge.read, the text of a section.

	\ingroup imtassist
*/
class CAssistantKnowledgeActionComp:
			public icomp::CComponentBase,
			virtual public IAssistantAction
{
public:
	typedef icomp::CComponentBase BaseClass;

	enum
	{
		DEFAULT_HITS = 8,
		MAX_HITS = 20,
		MAX_QUERY_LENGTH = 500
	};

	I_BEGIN_COMPONENT(CAssistantKnowledgeActionComp);
		I_REGISTER_INTERFACE(IAssistantAction);
		I_ASSIGN(m_knowledgeBaseCompPtr, "KnowledgeBase", "Knowledge base of the product", true, "KnowledgeBase");
		I_ASSIGN(m_kindAttrPtr, "Kind", "search or read", true, "search");
	I_END_COMPONENT;

	CAssistantKnowledgeActionComp();

	// reimplemented (imtassist::IAssistantAction)
	virtual QByteArray GetActionId() const override;
	virtual QString GetDescription() const override;
	virtual ActionKind GetActionKind() const override;
	virtual const CParameterSchema& GetParameterSchema() const override;
	virtual const CParameterSchema& GetResultSchema() const override;
	virtual bool CheckAccess(const CAssistantRequestContext& context, const QJsonObject& parameters) const override;
	virtual ExecutionStatus Execute(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) override;
	virtual ExecutionStatus PrepareChange(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				ChangePreview& preview,
				QString& userError) override;
	virtual ExecutionStatus ApplyChange(
				const CAssistantRequestContext& context,
				const QJsonObject& parameters,
				const QByteArray& stateToken,
				Result& result,
				QString& userError) override;

private:
	bool IsRead() const;

private:
	I_REF(IAssistantKnowledgeBase, m_knowledgeBaseCompPtr);
	I_ATTR(QByteArray, m_kindAttrPtr);

	CParameterSchema m_searchParameterSchema;
	CParameterSchema m_searchResultSchema;
	CParameterSchema m_readParameterSchema;
	CParameterSchema m_readResultSchema;
};


} // namespace imtassist


