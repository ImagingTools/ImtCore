// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ISelectionParam.h>
#include <iprm/ITextParam.h>


namespace imtassist
{


/**
	Base of CHttpAssistantProviderComp with the references to the settings of the server.
	Split from the component, because MSVC limits the number of static objects in one
	InitStaticInfo function (error C2603).

	\ingroup imtassist
*/
class CHttpAssistantProviderCompBase: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CHttpAssistantProviderCompBase);
		I_ASSIGN(m_providerParamCompPtr, "ProviderParam", "Setting of the server: selected provider, option IDs are preset IDs; Preset when unset", false, "ProviderParam");
		I_ASSIGN(m_modelParamCompPtr, "ModelParam", "Setting of the server: model ID; Model when empty", false, "ModelParam");
		I_ASSIGN(m_endpointParamCompPtr, "EndpointParam", "Setting of the server: endpoint; Endpoint when empty", false, "EndpointParam");
		I_ASSIGN(m_regionParamCompPtr, "RegionParam", "Setting of the server: region; Region when empty", false, "RegionParam");
		I_ASSIGN(m_projectParamCompPtr, "ProjectParam", "Setting of the server: Google Cloud project; Project when empty", false, "ProjectParam");
		I_ASSIGN(m_resourceParamCompPtr, "ResourceParam", "Setting of the server: Azure OpenAI resource; Resource when empty", false, "ResourceParam");
		I_ASSIGN(m_apiKeyParamCompPtr, "ApiKeyParam", "Setting of the server: API key or access token (use a protected secret parameter); ApiKeyVariable when empty", false, "ApiKeyParam");
		I_ASSIGN(m_maxOutputTokensParamCompPtr, "MaxOutputTokensParam", "Setting of the server: maximum number of generated tokens; MaxOutputTokens when empty or invalid", false, "MaxOutputTokensParam");
		I_ASSIGN(m_additionalInstructionsParamCompPtr, "AdditionalInstructionsParam", "Setting of the server: additional instructions; AdditionalInstructions when empty", false, "AdditionalInstructionsParam");
	I_END_COMPONENT;

protected:
	I_REF(iprm::ISelectionParam, m_providerParamCompPtr);
	I_REF(iprm::ITextParam, m_modelParamCompPtr);
	I_REF(iprm::ITextParam, m_endpointParamCompPtr);
	I_REF(iprm::ITextParam, m_regionParamCompPtr);
	I_REF(iprm::ITextParam, m_projectParamCompPtr);
	I_REF(iprm::ITextParam, m_resourceParamCompPtr);
	I_REF(iprm::ITextParam, m_apiKeyParamCompPtr);
	I_REF(iprm::ITextParam, m_maxOutputTokensParamCompPtr);
	I_REF(iprm::ITextParam, m_additionalInstructionsParamCompPtr);
};


} // namespace imtassist


