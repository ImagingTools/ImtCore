# MCP server: decision and design sketch

Status: **deferred**. The built-in assistant comes first in all server products (Puma, ProLife,
Agentino, Dogma, Claire). An MCP endpoint is added when a customer asks to connect an own AI
agent to one of our servers. This document records the decision and how the endpoint maps onto
the existing assistant components, so it can be built without redesign.

## What MCP is

The Model Context Protocol (MCP) is an open protocol between an AI client and an external
system. An MCP server publishes:

- **tools**: functions with a JSON schema for their parameters,
- **resources**: data the client can read,
- **prompts**: prepared templates.

The model runs on the client side (a desktop assistant, an IDE, an agent of the customer). It
decides which tool to call; the server executes it.

The built-in assistant works the other way round: our server calls the model, and the tools live
inside the server. Both directions use the same building blocks, so the endpoint is an adapter,
not a second implementation.

## Decision

Not now, for these reasons:

- **Confirmation of changes.** In the built-in assistant a change is applied only after the user
  confirms the preview in our user interface. With MCP the confirmation is shown by a foreign
  client whose behaviour we do not control.
- **Authentication.** Remote MCP expects OAuth 2.1 for third-party clients. Our servers do not
  offer OAuth authorization for third-party clients yet; this is a sensitive change on its own.
- **Prompt injection.** A foreign agent may carry instructions from sources outside our product.
  The permissions of the user limit the damage, but the attack surface is larger.
- **No demand yet.** Without a customer use case it is one more API to maintain and secure.

Reasons to build it later:

- Customers can use our servers from their own agents and automations, also across systems (for
  example Puma together with their ERP).
- It works without internet access: an MCP client inside the customer network connects to our
  server locally, in line with production servers having no access to external services.

## Mapping onto the existing components

| MCP | Existing component |
| --- | --- |
| tools/list | `IAssistantActionRegistry`, name and description of each `IAssistantAction` |
| tool input schema | `CParameterSchema` (already strict JSON schema) |
| tools/call (read) | `IAssistantAction::Execute` with `AK_READ` |
| tools/call (change) | `PrepareChange` and `ApplyChange` with preview and state token (see below) |
| resources | `IAssistantKnowledgeBase` sections and `IAssistantApiCatalog` command descriptions |
| user and tenant | `CAssistantRequestContext` with the cloned GraphQL context |
| permissions | the existing checks in the actions and GraphQL handlers, unchanged |

The transport is a Streamable HTTP endpoint in `imtrest`, registered in the server composite next
to the GraphQL endpoint. It uses the same action registry as the `TenantAssistant` composite, so
allow lists, forbidden fields and permission checks stay in one place.

## Stages

1. **Read only.** Publish `knowledge.search`, `knowledge.read`, `api.listCommands`,
   `api.describeCommand`, `api.query` and the administration search actions. Resources for the
   knowledge base and the API catalog.
2. **Changes with two steps.** A change tool returns only the preview and a preview id. A
   separate tool `assistant.applyPreview` applies it; the preview stays bound to user, tenant and
   state token exactly as today. The client is expected to ask the user between the two calls;
   the server still refuses stale or foreign previews.
3. **Optional:** a page in the administration area that lists connected MCP clients per tenant
   and can revoke them.

## Requirements before the first release

- Per-tenant switch in the assistant configuration ("MCP access allowed"), off by default.
- Authentication of the MCP client with the user's rights (OAuth 2.1 or a per-user token issued
  in the administration area); never a shared server credential. Treat as a sensitive change and
  go through `Docs/Development/ServerChangeChecklist.md`.
- The same logging rules as the assistant: no prompts, parameter values, results or secrets in
  logs.
- Rate limits per user and tenant.
- Tests for refusal of foreign tenants, stale previews and forbidden fields through the endpoint.
