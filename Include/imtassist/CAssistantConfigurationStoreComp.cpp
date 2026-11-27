// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantConfigurationStoreComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <imtauth/CollectionSelection.h>
#include <imtassist/CAssistantProviderPresets.h>


namespace imtassist
{


namespace
{


/**
	Copy the values of a configuration. Unlike CopyFrom it does not need the same identifiable
	implementation on both sides, e.g. for a configuration created by a controller.
*/
void CopyValues(const IAssistantConfiguration& source, IAssistantConfiguration& target)
{
	target.SetTenantId(source.GetTenantId());
	target.SetEnabled(source.IsEnabled());
	target.SetPresetId(source.GetPresetId());
	target.SetModel(source.GetModel());
	target.SetEndpoint(source.GetEndpoint());
	target.SetRegion(source.GetRegion());
	target.SetProject(source.GetProject());
	target.SetResource(source.GetResource());
	target.SetProtectedApiKey(source.GetProtectedApiKey());
	target.SetMaxOutputTokens(source.GetMaxOutputTokens());
	target.SetAdditionalInstructions(source.GetAdditionalInstructions());
	target.SetUpdatedAt(source.GetUpdatedAt());
}


} // anonymous namespace


// reimplemented (imtassist::IAssistantConfigurationStore)

IAssistantConfigurationUniquePtr CAssistantConfigurationStoreComp::GetConfiguration(const QByteArray& tenantId) const
{
	if (tenantId.isEmpty()){
		return nullptr;
	}

	const int timeToLive = m_cacheTimeToLiveAttrPtr.IsValid() ? *m_cacheTimeToLiveAttrPtr : 0;
	const QDateTime now = QDateTime::currentDateTimeUtc();

	if (timeToLive > 0){
		QMutexLocker lock(&m_cacheMutex);

		QMap<QByteArray, CacheEntry>::const_iterator iter = m_cache.constFind(tenantId);
		if ((iter != m_cache.constEnd()) && (iter.value().readAt.secsTo(now) < timeToLive) && iter.value().configurationPtr.IsValid()){
			return CopyConfiguration(*iter.value().configurationPtr);
		}
	}

	bool isStored = false;
	IAssistantConfigurationUniquePtr retVal = ReadConfiguration(tenantId, isStored);
	if (!retVal.IsValid()){
		return nullptr;
	}

	if (timeToLive > 0){
		IAssistantConfigurationUniquePtr cachedPtr = CopyConfiguration(*retVal);
		if (cachedPtr.IsValid()){
			QMutexLocker lock(&m_cacheMutex);

			if (m_cache.size() >= MAX_CACHE_SIZE){
				m_cache.clear();
			}

			CacheEntry& entry = m_cache[tenantId];
			entry.configurationPtr.SetPtr(cachedPtr.PopPtr());
			entry.readAt = now;
		}
	}

	return retVal;
}


bool CAssistantConfigurationStoreComp::SetConfiguration(
			const IAssistantConfiguration& configuration,
			ApiKeyChange apiKeyChange,
			const QByteArray& apiKey,
			QString& errorMessage)
{
	const QByteArray tenantId = configuration.GetTenantId();
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("The configuration belongs to no organization");

		return false;
	}

	if (!ValidateConfiguration(configuration, errorMessage)){
		return false;
	}

	bool isStored = false;
	IAssistantConfigurationUniquePtr storedPtr = ReadConfiguration(tenantId, isStored);
	if (!storedPtr.IsValid()){
		errorMessage = QStringLiteral("The configuration could not be read");

		return false;
	}

	const QByteArray protectedApiKey = storedPtr->GetProtectedApiKey();
	CopyValues(configuration, *storedPtr);

