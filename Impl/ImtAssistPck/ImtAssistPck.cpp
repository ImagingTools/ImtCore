// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAssistPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAssistPck
{


I_EXPORT_PACKAGE(
			"ImtAssistPck",
			"AI assistant core component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AssistantLogicController,
			"Provider-independent assistant service, disabled by default",
			"Assistant AI Service");

I_EXPORT_COMPONENT(
			AssistantActionRegistry,
			"Registry of the actions available to the assistant",
			"Assistant AI Action Registry");

I_EXPORT_COMPONENT(
			UiRepresentationRegistry,
			"Registry of the UI representation types known to the application",
			"Assistant AI UI Representation Registry");

I_EXPORT_COMPONENT(
			FakeAssistantProvider,
			"Deterministic assistant provider for development, demos and tests",
			"Assistant AI Provider Fake Test");

I_EXPORT_COMPONENT(
			HttpAssistantProvider,
			"Model provider over HTTP: OpenAI, Azure OpenAI, Mistral, Cohere, IONOS, STACKIT, Aleph Alpha, Anthropic, Gemini, Vertex AI, Bedrock, Ollama, vLLM, llama.cpp",
			"Assistant AI Provider HTTP OpenAI Anthropic Gemini Bedrock Mistral");

I_EXPORT_COMPONENT(
			AssistantProviderList,
			"The supported model providers as options for the provider selection in the server settings",
			"Assistant AI Provider List Options Settings");

I_EXPORT_COMPONENT(
			AssistantAuditLog,
			"Writes assistant audit events without content to the log",
			"Assistant AI Audit Log");

I_EXPORT_COMPONENT(
			AssistantConfiguration,
			"Assistant configuration of a tenant: provider, model, protected API key, enabling",
			"Assistant AI Configuration Tenant");

I_EXPORT_COMPONENT(
			AssistantConfigurationStore,
			"Assistant configurations of the tenants in an object collection, with the policy of the server; tenant policy of the assistant service",
			"Assistant AI Configuration Store Tenant Policy");

I_EXPORT_COMPONENT(
			TenantAssistantProvider,
			"Model provider configured per tenant in the administration",
			"Assistant AI Provider Tenant Administration");

I_EXPORT_COMPONENT(
			MarkdownKnowledgeBase,
			"Knowledge base of the product for the assistant from Markdown files",
			"Assistant AI Knowledge Documentation Markdown");

I_EXPORT_COMPONENT(
			AssistantKnowledgeAction,
			"Assistant actions knowledge.search and knowledge.read on the knowledge base",
			"Assistant AI Action Knowledge Search Read");


} // namespace ImtAssistPck


