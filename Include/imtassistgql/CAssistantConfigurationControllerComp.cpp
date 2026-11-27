// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantConfigurationControllerComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtservergql/imtservergql.h>
#include <imtassist/CAssistantConfiguration.h>
#include <imtassist/CAssistantProviderPresets.h>


namespace imtassistgql
{


const QByteArray CAssistantConfigurationControllerComp::s_getConfigurationCommandId = QByteArrayLiteral("AssistantGetConfiguration");
const QByteArray CAssistantConfigurationControllerComp::s_setConfigurationCommandId = QByteArrayLiteral("AssistantSetConfiguration");


// protected methods

QByteArray CAssistantConfigurationControllerComp::CheckAccess(const imtgql::CGqlRequest& gqlRequest, const QByteArray& permission, QString& errorMessage) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = (gqlContextPtr != nullptr) ? gqlContextPtr->GetUserInfo() : nullptr;
	if (userInfoPtr == nullptr){
		errorMessage = QStringLiteral("The request has no authenticated user");

		return QByteArray();
	}

	const QByteArray tenantId = gqlContextPtr->GetTenantId();
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("The assistant is configured inside an organization");

		return QByteArray();
	}

	if (!userInfoPtr->IsAdmin() && !imtauth::HasPermission(imtservergql::GetUserPermissions(*gqlContextPtr), permission)){
		errorMessage = QStringLiteral("Invalid permissions for the assistant configuration");
		SendWarningMessage(0, QStringLiteral("Command '%1' refused: the user does not hold '%2'").arg(QString::fromUtf8(gqlRequest.GetCommandId()), QString::fromUtf8(permission)), "AssistantConfigurationController");

		return QByteArray();
	}

	return tenantId;
}


