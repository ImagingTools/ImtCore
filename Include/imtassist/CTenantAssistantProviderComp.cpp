// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CTenantAssistantProviderComp.h>


// Qt includes
#include <QtCore/QMutexLocker>


namespace imtassist
{


// reimplemented (imtassist::IAssistantProvider)

bool CTenantAssistantProviderComp::IsConfigured() const
{
	// The configuration depends on the tenant; Complete reports RS_NOT_CONFIGURED for a tenant without one.
	return m_configurationStoreCompPtr.IsValid();
}


IAssistantProvider::Reply CTenantAssistantProviderComp::Complete(const Request& request, const CCancellationToken& cancellationToken)
{
	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = GetProvider(request.tenantId);
	if (!providerPtr || !providerPtr->IsConfigured()){
		Reply retVal;
		retVal.status = RS_NOT_CONFIGURED;

		return retVal;
	}

	return providerPtr->Complete(request, cancellationToken);
}


// private methods

bool CTenantAssistantProviderComp::Entry::IsSameConfiguration(const Entry& other) const
{
	return
				(presetId == other.presetId) &&
				(parameters.endpoint == other.parameters.endpoint) &&
				(parameters.model == other.parameters.model) &&
				(parameters.region == other.parameters.region) &&
				(parameters.project == other.parameters.project) &&
				(parameters.resource == other.parameters.resource) &&
				(protectedApiKey == other.protectedApiKey) &&
				(maxOutputTokens == other.maxOutputTokens) &&
				(additionalInstructions == other.additionalInstructions);
}


std::shared_ptr<CHttpAssistantProviderBase> CTenantAssistantProviderComp::GetProvider(const QByteArray& tenantId)
{
	if (tenantId.isEmpty() || !m_configurationStoreCompPtr.IsValid()){
		return nullptr;
	}

	IAssistantConfigurationUniquePtr configurationPtr = m_configurationStoreCompPtr->GetConfiguration(tenantId);
	if (!configurationPtr.IsValid() || !configurationPtr->IsEnabled() || configurationPtr->GetPresetId().isEmpty()){
		return nullptr;
	}

	// The policy of the server may have changed since the configuration was stored.
	QString error;
	if (!m_configurationStoreCompPtr->ValidateConfiguration(*configurationPtr, error)){
		SendWarningMessage(0, QStringLiteral("The assistant configuration of the organization '%1' is not valid on this server: %2").arg(QString::fromUtf8(tenantId), error), "TenantAssistantProvider");

		return nullptr;
	}

	Entry entry;
	entry.presetId = configurationPtr->GetPresetId();
	entry.parameters.endpoint = configurationPtr->GetEndpoint();
	entry.parameters.model = configurationPtr->GetModel().toUtf8();
	entry.parameters.region = configurationPtr->GetRegion().toUtf8();
	entry.parameters.project = configurationPtr->GetProject().toUtf8();
	entry.parameters.resource = configurationPtr->GetResource().toUtf8();
	entry.protectedApiKey = configurationPtr->GetProtectedApiKey();
	entry.maxOutputTokens = configurationPtr->GetMaxOutputTokens();
	entry.additionalInstructions = configurationPtr->GetAdditionalInstructions();

	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Entry>::const_iterator iter = m_entries.constFind(tenantId);
	if ((iter != m_entries.constEnd()) && iter.value().IsSameConfiguration(entry)){
		return iter.value().providerPtr;
	}

	CAssistantProviderPresets::Preset preset;
	CHttpAssistantProviderBase::Settings settings;
	if (!CAssistantProviderPresets::FindPreset(entry.presetId, preset) || !CAssistantProviderPresets::ApplyPreset(preset, entry.parameters, settings)){
		return nullptr;
	}

	QByteArray apiKey;
	if (!m_configurationStoreCompPtr->ReadApiKey(*configurationPtr, apiKey)){
		SendErrorMessage(0, QStringLiteral("The API key of the assistant of the organization '%1' could not be read").arg(QString::fromUtf8(tenantId)), "TenantAssistantProvider");

		return nullptr;
	}

	settings.apiKey = apiKey;

	const bool allowServerCredentials = m_allowServerCredentialsAttrPtr.IsValid() && *m_allowServerCredentialsAttrPtr;
	if (allowServerCredentials){
		settings.apiKeyVariable = m_apiKeyVariableAttrPtr.IsValid() ? *m_apiKeyVariableAttrPtr : QByteArray();
		settings.awsAccessKeyIdVariable = m_awsAccessKeyIdVariableAttrPtr.IsValid() ? *m_awsAccessKeyIdVariableAttrPtr : QByteArray();
		settings.awsSecretAccessKeyVariable = m_awsSecretAccessKeyVariableAttrPtr.IsValid() ? *m_awsSecretAccessKeyVariableAttrPtr : QByteArray();
		settings.awsSessionTokenVariable = m_awsSessionTokenVariableAttrPtr.IsValid() ? *m_awsSessionTokenVariableAttrPtr : QByteArray();
	}
	else{
		settings.apiKeyVariable.clear();
		settings.awsAccessKeyIdVariable.clear();
		settings.awsSecretAccessKeyVariable.clear();
		settings.awsSessionTokenVariable.clear();
	}

	settings.maxOutputTokens = entry.maxOutputTokens;

	QStringList instructions;
	const QString productInstructions = m_additionalInstructionsAttrPtr.IsValid() ? QString(*m_additionalInstructionsAttrPtr).trimmed() : QString();
	if (!productInstructions.isEmpty()){
		instructions.append(productInstructions);
	}

	if (!entry.additionalInstructions.isEmpty()){
		instructions.append(entry.additionalInstructions);
	}

	settings.additionalInstructions = instructions.join(QStringLiteral("\n\n"));

	const QByteArray presetId = entry.presetId;
	std::shared_ptr<CHttpAssistantProviderBase> providerPtr = CAssistantProviderPresets::CreateProvider(preset.protocol);
	providerPtr->SetSettings(settings);
	providerPtr->SetProblemHandler([this, tenantId, presetId](const QString& problem){
		SendWarningMessage(
					0,
					QStringLiteral("Assistant provider '%1' of the organization '%2': %3").arg(QString::fromUtf8(presetId), QString::fromUtf8(tenantId), problem),
					"TenantAssistantProvider");
	});

	if (!providerPtr->IsConfigured()){
		SendWarningMessage(
					0,
					QStringLiteral("Assistant provider '%1' of the organization '%2' has no credentials or model; requests are rejected").arg(QString::fromUtf8(presetId), QString::fromUtf8(tenantId)),
					"TenantAssistantProvider");
	}

	if ((m_entries.size() >= MAX_CACHED_PROVIDERS) && !m_entries.contains(tenantId)){
		m_entries.clear();
	}

	entry.providerPtr = providerPtr;
	m_entries[tenantId] = entry;

	return providerPtr;
}


} // namespace imtassist


