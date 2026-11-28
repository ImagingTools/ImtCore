// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CFakeAssistantProviderComp.h>


// Qt includes
#include <QtCore/QJsonDocument>


namespace imtassist
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CFakeAssistantProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetConfigured(!m_isConfiguredAttrPtr.IsValid() || *m_isConfiguredAttrPtr);

	if (m_defaultReplyAttrPtr.IsValid() && !(*m_defaultReplyAttrPtr).isEmpty()){
		SetDefaultReply(CreateTextReply(*m_defaultReplyAttrPtr));
	}

	if (m_toolResultReplyAttrPtr.IsValid() && !(*m_toolResultReplyAttrPtr).isEmpty()){
		SetToolResultReply(CreateTextReply(*m_toolResultReplyAttrPtr));
	}

	int count = m_ruleKeywordsAttrPtr.GetCount();
	for (int i = 0; i < count; ++i){
		QString keyword = m_ruleKeywordsAttrPtr[i];
		if (keyword.isEmpty()){
			continue;
		}

		QByteArray actionId = (i < m_ruleActionIdsAttrPtr.GetCount()) ? m_ruleActionIdsAttrPtr[i] : QByteArray();
		QString question = (i < m_ruleQuestionsAttrPtr.GetCount()) ? m_ruleQuestionsAttrPtr[i] : QString();

		if (actionId.isEmpty()){
			QString replyText = (i < m_ruleRepliesAttrPtr.GetCount()) ? m_ruleRepliesAttrPtr[i] : QString();
			if (replyText.isEmpty()){
				SendErrorMessage(0, QStringLiteral("Rule %1 of the fake provider has neither an action nor a reply").arg(i), "FakeAssistantProvider");

				continue;
			}

			AddRule(keyword, CreateTextReply(replyText));

			continue;
		}

		QJsonObject parameters;
		QString parametersText = (i < m_ruleParametersAttrPtr.GetCount()) ? m_ruleParametersAttrPtr[i] : QString();
		if (!parametersText.isEmpty()){
			QJsonParseError parseError;
			QJsonDocument document = QJsonDocument::fromJson(parametersText.toUtf8(), &parseError);
			if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
				SendErrorMessage(0, QStringLiteral("Parameters of rule %1 of the fake provider are not a JSON object").arg(i), "FakeAssistantProvider");

				continue;
			}

			parameters = document.object();
		}

		AddRule(keyword, CreateActionCallReply(actionId, parameters), question);
	}

	SendWarningMessage(0, QStringLiteral("The fake assistant provider is in use; it is meant for development and demos only."), "FakeAssistantProvider");
}


} // namespace imtassist