QJsonObject CAssistantConfigurationControllerComp::CreateConfigurationResponse(const imtassist::IAssistantConfiguration& configuration) const
{
	QJsonObject configurationObject;
	configurationObject["enabled"] = configuration.IsEnabled();
	configurationObject["presetId"] = QString::fromUtf8(configuration.GetPresetId());
	configurationObject["model"] = configuration.GetModel();
	configurationObject["endpoint"] = configuration.GetEndpoint();
	configurationObject["region"] = configuration.GetRegion();
	configurationObject["project"] = configuration.GetProject();
	configurationObject["resource"] = configuration.GetResource();
	configurationObject["hasApiKey"] = !configuration.GetProtectedApiKey().isEmpty();
	configurationObject["maxOutputTokens"] = configuration.GetMaxOutputTokens();
	configurationObject["additionalInstructions"] = configuration.GetAdditionalInstructions();
	configurationObject["updatedAt"] = configuration.GetUpdatedAt().isValid() ? configuration.GetUpdatedAt().toString(Qt::ISODate) : QString();
	configurationObject["allowsCustomEndpoint"] = m_configurationStoreCompPtr->AreCustomEndpointsAllowed();

	QJsonArray presetsArray;
	const QByteArrayList allowedPresetIds = m_configurationStoreCompPtr->GetAllowedPresetIds();
	for (const QByteArray& presetId: allowedPresetIds){
		imtassist::CAssistantProviderPresets::Preset preset;
		if (!imtassist::CAssistantProviderPresets::FindPreset(presetId, preset)){
			continue;
		}

		QJsonObject presetObject;
		presetObject["id"] = QString::fromUtf8(preset.id);
		presetObject["name"] = preset.name;
		presetObject["needsEndpoint"] = preset.endpointTemplate.isEmpty();
		presetObject["needsRegion"] = preset.endpointTemplate.contains(QStringLiteral("{region}")) || (preset.protocol == imtassist::CAssistantProviderPresets::P_BEDROCK);
		presetObject["needsProject"] = preset.endpointTemplate.contains(QStringLiteral("{project}"));
		presetObject["needsResource"] = preset.endpointTemplate.contains(QStringLiteral("{resource}"));
		presetObject["needsApiKey"] = !preset.allowsNoCredentials;
		presetObject["credentialHint"] = preset.credentialHint;
		presetsArray.append(presetObject);
	}

	configurationObject["presets"] = presetsArray;

	QJsonObject retVal;
	retVal["configurationJson"] = QString::fromUtf8(QJsonDocument(configurationObject).toJson(QJsonDocument::Compact));

	return retVal;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

QJsonObject CAssistantConfigurationControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	const bool isGet = (commandId == s_getConfigurationCommandId);
	const bool isSet = (commandId == s_setConfigurationCommandId);
	if (!isGet && !isSet){
		errorMessage = QStringLiteral("Unsupported assistant configuration command");

		return QJsonObject();
	}

	const QByteArray permission = isGet ? *m_viewPermissionAttrPtr : *m_editPermissionAttrPtr;
	const QByteArray tenantId = CheckAccess(gqlRequest, permission, errorMessage);
	if (tenantId.isEmpty()){
		return QJsonObject();
	}

	if (isGet){
		imtassist::IAssistantConfigurationUniquePtr configurationPtr = m_configurationStoreCompPtr->GetConfiguration(tenantId);
		if (!configurationPtr.IsValid()){
			errorMessage = QStringLiteral("The assistant configuration could not be read");

			return QJsonObject();
		}

		return CreateConfigurationResponse(*configurationPtr);
	}

	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	if (inputPtr == nullptr){
		errorMessage = QStringLiteral("The configuration is missing");

		return QJsonObject();
	}

	imtassist::CAssistantConfiguration configuration;
	configuration.SetTenantId(tenantId);
	configuration.SetEnabled(inputPtr->GetParamArgumentValue("enabled").toBool());
	configuration.SetPresetId(inputPtr->GetParamArgumentValue("presetId").toString().trimmed().toUtf8());
	configuration.SetModel(inputPtr->GetParamArgumentValue("model").toString());
	configuration.SetEndpoint(inputPtr->GetParamArgumentValue("endpoint").toString());
	configuration.SetRegion(inputPtr->GetParamArgumentValue("region").toString());
	configuration.SetProject(inputPtr->GetParamArgumentValue("project").toString());
	configuration.SetResource(inputPtr->GetParamArgumentValue("resource").toString());
	configuration.SetAdditionalInstructions(inputPtr->GetParamArgumentValue("additionalInstructions").toString());

	bool isNumber = false;
	const int maxOutputTokens = inputPtr->GetParamArgumentValue("maxOutputTokens").toInt(&isNumber);
	configuration.SetMaxOutputTokens(isNumber ? maxOutputTokens : int(imtassist::IAssistantConfiguration::DEFAULT_MAX_OUTPUT_TOKENS));

	imtassist::IAssistantConfigurationStore::ApiKeyChange apiKeyChange = imtassist::IAssistantConfigurationStore::AKC_KEEP;
	const QString apiKeyChangeText = inputPtr->GetParamArgumentValue("apiKeyChange").toString();
	if (apiKeyChangeText == QStringLiteral("set")){
		apiKeyChange = imtassist::IAssistantConfigurationStore::AKC_SET;
	}
	else if (apiKeyChangeText == QStringLiteral("clear")){
		apiKeyChange = imtassist::IAssistantConfigurationStore::AKC_CLEAR;
	}
	else if (!apiKeyChangeText.isEmpty() && (apiKeyChangeText != QStringLiteral("keep"))){
		errorMessage = QStringLiteral("Invalid change of the API key");

		return QJsonObject();
	}

	const QByteArray apiKey = (apiKeyChange == imtassist::IAssistantConfigurationStore::AKC_SET) ? inputPtr->GetParamArgumentValue("apiKey").toString().trimmed().toUtf8() : QByteArray();

	QString storeError;
	if (!m_configurationStoreCompPtr->SetConfiguration(configuration, apiKeyChange, apiKey, storeError)){
		QJsonObject retVal;
		retVal["success"] = false;
		retVal["errorMessage"] = storeError;

		return retVal;
	}

	imtassist::IAssistantConfigurationUniquePtr storedPtr = m_configurationStoreCompPtr->GetConfiguration(tenantId);
	if (!storedPtr.IsValid()){
		errorMessage = QStringLiteral("The assistant configuration could not be read");

		return QJsonObject();
	}

	QJsonObject retVal = CreateConfigurationResponse(*storedPtr);
	retVal["success"] = true;

	return retVal;
}


} // namespace imtassistgql


