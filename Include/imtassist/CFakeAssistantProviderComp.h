// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/CFakeAssistantProvider.h>


namespace imtassist
{


/**
	Component of the deterministic fake provider, for development, demos and tests.
	It never contacts a network and must not be used as a production provider.

	Rules are given as parallel lists: the n-th keyword is answered either with a call of
	the n-th action ID with the n-th parameters (a JSON object), or, if the action ID is
	empty, with the n-th reply text. Parameters may use \c $context.<kind>.<field>
	placeholders; if the context is missing, the n-th question is asked back.

	\ingroup imtassist
*/
class CFakeAssistantProviderComp:
			public ilog::CLoggerComponentBase,
			public CFakeAssistantProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFakeAssistantProviderComp);
		I_REGISTER_INTERFACE(IAssistantProvider);
		I_ASSIGN(m_isConfiguredAttrPtr, "Configured", "Report the provider as configured", true, true);
		I_ASSIGN(m_defaultReplyAttrPtr, "DefaultReply", "Reply to a message that matches no rule", true, "Sorry, I cannot help with that.");
		I_ASSIGN(m_toolResultReplyAttrPtr, "ToolResultReply", "Reply after a reading action was executed; empty to disable", false, "");
		I_ASSIGN_MULTI_0(m_ruleKeywordsAttrPtr, "RuleKeywords", "Keywords of the rules (case-insensitive)", false);
		I_ASSIGN_MULTI_0(m_ruleActionIdsAttrPtr, "RuleActionIds", "Action IDs of the rules; empty for a text reply", false);
		I_ASSIGN_MULTI_0(m_ruleParametersAttrPtr, "RuleParameters", "Action parameters of the rules as JSON objects", false);
		I_ASSIGN_MULTI_0(m_ruleRepliesAttrPtr, "RuleReplies", "Reply texts of the rules without action", false);
		I_ASSIGN_MULTI_0(m_ruleQuestionsAttrPtr, "RuleQuestions", "Questions asked back if a context placeholder of the rule cannot be resolved", false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(bool, m_isConfiguredAttrPtr);
	I_TEXTATTR(m_defaultReplyAttrPtr);
	I_TEXTATTR(m_toolResultReplyAttrPtr);
	I_MULTITEXTATTR(m_ruleKeywordsAttrPtr);
	I_MULTIATTR(QByteArray, m_ruleActionIdsAttrPtr);
	I_MULTITEXTATTR(m_ruleParametersAttrPtr);
	I_MULTITEXTATTR(m_ruleRepliesAttrPtr);
	I_MULTITEXTATTR(m_ruleQuestionsAttrPtr);
};


} // namespace imtassist