	storedPtr->SetTenantId(tenantId);
	storedPtr->SetPresetId(configuration.GetPresetId().trimmed());
	storedPtr->SetModel(configuration.GetModel().trimmed());
	storedPtr->SetEndpoint(configuration.GetEndpoint().trimmed());
	storedPtr->SetRegion(configuration.GetRegion().trimmed());
	storedPtr->SetProject(configuration.GetProject().trimmed());
	storedPtr->SetResource(configuration.GetResource().trimmed());
	storedPtr->SetAdditionalInstructions(configuration.GetAdditionalInstructions().trimmed());
	storedPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());

	switch (apiKeyChange){
	case AKC_SET:{
		if (apiKey.isEmpty() || (apiKey.size() > MAX_API_KEY_LENGTH)){
			errorMessage = QStringLiteral("The API key is empty or too long");

			return false;
		}

		if (!m_secretProtectorCompPtr.IsValid()){
			errorMessage = QStringLiteral("The server has no protection for secrets; an API key cannot be stored");

			return false;
		}

		QByteArray newProtectedApiKey;
		if (!m_secretProtectorCompPtr->Protect(apiKey, newProtectedApiKey) || newProtectedApiKey.isEmpty()){
			errorMessage = QStringLiteral("The API key could not be protected");

			return false;
		}

		storedPtr->SetProtectedApiKey(newProtectedApiKey);
		break;
	}
	case AKC_CLEAR:
		storedPtr->SetProtectedApiKey(QByteArray());
		break;
	default:
		storedPtr->SetProtectedApiKey(protectedApiKey);
		break;
	}

	bool isWritten = isStored && m_collectionCompPtr->SetObjectData(tenantId, *storedPtr);
	if (!isStored){
		isWritten = !m_collectionCompPtr->InsertNewObject("AssistantConfiguration", QString::fromUtf8(tenantId), QString(), storedPtr.GetPtr(), tenantId).isEmpty();
	}

	{
		QMutexLocker lock(&m_cacheMutex);

		m_cache.remove(tenantId);
	}

	if (!isWritten){
		errorMessage = QStringLiteral("The configuration could not be stored");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Assistant configuration of the organization '%1' changed").arg(QString::fromUtf8(tenantId)), "AssistantConfigurationStore");

	return true;
}


bool CAssistantConfigurationStoreComp::ValidateConfiguration(const IAssistantConfiguration& configuration, QString& errorMessage) const
{
	const QByteArray presetId = configuration.GetPresetId().trimmed();
	if (presetId.isEmpty()){
		if (configuration.IsEnabled()){
			errorMessage = QStringLiteral("Select a provider to enable the assistant");

			return false;
		}
	}
	else if (!GetAllowedPresetIds().contains(presetId)){
		errorMessage = QStringLiteral("The provider is not available on this server");

		return false;
	}

	const QStringList texts = {
		configuration.GetModel(),
		configuration.GetEndpoint(),
		configuration.GetRegion(),
		configuration.GetProject(),
		configuration.GetResource()
	};
	for (const QString& text: texts){
		if (text.size() > MAX_TEXT_LENGTH){
			errorMessage = QStringLiteral("A value of the configuration is too long");

			return false;
		}
	}

	// Region, project and resource are parts of the host name of the provider: without
	// dots, a value cannot move the request to another host of the same domain.
	const QStringList hostParts = {configuration.GetRegion(), configuration.GetProject(), configuration.GetResource()};
	for (const QString& hostPart: hostParts){
		if (hostPart.contains(QLatin1Char('.'))){
			errorMessage = QStringLiteral("Region, project and resource must not contain dots");

			return false;
		}
	}

	if (configuration.GetAdditionalInstructions().size() > MAX_INSTRUCTIONS_LENGTH){
		errorMessage = QStringLiteral("The additional instructions are too long");

		return false;
	}

	const int maxOutputTokens = configuration.GetMaxOutputTokens();
	if ((maxOutputTokens <= 0) || (maxOutputTokens > IAssistantConfiguration::MAX_OUTPUT_TOKENS_LIMIT)){
		errorMessage = QStringLiteral("The maximum answer length is out of range");

		return false;
	}

	if (!configuration.GetEndpoint().trimmed().isEmpty() && !AreCustomEndpointsAllowed()){
		errorMessage = QStringLiteral("Custom endpoints are not allowed on this server");

		return false;
	}

	if (presetId.isEmpty()){
		return true;
	}

	CAssistantProviderPresets::Preset preset;
	if (!CAssistantProviderPresets::FindPreset(presetId, preset)){
		errorMessage = QStringLiteral("The provider is not available on this server");

		return false;
	}

	CAssistantProviderPresets::Parameters parameters;
	parameters.endpoint = configuration.GetEndpoint().trimmed();
	parameters.model = configuration.GetModel().trimmed().toUtf8();
	parameters.region = configuration.GetRegion().trimmed().toUtf8();
	parameters.project = configuration.GetProject().trimmed().toUtf8();
	parameters.resource = configuration.GetResource().trimmed().toUtf8();

	CHttpAssistantProviderBase::Settings settings;
	QString presetError;
	if (!CAssistantProviderPresets::ApplyPreset(preset, parameters, settings, &presetError)){
		// The texts of the preset name the missing placeholder only, never a value.
		errorMessage = presetError;

		return false;
	}

	return true;
}


