// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassist/CAssistantLogicControllerComp.h>
#include <imtassist/CAssistantActionRegistryComp.h>
#include <imtassist/CUiRepresentationRegistryComp.h>
#include <imtassist/CFakeAssistantProviderComp.h>
#include <imtassist/CAssistantAuditLogComp.h>
#include <imtassist/CHttpAssistantProviderComp.h>
#include <imtassist/CAssistantProviderPresetListComp.h>
#include <imtassist/CAssistantConfigurationComp.h>
#include <imtassist/CAssistantConfigurationStoreComp.h>
#include <imtassist/CTenantAssistantProviderComp.h>
#include <imtassist/CMarkdownKnowledgeBaseComp.h>
#include <imtassist/CAssistantKnowledgeActionComp.h>


/**
	ImtAssistPck package
*/
namespace ImtAssistPck
{


typedef imtassist::CAssistantLogicControllerComp AssistantLogicController;
typedef imtassist::CAssistantActionRegistryComp AssistantActionRegistry;
typedef imtassist::CUiRepresentationRegistryComp UiRepresentationRegistry;
typedef imtassist::CFakeAssistantProviderComp FakeAssistantProvider;
typedef imtassist::CAssistantAuditLogComp AssistantAuditLog;
typedef imtassist::CHttpAssistantProviderComp HttpAssistantProvider;
typedef imtassist::CAssistantProviderPresetListComp AssistantProviderList;
typedef imtassist::CAssistantConfigurationComp AssistantConfiguration;
typedef imtassist::CAssistantConfigurationStoreComp AssistantConfigurationStore;
typedef imtassist::CTenantAssistantProviderComp TenantAssistantProvider;
typedef imtassist::CMarkdownKnowledgeBaseComp MarkdownKnowledgeBase;
typedef imtassist::CAssistantKnowledgeActionComp AssistantKnowledgeAction;


} // namespace ImtAssistPck


