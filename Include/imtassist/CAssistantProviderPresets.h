// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <memory>

// Qt includes
#include <QtCore/QStringList>

// ImtCore includes
#include <imtassist/CHttpAssistantProviderBase.h>


namespace imtassist
{


/**
	Known model providers: API protocol, endpoint and authentication of each.

	The endpoints are templates with the placeholders {model}, {region}, {project} and
	{resource}. Every value of a preset can be overridden in the configuration, e.g. the
	endpoint of a self-hosted server. Check the endpoints against the documentation of the
	provider when configuring a product; providers change them.

	\ingroup imtassist
*/
class CAssistantProviderPresets
{
public:
	enum Protocol
	{
		P_OPENAI_COMPATIBLE,
		P_ANTHROPIC,
		P_GEMINI,
		P_BEDROCK
	};

	struct Preset
	{
		QByteArray id;
		QString name;
		Protocol protocol = P_OPENAI_COMPATIBLE;
		QString endpointTemplate;
		QByteArray authHeaderName = "Authorization";
		QByteArray authValuePrefix = "Bearer ";
		IAssistantHttpTransport::Headers extraHeaders;
		bool allowsNoCredentials = false;
		bool sendParallelToolCallsFlag = false;
		QByteArray maxTokensField = "max_tokens";
		bool includeModelInBody = true;
		QByteArray anthropicVersionBodyValue;
		QString credentialHint;
	};

	typedef QList<Preset> Presets;

	/**
		Values that complete a preset.
	*/
	struct Parameters
	{
		/**
			Endpoint that replaces the template of the preset; may use placeholders.
		*/
		QString endpoint;
		QByteArray model;
		QByteArray region;
		QByteArray project;
		QByteArray resource;
	};

	static Presets GetPresets();
	static bool FindPreset(const QByteArray& presetId, Preset& preset);

	/**
		Replace the placeholders of an endpoint template.
		\return False if a placeholder has no or an invalid value, or the result is not a valid URL.
	*/
	static bool ResolveEndpoint(const QString& endpointTemplate, const Parameters& parameters, QUrl& endpoint, QString* errorPtr = nullptr);

	/**
		Fill the provider settings from a preset. Credentials are not touched.
	*/
	static bool ApplyPreset(const Preset& preset, const Parameters& parameters, CHttpAssistantProviderBase::Settings& settings, QString* errorPtr = nullptr);

	/**
		Create the adapter of a protocol.
	*/
	static std::shared_ptr<CHttpAssistantProviderBase> CreateProvider(Protocol protocol);
};


} // namespace imtassist