bool CAssistantConfigurationStoreComp::ReadApiKey(const IAssistantConfiguration& configuration, QByteArray& apiKey) const
{
	apiKey.clear();

	const QByteArray protectedApiKey = configuration.GetProtectedApiKey();
	if (protectedApiKey.isEmpty()){
		return true;
	}

	if (!m_secretProtectorCompPtr.IsValid()){
		return false;
	}

	return m_secretProtectorCompPtr->Unprotect(protectedApiKey, apiKey);
}


QByteArrayList CAssistantConfigurationStoreComp::GetAllowedPresetIds() const
{
	QByteArrayList retVal;

	const CAssistantProviderPresets::Presets presets = CAssistantProviderPresets::GetPresets();

	if (m_allowedPresetsAttrPtr.GetCount() > 0){
		for (int i = 0; i < m_allowedPresetsAttrPtr.GetCount(); ++i){
			CAssistantProviderPresets::Preset preset;
			if (CAssistantProviderPresets::FindPreset(m_allowedPresetsAttrPtr[i], preset) && !retVal.contains(preset.id)){
				retVal.append(preset.id);
			}
		}

		return retVal;
	}

	// By default only the presets with a fixed public endpoint: a preset without one
	// would make the server send requests to an address chosen by the tenant.
	for (const CAssistantProviderPresets::Preset& preset: presets){
		if (preset.endpointTemplate.startsWith(QStringLiteral("https://"))){
			retVal.append(preset.id);
		}
	}

	return retVal;
}


bool CAssistantConfigurationStoreComp::AreCustomEndpointsAllowed() const
{
	return m_allowCustomEndpointsAttrPtr.IsValid() && *m_allowCustomEndpointsAttrPtr;
}


// reimplemented (imtassist::IAssistantTenantPolicy)

bool CAssistantConfigurationStoreComp::IsAssistantEnabledForTenant(const QByteArray& tenantId) const
{
	IAssistantConfigurationUniquePtr configurationPtr = GetConfiguration(tenantId);

	return configurationPtr.IsValid() && configurationPtr->IsEnabled() && !configurationPtr->GetPresetId().isEmpty();
}


// private methods

IAssistantConfigurationUniquePtr CAssistantConfigurationStoreComp::ReadConfiguration(const QByteArray& tenantId, bool& isStored) const
{
	isStored = false;

	IAssistantConfigurationUniquePtr retVal = m_configurationFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_collectionCompPtr->GetObjectData(tenantId, dataPtr)){
		const IAssistantConfiguration* storedPtr = dynamic_cast<const IAssistantConfiguration*>(dataPtr.GetPtr());
		if ((storedPtr == nullptr) || (storedPtr->GetTenantId() != tenantId)){
			SendErrorMessage(0, QStringLiteral("The assistant configuration of the organization '%1' could not be read").arg(QString::fromUtf8(tenantId)), "AssistantConfigurationStore");

			return nullptr;
		}

		CopyValues(*storedPtr, *retVal);
		isStored = true;

		return retVal;
	}

	// Selected by the indexed column; a failed selection or a stored but unreadable configuration
	// never falls back to a default that could overwrite it.
	bool isRead = true;
	const QByteArrayList ids = imtauth::CollectionSelection::SelectIds(
				*m_collectionCompPtr,
				imtauth::CollectionSelection::ColumnEquals("TenantId", tenantId),
				{QByteArrayLiteral("TenantId")},
				&isRead);
	if (!isRead || ids.contains(tenantId)){
		SendErrorMessage(0, QStringLiteral("The assistant configuration of the organization '%1' could not be read").arg(QString::fromUtf8(tenantId)), "AssistantConfigurationStore");

		return nullptr;
	}

	retVal->SetTenantId(tenantId);

	return retVal;
}


IAssistantConfigurationUniquePtr CAssistantConfigurationStoreComp::CopyConfiguration(const IAssistantConfiguration& configuration) const
{
	IAssistantConfigurationUniquePtr retVal = m_configurationFactoryCompPtr.CreateInstance();
	if (retVal.IsValid()){
		CopyValues(configuration, *retVal);
	}

	return retVal;
}


} // namespace imtassist


