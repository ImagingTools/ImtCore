// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/CAssistantProviderPresets.h>
#include <imtassist/IAssistantConfigurationStore.h>


namespace imtassist
{


/**
	Model provider configured per tenant in the administration.
	For every request the configuration of the tenant of the request is read from the
	configuration store and checked against the policy of the server; the adapter is
	recreated when the configuration changed.

	A tenant uses its own API key. Credentials of the server (environment variables) are
	used only if \c AllowServerCredentials is set, e.g. for a server that pays the model
	usage of all tenants.

	\ingroup imtassist
*/
class CTenantAssistantProviderComp:
			public ilog::CLoggerComponentBase,
			virtual public IAssistantProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	enum
	{
		MAX_CACHED_PROVIDERS = 256
	};

	I_BEGIN_COMPONENT(CTenantAssistantProviderComp);
		I_REGISTER_INTERFACE(IAssistantProvider);
		I_ASSIGN(m_configurationStoreCompPtr, "ConfigurationStore", "Assistant configurations of the tenants", true, "ConfigurationStore");
		I_ASSIGN(m_allowServerCredentialsAttrPtr, "AllowServerCredentials", "Use the credentials of the server (environment variables) for tenants without own API key", true, false);
		I_ASSIGN(m_apiKeyVariableAttrPtr, "ApiKeyVariable", "Environment variable with the API key of the server (only with AllowServerCredentials)", false, "IMT_ASSISTANT_API_KEY");
		I_ASSIGN(m_awsAccessKeyIdVariableAttrPtr, "AwsAccessKeyIdVariable", "Environment variable with the AWS access key ID of the server (only with AllowServerCredentials)", false, "AWS_ACCESS_KEY_ID");
		I_ASSIGN(m_awsSecretAccessKeyVariableAttrPtr, "AwsSecretAccessKeyVariable", "Environment variable with the AWS secret access key of the server", false, "AWS_SECRET_ACCESS_KEY");
		I_ASSIGN(m_awsSessionTokenVariableAttrPtr, "AwsSessionTokenVariable", "Environment variable with the AWS session token of the server (optional)", false, "AWS_SESSION_TOKEN");
		I_ASSIGN(m_additionalInstructionsAttrPtr, "AdditionalInstructions", "Instructions of the product appended to the system prompt before those of the tenant", false, "");
	I_END_COMPONENT;

	// reimplemented (imtassist::IAssistantProvider)
	virtual bool IsConfigured() const override;
	virtual Reply Complete(const Request& request, const CCancellationToken& cancellationToken) override;

private:
	struct Entry
	{
		QByteArray presetId;
		CAssistantProviderPresets::Parameters parameters;
		QByteArray protectedApiKey;
		int maxOutputTokens = 0;
		QString additionalInstructions;
		std::shared_ptr<IHttpAssistantProvider> providerPtr;

		bool IsSameConfiguration(const Entry& other) const;
	};

	/**
		Get the adapter of the tenant; nullptr if the tenant has no valid configuration.
	*/
	std::shared_ptr<IHttpAssistantProvider> GetProvider(const QByteArray& tenantId);

private:
	I_REF(IAssistantConfigurationStore, m_configurationStoreCompPtr);
	I_ATTR(bool, m_allowServerCredentialsAttrPtr);
	I_ATTR(QByteArray, m_apiKeyVariableAttrPtr);
	I_ATTR(QByteArray, m_awsAccessKeyIdVariableAttrPtr);
	I_ATTR(QByteArray, m_awsSecretAccessKeyVariableAttrPtr);
	I_ATTR(QByteArray, m_awsSessionTokenVariableAttrPtr);
	I_TEXTATTR(m_additionalInstructionsAttrPtr);

	QMutex m_mutex;
	QMap<QByteArray, Entry> m_entries;
};


} // namespace imtassist


