// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtassist
{


/**
	Configuration of the assistant of one tenant, edited in the administration of the tenant.
	The API key is kept only in protected form (\see imtcrypt::ISecretProtector); the clear
	text never leaves the server and is never shown again after it was entered.

	\ingroup imtassist
*/
class IAssistantConfiguration: virtual public iser::IObject
{
public:
	enum
	{
		DEFAULT_MAX_OUTPUT_TOKENS = 1024,
		MAX_OUTPUT_TOKENS_LIMIT = 100000
	};

	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	/**
		Whether the assistant is enabled for the tenant.
	*/
	virtual bool IsEnabled() const = 0;
	virtual void SetEnabled(bool isEnabled) = 0;

	/**
		ID of the provider preset, \see CAssistantProviderPresets.
	*/
	virtual QByteArray GetPresetId() const = 0;
	virtual void SetPresetId(const QByteArray& presetId) = 0;

	virtual QString GetModel() const = 0;
	virtual void SetModel(const QString& model) = 0;

	/**
		Endpoint that replaces the one of the preset; only used if the server allows custom endpoints.
	*/
	virtual QString GetEndpoint() const = 0;
	virtual void SetEndpoint(const QString& endpoint) = 0;

	virtual QString GetRegion() const = 0;
	virtual void SetRegion(const QString& region) = 0;

	virtual QString GetProject() const = 0;
	virtual void SetProject(const QString& project) = 0;

	virtual QString GetResource() const = 0;
	virtual void SetResource(const QString& resource) = 0;

	/**
		API key or access token, protected by the secret protector of the server. Empty if none is set.
	*/
	virtual QByteArray GetProtectedApiKey() const = 0;
	virtual void SetProtectedApiKey(const QByteArray& protectedApiKey) = 0;

	virtual int GetMaxOutputTokens() const = 0;
	virtual void SetMaxOutputTokens(int maxOutputTokens) = 0;

	/**
		Instructions of the tenant appended to the system prompt.
	*/
	virtual QString GetAdditionalInstructions() const = 0;
	virtual void SetAdditionalInstructions(const QString& instructions) = 0;

	virtual QDateTime GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QDateTime& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<IAssistantConfiguration> IAssistantConfigurationUniquePtr;
typedef istd::TSharedInterfacePtr<IAssistantConfiguration> IAssistantConfigurationSharedPtr;


} // namespace imtassist


