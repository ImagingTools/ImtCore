// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ImtCore includes
#include <imtassist/IAssistantConfiguration.h>


namespace imtassist
{


/**
	Store of the assistant configurations of the tenants, together with the policy of the
	server that limits what a tenant may configure.
	The store protects and unprotects the API keys; callers never handle protected data.

	\ingroup imtassist
*/
class IAssistantConfigurationStore: virtual public istd::IPolymorphic
{
public:
	enum ApiKeyChange
	{
		/**
			Keep the stored API key.
		*/
		AKC_KEEP,

		/**
			Replace the stored API key by the given one.
		*/
		AKC_SET,

		/**
			Remove the stored API key.
		*/
		AKC_CLEAR
	};

	/**
		Get the configuration of a tenant; a disabled default configuration if none is stored.
		\return nullptr if the tenant ID is empty or the configuration could not be read.
	*/
	virtual IAssistantConfigurationUniquePtr GetConfiguration(const QByteArray& tenantId) const = 0;

	/**
		Validate and store the configuration of its tenant.
		The protected API key of \c configuration is ignored; \c apiKeyChange decides about the key.
		\param apiKey			Clear text of the new API key for AKC_SET; never logged or stored in clear text.
		\param errorMessage		Reason of a rejection, without any value of the configuration.
	*/
	virtual bool SetConfiguration(
				const IAssistantConfiguration& configuration,
				ApiKeyChange apiKeyChange,
				const QByteArray& apiKey,
				QString& errorMessage) = 0;

	/**
		Check a configuration against the policy of the server.
	*/
	virtual bool ValidateConfiguration(const IAssistantConfiguration& configuration, QString& errorMessage) const = 0;

	/**
		Get the clear text of the API key of a configuration, for the request to the provider only.
		\return False if the key could not be unprotected; true with an empty key if none is set.
	*/
	virtual bool ReadApiKey(const IAssistantConfiguration& configuration, QByteArray& apiKey) const = 0;

	/**
		Provider presets the server allows tenants to use.
	*/
	virtual QByteArrayList GetAllowedPresetIds() const = 0;

	/**
		Whether tenants may set their own endpoint (e.g. a self-hosted model server).
	*/
	virtual bool AreCustomEndpointsAllowed() const = 0;
};


} // namespace imtassist


