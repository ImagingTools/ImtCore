// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantProviderPresets.h>


// ImtCore includes
#include <imtassist/CAnthropicMessagesProvider.h>
#include <imtassist/CBedrockConverseProvider.h>
#include <imtassist/CGeminiProvider.h>
#include <imtassist/COpenAiCompatibleProvider.h>


namespace imtassist
{


namespace
{


CAssistantProviderPresets::Preset CreateOpenAiCompatiblePreset(
			const QByteArray& id,
			const QString& name,
			const QString& endpointTemplate,
			const QString& credentialHint)
{
	CAssistantProviderPresets::Preset retVal;
	retVal.id = id;
	retVal.name = name;
	retVal.protocol = CAssistantProviderPresets::P_OPENAI_COMPATIBLE;
	retVal.endpointTemplate = endpointTemplate;
	retVal.credentialHint = credentialHint;

	return retVal;
}


bool IsValidPlaceholderValue(const QByteArray& value)
{
	if (value.isEmpty() || (value.size() > 128)){
		return false;
	}

	for (char character: value){
		bool isAllowed =
					((character >= 'a') && (character <= 'z')) ||
					((character >= 'A') && (character <= 'Z')) ||
					((character >= '0') && (character <= '9')) ||
					(character == '-') || (character == '_') || (character == '.');
		if (!isAllowed){
			return false;
		}
	}

	return true;
}


} // namespace


CAssistantProviderPresets::Presets CAssistantProviderPresets::GetPresets()
{
	Presets retVal;

	// OpenAI-compatible Chat Completions API
	Preset openAi = CreateOpenAiCompatiblePreset("openai", "OpenAI", "https://api.openai.com/v1/chat/completions", "OpenAI API key");
	openAi.sendParallelToolCallsFlag = true;
	openAi.maxTokensField = "max_completion_tokens";
	retVal.append(openAi);

	Preset azure = CreateOpenAiCompatiblePreset("azure-openai", "Azure OpenAI", "https://{resource}.openai.azure.com/openai/v1/chat/completions", "API key of the Azure OpenAI resource; the model is the deployment name");
	azure.authHeaderName = "api-key";
	azure.authValuePrefix = "";
	azure.sendParallelToolCallsFlag = true;
	azure.maxTokensField = "max_completion_tokens";
	retVal.append(azure);

	Preset mistral = CreateOpenAiCompatiblePreset("mistral", "Mistral AI", "https://api.mistral.ai/v1/chat/completions", "Mistral API key");
	mistral.sendParallelToolCallsFlag = true;
	retVal.append(mistral);

	retVal.append(CreateOpenAiCompatiblePreset("cohere", "Cohere (compatibility API)", "https://api.cohere.ai/compatibility/v1/chat/completions", "Cohere API key"));
	retVal.append(CreateOpenAiCompatiblePreset("ionos", "IONOS AI Model Hub", "https://openai.inference.de-txl.ionos.com/v1/chat/completions", "IONOS token"));
	retVal.append(CreateOpenAiCompatiblePreset("stackit", "STACKIT AI Model Serving", "https://api.openai-compat.model-serving.eu01.onstackit.cloud/v1/chat/completions", "STACKIT AI Model Serving auth token"));
	retVal.append(CreateOpenAiCompatiblePreset("aleph-alpha", "Aleph Alpha PhariaAI", "", "Token of the PhariaAI installation; set the endpoint of the installation"));
	retVal.append(CreateOpenAiCompatiblePreset("openai-compatible", "Other OpenAI-compatible server", "", "Optional token; set the endpoint"));

	Preset ollama = CreateOpenAiCompatiblePreset("ollama", "Ollama (local)", "http://localhost:11434/v1/chat/completions", "none");
	ollama.allowsNoCredentials = true;
	retVal.append(ollama);

	Preset vllm = CreateOpenAiCompatiblePreset("vllm", "vLLM (local)", "http://localhost:8000/v1/chat/completions", "optional API key of the server");
	vllm.allowsNoCredentials = true;
	retVal.append(vllm);

	Preset llamaCpp = CreateOpenAiCompatiblePreset("llamacpp", "llama.cpp server (local)", "http://localhost:8080/v1/chat/completions", "optional API key of the server");
	llamaCpp.allowsNoCredentials = true;
	retVal.append(llamaCpp);

	// Anthropic Messages API
	Preset anthropic;
	anthropic.id = "anthropic";
	anthropic.name = "Anthropic";
	anthropic.protocol = P_ANTHROPIC;
	anthropic.endpointTemplate = "https://api.anthropic.com/v1/messages";
	anthropic.authHeaderName = "x-api-key";
	anthropic.authValuePrefix = "";
	anthropic.extraHeaders.append(qMakePair(QByteArray("anthropic-version"), QByteArray("2023-06-01")));
	anthropic.credentialHint = "Anthropic API key";
	retVal.append(anthropic);

	Preset vertexAnthropic;
	vertexAnthropic.id = "vertex-anthropic";
	vertexAnthropic.name = "Claude on Google Vertex AI";
	vertexAnthropic.protocol = P_ANTHROPIC;
	vertexAnthropic.endpointTemplate = "https://{region}-aiplatform.googleapis.com/v1/projects/{project}/locations/{region}/publishers/anthropic/models/{model}:rawPredict";
	vertexAnthropic.includeModelInBody = false;
	vertexAnthropic.anthropicVersionBodyValue = "vertex-2023-10-16";
	vertexAnthropic.credentialHint = "OAuth access token of a service account, refreshed by the deployment";
	retVal.append(vertexAnthropic);

	// Google Gemini API
	Preset gemini;
	gemini.id = "gemini";
	gemini.name = "Google Gemini API";
	gemini.protocol = P_GEMINI;
	gemini.endpointTemplate = "https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent";
	gemini.authHeaderName = "x-goog-api-key";
	gemini.authValuePrefix = "";
	gemini.includeModelInBody = false;
	gemini.credentialHint = "Gemini API key";
	retVal.append(gemini);

	Preset vertexGemini;
	vertexGemini.id = "vertex-gemini";
	vertexGemini.name = "Gemini on Google Vertex AI";
	vertexGemini.protocol = P_GEMINI;
	vertexGemini.endpointTemplate = "https://{region}-aiplatform.googleapis.com/v1/projects/{project}/locations/{region}/publishers/google/models/{model}:generateContent";
	vertexGemini.includeModelInBody = false;
	vertexGemini.credentialHint = "OAuth access token of a service account, refreshed by the deployment";
	retVal.append(vertexGemini);

	// Amazon Bedrock Converse API
	Preset bedrock;
	bedrock.id = "bedrock";
	bedrock.name = "Amazon Bedrock";
	bedrock.protocol = P_BEDROCK;
	bedrock.endpointTemplate = "https://bedrock-runtime.{region}.amazonaws.com/model/{model}/converse";
	bedrock.includeModelInBody = false;
	bedrock.credentialHint = "Bedrock API key, or AWS access key ID and secret access key (optionally session token)";
	retVal.append(bedrock);

	return retVal;
}


bool CAssistantProviderPresets::FindPreset(const QByteArray& presetId, Preset& preset)
{
	for (const Preset& candidate: GetPresets()){
		if (candidate.id == presetId){
			preset = candidate;

			return true;
		}
	}

	return false;
}


bool CAssistantProviderPresets::ResolveEndpoint(const QString& endpointTemplate, const Parameters& parameters, QUrl& endpoint, QString* errorPtr)
{
	QString result = endpointTemplate;

	struct Placeholder
	{
		const char* name;
		QByteArray value;
	};

	Placeholder placeholders[] = {
		{"{region}", parameters.region},
		{"{project}", parameters.project},
		{"{resource}", parameters.resource}
	};

	for (const Placeholder& placeholder: placeholders){
		if (!result.contains(QLatin1String(placeholder.name))){
			continue;
		}

		if (!IsValidPlaceholderValue(placeholder.value)){
			if (errorPtr != nullptr){
				*errorPtr = QStringLiteral("Missing or invalid value for %1").arg(QLatin1String(placeholder.name));
			}

			return false;
		}

		result.replace(QLatin1String(placeholder.name), QString::fromLatin1(placeholder.value));
	}

	if (result.contains(QLatin1String("{model}"))){
		if (parameters.model.isEmpty() || (parameters.model.size() > 256)){
			if (errorPtr != nullptr){
				*errorPtr = QStringLiteral("Missing value for {model}");
			}

			return false;
		}

		// Model IDs may contain ':' or '/', e.g. on Bedrock; they form one path segment.
		result.replace(QLatin1String("{model}"), QString::fromLatin1(QUrl::toPercentEncoding(QString::fromUtf8(parameters.model))));
	}

	if (result.contains('{') || result.contains('}')){
		if (errorPtr != nullptr){
			*errorPtr = QStringLiteral("Unknown placeholder in the endpoint");
		}

		return false;
	}

	endpoint = QUrl(result, QUrl::StrictMode);
	if (!CHttpAssistantProviderBase::IsAllowedEndpoint(endpoint)){
		if (errorPtr != nullptr){
			*errorPtr = QStringLiteral("The endpoint must be an https URL, or an http URL of the local machine");
		}

		return false;
	}

	return true;
}


bool CAssistantProviderPresets::ApplyPreset(const Preset& preset, const Parameters& parameters, CHttpAssistantProviderBase::Settings& settings, QString* errorPtr)
{
	QString endpointTemplate = parameters.endpoint.isEmpty() ? preset.endpointTemplate : parameters.endpoint;
	if (endpointTemplate.isEmpty()){
		if (errorPtr != nullptr){
			*errorPtr = QStringLiteral("The provider '%1' needs an endpoint").arg(QString::fromUtf8(preset.id));
		}

		return false;
	}

	QUrl endpoint;
	if (!ResolveEndpoint(endpointTemplate, parameters, endpoint, errorPtr)){
		return false;
	}

	settings.endpoint = endpoint;
	settings.model = parameters.model;
	settings.includeModelInBody = preset.includeModelInBody;
	settings.authHeaderName = preset.authHeaderName;
	settings.authValuePrefix = preset.authValuePrefix;
	settings.extraHeaders = preset.extraHeaders;
	settings.allowsNoCredentials = preset.allowsNoCredentials;
	settings.sendParallelToolCallsFlag = preset.sendParallelToolCallsFlag;
	settings.maxTokensField = preset.maxTokensField;
	settings.anthropicVersionBodyValue = preset.anthropicVersionBodyValue;
	settings.awsRegion = parameters.region;

	return true;
}


std::shared_ptr<CHttpAssistantProviderBase> CAssistantProviderPresets::CreateProvider(Protocol protocol)
{
	switch (protocol){
	case P_OPENAI_COMPATIBLE:
		return std::make_shared<COpenAiCompatibleProvider>();
	case P_ANTHROPIC:
		return std::make_shared<CAnthropicMessagesProvider>();
	case P_GEMINI:
		return std::make_shared<CGeminiProvider>();
	case P_BEDROCK:
		return std::make_shared<CBedrockConverseProvider>();
	}

	return std::shared_ptr<CHttpAssistantProviderBase>();
}


} // namespace imtassist


