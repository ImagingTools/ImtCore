// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CHttpAssistantProviderComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtassist
{


// reimplemented (imtassist::IAssistantProvider)

bool CHttpAssistantProviderComp::IsConfigured() const
{
	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = GetProvider();

	return providerPtr && providerPtr->IsConfigured();
}


IAssistantProvider::Reply CHttpAssistantProviderComp::Complete(const Request& request, const CCancellationToken& cancellationToken)
{
	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = GetProvider();
	if (!providerPtr){
		Reply retVal;
		retVal.status = RS_PROVIDER_ERROR;

		return retVal;
	}

	return providerPtr->Complete(request, cancellationToken);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpAssistantProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Reports an invalid or incomplete configuration at startup.
	GetProvider();
}


// private methods

bool CHttpAssistantProviderComp::Configuration::operator==(const Configuration& other) const
{
	return
				(presetId == other.presetId) &&
				(parameters.endpoint == other.parameters.endpoint) &&
				(parameters.model == other.parameters.model) &&
				(parameters.region == other.parameters.region) &&
				(parameters.project == other.parameters.project) &&
				(parameters.resource == other.parameters.resource) &&
				(apiKey == other.apiKey) &&
				(maxOutputTokens == other.maxOutputTokens) &&
				(additionalInstructions == other.additionalInstructions);
}


CHttpAssistantProviderComp::Configuration CHttpAssistantProviderComp::ReadConfiguration() const
{
	Configuration retVal;

	retVal.presetId = m_presetAttrPtr.IsValid() ? *m_presetAttrPtr : QByteArray();
	if (m_providerParamCompPtr.IsValid()){
		const iprm::IOptionsList* optionsPtr = m_providerParamCompPtr->GetSelectionConstraints();
		int index = m_providerParamCompPtr->GetSelectedOptionIndex();
		if ((optionsPtr != nullptr) && (index >= 0) && (index < optionsPtr->GetOptionsCount())){
			retVal.presetId = optionsPtr->GetOptionId(index);
		}
	}

	QString endpoint = ReadText(m_endpointParamCompPtr.GetPtr());
	retVal.parameters.endpoint = endpoint.isEmpty() ? (m_endpointAttrPtr.IsValid() ? *m_endpointAttrPtr : QString()) : endpoint;

	QString model = ReadText(m_modelParamCompPtr.GetPtr());
	retVal.parameters.model = model.isEmpty() ? (m_modelAttrPtr.IsValid() ? *m_modelAttrPtr : QByteArray()) : model.toUtf8();

	QString region = ReadText(m_regionParamCompPtr.GetPtr());
	retVal.parameters.region = region.isEmpty() ? (m_regionAttrPtr.IsValid() ? *m_regionAttrPtr : QByteArray()) : region.toUtf8();

	QString project = ReadText(m_projectParamCompPtr.GetPtr());
	retVal.parameters.project = project.isEmpty() ? (m_projectAttrPtr.IsValid() ? *m_projectAttrPtr : QByteArray()) : project.toUtf8();

	QString resource = ReadText(m_resourceParamCompPtr.GetPtr());
	retVal.parameters.resource = resource.isEmpty() ? (m_resourceAttrPtr.IsValid() ? *m_resourceAttrPtr : QByteArray()) : resource.toUtf8();

	retVal.apiKey = ReadText(m_apiKeyParamCompPtr.GetPtr()).toUtf8();

	retVal.maxOutputTokens = m_maxOutputTokensAttrPtr.IsValid() ? *m_maxOutputTokensAttrPtr : 1024;
	bool isNumber = false;
	int maxOutputTokens = ReadText(m_maxOutputTokensParamCompPtr.GetPtr()).toInt(&isNumber);
	if (isNumber && (maxOutputTokens > 0)){
		retVal.maxOutputTokens = maxOutputTokens;
	}

	QString instructions = ReadText(m_additionalInstructionsParamCompPtr.GetPtr());
	retVal.additionalInstructions = instructions.isEmpty() ? (m_additionalInstructionsAttrPtr.IsValid() ? *m_additionalInstructionsAttrPtr : QString()) : instructions;

	return retVal;
}


std::shared_ptr<CHttpAssistantProviderBase> CHttpAssistantProviderComp::GetProvider() const
{
	Configuration configuration = ReadConfiguration();

	QMutexLocker lock(&m_mutex);

	if (m_hasConfiguration && (configuration == m_configuration)){
		return m_providerPtr;
	}

	m_hasConfiguration = true;
	m_configuration = configuration;
	m_providerPtr.reset();

	CAssistantProviderPresets::Preset preset;
	if (!CAssistantProviderPresets::FindPreset(configuration.presetId, preset)){
		SendErrorMessage(0, QStringLiteral("Unknown assistant provider '%1'; the provider is not configured").arg(QString::fromUtf8(configuration.presetId.left(64))), "HttpAssistantProvider");

		return m_providerPtr;
	}

	CHttpAssistantProviderBase::Settings settings;
	QString error;
	if (!CAssistantProviderPresets::ApplyPreset(preset, configuration.parameters, settings, &error)){
		SendErrorMessage(0, QStringLiteral("Assistant provider '%1' is not configured: %2").arg(QString::fromUtf8(configuration.presetId), error), "HttpAssistantProvider");

		return m_providerPtr;
	}

	settings.apiKey = configuration.apiKey;
	settings.apiKeyVariable = m_apiKeyVariableAttrPtr.IsValid() ? *m_apiKeyVariableAttrPtr : QByteArray();
	settings.awsAccessKeyIdVariable = m_awsAccessKeyIdVariableAttrPtr.IsValid() ? *m_awsAccessKeyIdVariableAttrPtr : QByteArray();
	settings.awsSecretAccessKeyVariable = m_awsSecretAccessKeyVariableAttrPtr.IsValid() ? *m_awsSecretAccessKeyVariableAttrPtr : QByteArray();
	settings.awsSessionTokenVariable = m_awsSessionTokenVariableAttrPtr.IsValid() ? *m_awsSessionTokenVariableAttrPtr : QByteArray();
	settings.maxOutputTokens = configuration.maxOutputTokens;
	settings.additionalInstructions = configuration.additionalInstructions;

	QByteArray presetId = configuration.presetId;

	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = CAssistantProviderPresets::CreateProvider(preset.protocol);
	providerPtr->SetSettings(settings);
	providerPtr->SetProblemHandler([this, presetId](const QString& problem){
		SendWarningMessage(0, QStringLiteral("Assistant provider '%1': %2").arg(QString::fromUtf8(presetId), problem), "HttpAssistantProvider");
	});

	m_providerPtr = providerPtr;

	if (providerPtr->IsConfigured()){
		SendInfoMessage(0, QStringLiteral("Assistant provider '%1' configured").arg(QString::fromUtf8(presetId)), "HttpAssistantProvider");
	}
	else{
		SendWarningMessage(
					0,
					QStringLiteral("Assistant provider '%1' has no credentials or model; requests are rejected. Expected: %2").arg(QString::fromUtf8(presetId), preset.credentialHint),
					"HttpAssistantProvider");
	}

	return m_providerPtr;
}


QString CHttpAssistantProviderComp::ReadText(const iprm::ITextParam* paramPtr)
{
	return (paramPtr != nullptr) ? paramPtr->GetText().trimmed() : QString();
}


} // namespace imtassist


