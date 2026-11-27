// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassist/CAssistantServiceComp.h>
#include <imtassist/CAssistantActionRegistryComp.h>
#include <imtassist/CUiRepresentationRegistryComp.h>
#include <imtassist/CFakeAssistantProviderComp.h>
#include <imtassist/CAssistantAuditLogComp.h>
#include <imtassist/CHttpAssistantProviderComp.h>
#include <imtassist/CAssistantProviderPresetListComp.h>
#include <imtassist/CAssistantConfigurationComp.h>
#include <imtassist/CAssistantConfigurationStoreComp.h>
#include <imtassist/CTenantAssistantProviderComp.h>


/**
	ImtAssistPck package
*/
namespace ImtAssistPck
{


typedef imtassist::CAssistantServiceComp AssistantService;
typedef imtassist::CAssistantActionRegistryComp AssistantActionRegistry;
typedef imtassist::CUiRepresentationRegistryComp UiRepresentationRegistry;
typedef imtassist::CFakeAssistantProviderComp FakeAssistantProvider;
typedef imtassist::CAssistantAuditLogComp AssistantAuditLog;
typedef imtassist::CHttpAssistantProviderComp HttpAssistantProvider;
typedef imtassist::CAssistantProviderPresetListComp AssistantProviderList;
typedef imtassist::CAssistantConfigurationComp AssistantConfiguration;
typedef imtassist::CAssistantConfigurationStoreComp AssistantConfigurationStore;
typedef imtassist::CTenantAssistantProviderComp TenantAssistantProvider;


} // namespace ImtAssistPck


