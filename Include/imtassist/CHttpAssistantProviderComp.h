// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMutex>

// ImtCore includes
#include <imtassist/CHttpAssistantProviderCompBase.h>
#include <imtassist/CAssistantProviderPresets.h>


namespace imtassist
{


/**
	Component of a model provider reached over HTTP.

	The attribute \c Preset selects the provider (see CAssistantProviderPresets and
	Docs/Assistant/README.md). Every attribute can be overridden by a setting of the server
	(the \c ...Param references, e.g. parameters of the server settings);
	a setting that is not set or empty falls back to the attribute. Settings are read at
	every request, so a change in the server settings takes effect without restart.

	The API key is taken from the \c ApiKeyParam setting (a protected secret) if set,
	otherwise from the environment variable named by \c ApiKeyVariable. Without credentials
	the provider reports itself as not configured, and the assistant service sends no request.

	\ingroup imtassist
*/
class CHttpAssistantProviderComp:
			public CHttpAssistantProviderCompBase,
			virtual public IAssistantProvider
{
public:
	typedef CHttpAssistantProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpAssistantProviderComp);
		I_REGISTER_INTERFACE(IAssistantProvider);
		I_ASSIGN(m_presetAttrPtr, "Preset", "Provider: openai, azure-openai, mistral, cohere, ionos, stackit, aleph-alpha, openai-compatible, ollama, vllm, llamacpp, anthropic, vertex-anthropic, gemini, vertex-gemini, bedrock", true, "");
		I_ASSIGN(m_endpointAttrPtr, "Endpoint", "Endpoint that replaces the one of the preset; may use {model}, {region}, {project}, {resource}", false, "");
		I_ASSIGN(m_modelAttrPtr, "Model", "Model ID (deployment name on Azure OpenAI)", false, "");
		I_ASSIGN(m_regionAttrPtr, "Region", "Region, e.g. for Vertex AI and Bedrock", false, "");
		I_ASSIGN(m_projectAttrPtr, "Project", "Google Cloud project for Vertex AI", false, "");
		I_ASSIGN(m_resourceAttrPtr, "Resource", "Azure OpenAI resource name", false, "");
		I_ASSIGN(m_apiKeyVariableAttrPtr, "ApiKeyVariable", "Environment variable with the API key or access token", false, "IMT_ASSISTANT_API_KEY");
		I_ASSIGN(m_awsAccessKeyIdVariableAttrPtr, "AwsAccessKeyIdVariable", "Environment variable with the AWS access key ID (Bedrock without API key)", false, "AWS_ACCESS_KEY_ID");
		I_ASSIGN(m_awsSecretAccessKeyVariableAttrPtr, "AwsSecretAccessKeyVariable", "Environment variable with the AWS secret access key", false, "AWS_SECRET_ACCESS_KEY");
		I_ASSIGN(m_awsSessionTokenVariableAttrPtr, "AwsSessionTokenVariable", "Environment variable with the AWS session token (optional)", false, "AWS_SESSION_TOKEN");
		I_ASSIGN(m_maxOutputTokensAttrPtr, "MaxOutputTokens", "Maximum number of generated tokens", true, 1024);
		I_ASSIGN(m_additionalInstructionsAttrPtr, "AdditionalInstructions", "Instructions of the application appended to the system prompt", false, "");
	I_END_COMPONENT;

	// reimplemented (imtassist::IAssistantProvider)
	virtual bool IsConfigured() const override;
	virtual Reply Complete(const Request& request, const CCancellationToken& cancellationToken) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	/**
		Configuration from the settings and attributes; compared to detect changes.
	*/
	struct Configuration
	{
		QByteArray presetId;
		CAssistantProviderPresets::Parameters parameters;
		QByteArray apiKey;
		int maxOutputTokens = 1024;
		QString additionalInstructions;

		bool operator==(const Configuration& other) const;
	};

	Configuration ReadConfiguration() const;

	/**
		Get the provider for the current configuration; it is recreated when the configuration changed.
		\return nullptr if the configuration is invalid.
	*/
	std::shared_ptr<CHttpAssistantProviderBase> GetProvider() const;

	static QString ReadText(const iprm::ITextParam* paramPtr);

private:
	I_ATTR(QByteArray, m_presetAttrPtr);
	I_TEXTATTR(m_endpointAttrPtr);
	I_ATTR(QByteArray, m_modelAttrPtr);
	I_ATTR(QByteArray, m_regionAttrPtr);
	I_ATTR(QByteArray, m_projectAttrPtr);
	I_ATTR(QByteArray, m_resourceAttrPtr);
	I_ATTR(QByteArray, m_apiKeyVariableAttrPtr);
	I_ATTR(QByteArray, m_awsAccessKeyIdVariableAttrPtr);
	I_ATTR(QByteArray, m_awsSecretAccessKeyVariableAttrPtr);
	I_ATTR(QByteArray, m_awsSessionTokenVariableAttrPtr);
	I_ATTR(int, m_maxOutputTokensAttrPtr);
	I_TEXTATTR(m_additionalInstructionsAttrPtr);

	mutable QMutex m_mutex;
	mutable bool m_hasConfiguration = false;
	mutable Configuration m_configuration;
	mutable std::shared_ptr<CHttpAssistantProviderBase> m_providerPtr;
};


} // namespace imtassist


