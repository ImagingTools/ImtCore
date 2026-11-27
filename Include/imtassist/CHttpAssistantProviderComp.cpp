// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CHttpAssistantProviderComp.h>


namespace imtassist
{


// reimplemented (imtassist::IAssistantProvider)

bool CHttpAssistantProviderComp::IsConfigured() const
{
	return m_providerPtr && m_providerPtr->IsConfigured();
}


IAssistantProvider::Reply CHttpAssistantProviderComp::Complete(const Request& request, const CCancellationToken& cancellationToken)
{
	if (!m_providerPtr){
		Reply retVal;
		retVal.status = RS_PROVIDER_ERROR;

		return retVal;
	}

	return m_providerPtr->Complete(request, cancellationToken);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpAssistantProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QByteArray presetId = m_presetAttrPtr.IsValid() ? *m_presetAttrPtr : QByteArray();

	CAssistantProviderPresets::Preset preset;
	if (!CAssistantProviderPresets::FindPreset(presetId, preset)){
		SendErrorMessage(0, QStringLiteral("Unknown assistant provider preset '%1'; the provider is not configured").arg(QString::fromUtf8(presetId.left(64))), "HttpAssistantProvider");

		return;
	}

	CAssistantProviderPresets::Parameters parameters;
	parameters.endpoint = m_endpointAttrPtr.IsValid() ? *m_endpointAttrPtr : QString();
	parameters.model = m_modelAttrPtr.IsValid() ? *m_modelAttrPtr : QByteArray();
	parameters.region = m_regionAttrPtr.IsValid() ? *m_regionAttrPtr : QByteArray();
	parameters.project = m_projectAttrPtr.IsValid() ? *m_projectAttrPtr : QByteArray();
	parameters.resource = m_resourceAttrPtr.IsValid() ? *m_resourceAttrPtr : QByteArray();

	CHttpAssistantProviderBase::Settings settings;
	QString error;
	if (!CAssistantProviderPresets::ApplyPreset(preset, parameters, settings, &error)){
		SendErrorMessage(0, QStringLiteral("Assistant provider '%1' is not configured: %2").arg(QString::fromUtf8(presetId), error), "HttpAssistantProvider");

		return;
	}

	settings.apiKeyVariable = m_apiKeyVariableAttrPtr.IsValid() ? *m_apiKeyVariableAttrPtr : QByteArray();
	settings.awsAccessKeyIdVariable = m_awsAccessKeyIdVariableAttrPtr.IsValid() ? *m_awsAccessKeyIdVariableAttrPtr : QByteArray();
	settings.awsSecretAccessKeyVariable = m_awsSecretAccessKeyVariableAttrPtr.IsValid() ? *m_awsSecretAccessKeyVariableAttrPtr : QByteArray();
	settings.awsSessionTokenVariable = m_awsSessionTokenVariableAttrPtr.IsValid() ? *m_awsSessionTokenVariableAttrPtr : QByteArray();
	settings.maxOutputTokens = m_maxOutputTokensAttrPtr.IsValid() ? *m_maxOutputTokensAttrPtr : 1024;
	settings.additionalInstructions = m_additionalInstructionsAttrPtr.IsValid() ? *m_additionalInstructionsAttrPtr : QString();

	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = CAssistantProviderPresets::CreateProvider(preset.protocol);
	providerPtr->SetSettings(settings);
	providerPtr->SetProblemHandler([this, presetId](const QString& problem){
		SendWarningMessage(0, QStringLiteral("Assistant provider '%1': %2").arg(QString::fromUtf8(presetId), problem), "HttpAssistantProvider");
	});

	m_providerPtr = providerPtr;

	if (!m_providerPtr->IsConfigured()){
		SendWarningMessage(
					0,
					QStringLiteral("Assistant provider '%1' has no credentials or model; requests are rejected. Expected: %2").arg(QString::fromUtf8(presetId), preset.credentialHint),
					"HttpAssistantProvider");
	}
}


} // namespace imtassist


