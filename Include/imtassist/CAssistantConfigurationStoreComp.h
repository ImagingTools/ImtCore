// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/ISecretProtector.h>
#include <imtassist/IAssistantConfigurationStore.h>
#include <imtassist/IAssistantTenantPolicy.h>


namespace imtassist
{


/**
	Store of the assistant configurations of the tenants in an object collection
	(one object per tenant, the object ID is the tenant ID), e.g. the SQL collection of the
	composite ImtAssistVoce/AssistantConfigurationSqlRepository.

	The policy of the server is given by attributes: the presets a tenant may use
	(\c AllowedPresets) and whether a tenant may set its own endpoint (\c AllowCustomEndpoints).
	A custom endpoint is a request of the server to an address chosen by a tenant, so it is
	refused unless the operator allows it.

	The component is also the tenant policy of the assistant service: the assistant answers a
	tenant only if its configuration is enabled. Configurations are cached for a short time.

	\ingroup imtassist
*/
class CAssistantConfigurationStoreComp:
			public ilog::CLoggerComponentBase,
			virtual public IAssistantConfigurationStore,
			virtual public IAssistantTenantPolicy
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	enum
	{
		MAX_TEXT_LENGTH = 500,
		MAX_INSTRUCTIONS_LENGTH = 4000,
		MAX_API_KEY_LENGTH = 8192,
		MAX_CACHE_SIZE = 1000
	};

	I_BEGIN_COMPONENT(CAssistantConfigurationStoreComp);
		I_REGISTER_INTERFACE(IAssistantConfigurationStore);
		I_REGISTER_INTERFACE(IAssistantTenantPolicy);
		I_ASSIGN(m_collectionCompPtr, "ConfigurationCollection", "Stored configurations, the object ID is the tenant ID", true, "ConfigurationCollection");
		I_ASSIGN(m_configurationFactoryCompPtr, "ConfigurationFactory", "Factory of a configuration", true, "AssistantConfiguration");
		I_ASSIGN(m_secretProtectorCompPtr, "SecretProtector", "Protection of the API keys at rest; without it no API key can be stored", false, "SecretProtector");
		I_ASSIGN_MULTI_0(m_allowedPresetsAttrPtr, "AllowedPresets", "Presets tenants may use; all presets with a fixed endpoint when empty", false);
		I_ASSIGN(m_allowCustomEndpointsAttrPtr, "AllowCustomEndpoints", "Allow tenants to set their own endpoint. Only for servers whose network is restricted; the server sends requests to that address", true, false);
		I_ASSIGN(m_cacheTimeToLiveAttrPtr, "CacheTimeToLive", "Time in seconds a read configuration is reused; 0 reads it for every request", true, 10);
	I_END_COMPONENT;

	// reimplemented (imtassist::IAssistantConfigurationStore)
	virtual IAssistantConfigurationUniquePtr GetConfiguration(const QByteArray& tenantId) const override;
	virtual bool SetConfiguration(
				const IAssistantConfiguration& configuration,
				ApiKeyChange apiKeyChange,
				const QByteArray& apiKey,
				QString& errorMessage) override;
	virtual bool ValidateConfiguration(const IAssistantConfiguration& configuration, QString& errorMessage) const override;
	virtual bool ReadApiKey(const IAssistantConfiguration& configuration, QByteArray& apiKey) const override;
	virtual QByteArrayList GetAllowedPresetIds() const override;
	virtual bool AreCustomEndpointsAllowed() const override;

	// reimplemented (imtassist::IAssistantTenantPolicy)
	virtual bool IsAssistantEnabledForTenant(const QByteArray& tenantId) const override;

private:
	struct CacheEntry
	{
		IAssistantConfigurationSharedPtr configurationPtr;
		QDateTime readAt;
	};

	/**
		Read the configuration from the collection.
		\param isStored		Set if an object of the tenant was read.
	*/
	IAssistantConfigurationUniquePtr ReadConfiguration(const QByteArray& tenantId, bool& isStored) const;
	IAssistantConfigurationUniquePtr CopyConfiguration(const IAssistantConfiguration& configuration) const;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_FACT(IAssistantConfiguration, m_configurationFactoryCompPtr);
	I_REF(imtcrypt::ISecretProtector, m_secretProtectorCompPtr);
	I_MULTIATTR(QByteArray, m_allowedPresetsAttrPtr);
	I_ATTR(bool, m_allowCustomEndpointsAttrPtr);
	I_ATTR(int, m_cacheTimeToLiveAttrPtr);

	mutable QMutex m_cacheMutex;
	mutable QMap<QByteArray, CacheEntry> m_cache;
};


} // namespace imtassist


