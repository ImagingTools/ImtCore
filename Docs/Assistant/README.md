# AI Assistant (`imtassist`)

Generic, provider-independent AI assistant for ImtCore applications (issue #1206).
Users describe a task in natural language, answer clarifying questions and work on the
results with registered application components. The assistant supports existing
workflows; it never bypasses permissions or business logic.

> **Status:** MVP implemented: core library `imtassist` with adapters to the common model
> providers, GraphQL API `imtassistgql`, QML panel `imtassistgui` and an example in the
> ControlsGallery. Which provider a product uses is a product decision
> (see [Open decisions](#open-decisions-before-productive-use)).

| Module | Content |
|---|---|
| `Include/imtassist` | Core library and ACF components |
| `Include/imtassistgql` | GraphQL controller |
| `Qml/imtassistgui` | QML panel, controller contract, GraphQL controller, standard representations |
| `Impl/ImtAssistPck`, `Impl/ImtAssistGqlPck` | Component packages |
| `Impl/ImtControlsGallery` | Example: demo tasks, actions, configuration and the page "Assistant" |

## Building blocks

| Class | Purpose |
|---|---|
| `IAssistantService` / `CAssistantService` | Conversation turns, confirmation and rejection of previews, cancellation. Disabled by default. |
| `IAssistantAction` | Domain function registered by the application: stable ID, description, versioned parameter and result schema, reading or changing. |
| `CAssistantActionRegistry` | Registry of actions. Only registered actions can be called. |
| `CParameterSchema` | Strict, versioned schema of a JSON object (parameters, results, UI data). |
| `IUiRepresentationRegistry` / `CUiRepresentationRegistry` | Known UI representation types (`type`, `version`) and their data schemas. |
| `CActionPreviewStore` | Server-side store of prepared changes: bound, time-limited, executed at most once. |
| `IAssistantProvider` | Adapter to a model provider. |
| `CFakeAssistantProvider` | Deterministic provider for development and tests; never contacts a network. |
| `CHttpAssistantProviderBase` | Common part of the HTTP adapters: turns, tool names, clarification tool, credentials, errors. |
| `COpenAiCompatibleProvider`, `CAnthropicMessagesProvider`, `CGeminiProvider`, `CBedrockConverseProvider` | Adapters of the four API protocols. |
| `CAssistantProviderPresets` | Endpoint, protocol and authentication of every supported provider. |
| `IAssistantHttpTransport` / `CRestHttpTransport` | Blocking HTTP POST with timeout and cancellation, implemented with `imtrest::CHttpClientRequest` and `imtrest::CHttpClientRequestSender`; replaceable in tests. |
| `CAwsSigV4Signer` | AWS Signature Version 4 for Bedrock. |
| `CAssistantMessage` | Typed, versioned message exchanged with the UI. |
| `IAssistantAuditLog` | Receiver of technical events and action decisions without content. |
| `CAssistantRequestContext` | User, tenant, correlation ID and context released by the user. |

The classes without the `Comp` suffix depend only on Qt Core and ACF `istd`. The module is
independent of `imtchat` (Service Desk chat); no storage, data model or QML component is shared,
and `imtchatgui` is unchanged.

### Components (`ImtAssistPck`, `ImtAssistGqlPck`)

| Component | Attributes |
|---|---|
| `ImtAssistPck/AssistantService` | `Enabled` (default `false`), `EnabledParam` (server setting), `Provider`, `ActionRegistry`, `RepresentationRegistry`, `AuditLog`, `ProviderTimeout`, `MaxStepsPerTurn`, `PreviewTimeToLive` |
| `ImtAssistPck/AssistantActionRegistry` | `Actions`: the action components of the application |
| `ImtAssistPck/UiRepresentationRegistry` | `RegisterStandardRepresentations` (default `true`), `SchemaProviders` (`IUiRepresentationSchemaProvider`) |
| `ImtAssistPck/HttpAssistantProvider` | `Preset`, `Model`, `Endpoint`, `Region`, `Project`, `Resource`, `ApiKeyVariable`, `AwsAccessKeyIdVariable`, `AwsSecretAccessKeyVariable`, `AwsSessionTokenVariable`, `MaxOutputTokens`, `AdditionalInstructions` (see [Model providers](#model-providers)); the `...Param` references to the [server settings](#server-settings) |
| `ImtAssistPck/AssistantProviderList` | – (the presets as options for the provider selection) |
| `ImtAssistPck/FakeAssistantProvider` | rules: `RuleKeywords`, `RuleActionIds`, `RuleParameters` (JSON), `RuleReplies`, `RuleQuestions`; `DefaultReply`, `ToolResultReply` |
| `ImtAssistPck/AssistantAuditLog` | writes events to the ACF log of the component |
| `ImtAssistGqlPck/AssistantController` | `AssistantService`, `CommandIds` (the `Assistant*` commands), `CommandPermissions`, `PermissionChecker`; `AllowDemoPrincipal`, `DemoUserId`, `DemoTenantId` (demo only, see below) |

Add the controller to the `SlaveGqlHandlers` of the GraphQL servlet and gate the commands
with `CommandPermissions` like any other command of the application.

### GraphQL API

All commands take an `input` object. `messagesJson` is a JSON array of [messages](#messages).

| Command | Input | Output |
|---|---|---|
| `AssistantStartConversation` (mutation) | – | `conversationId`, `messagesJson` |
| `AssistantSendMessage` (mutation) | `conversationId`, `text`, `contextItems` [{`kind`, `label`, `dataJson`}] | `conversationId`, `messagesJson` |
| `AssistantConfirmPreview` (mutation) | `previewId` | `messagesJson` |
| `AssistantRejectPreview` (mutation) | `previewId` | `messagesJson` |
| `AssistantCancelRequest` (mutation) | `conversationId` | `canceled` |
| `AssistantGetHistory` (query) | `conversationId` | `conversationId`, `messagesJson` |
| `AssistantCloseConversation` (mutation) | `conversationId` | `closed` |

User and tenant come from `imtgql::IGqlContext`. A request without an authenticated user is
answered as `Unauthorized`. Context items are limited to 10 per request, 16 KB of JSON data
each, and a kind of letters, digits, `.`, `_`, `-`.

`AllowDemoPrincipal` processes requests without an authenticated user as `DemoUserId` in
`DemoTenantId`. It exists for demo applications without authentication, logs a warning at
startup and must never be enabled in a product.

### QML panel (`imtassistgui`)

```qml
import imtassistgui 1.0

AssistantPanel {
    controller: GqlBasedAssistantController {}
}
```

- `AssistantController`: transport-independent state (messages, busy state, released context,
  decisions on previews). It validates every message (`formatVersion`, known `kind`) before it is
  shown and sends a confirmation or rejection at most once. Keep the controller outside the panel
  to keep the conversation while the panel is hidden or re-created.
- `GqlBasedAssistantController`: the GraphQL transport, one sender per command, so cancel and
  confirm can be sent while a message is being processed.
- `AssistantPanel`: history, input (Enter sends, Escape cancels), processing state with Cancel,
  context chips that can be removed, preview cards with Confirm/Reject, error texts by code.
  `addContext(kind, label, data)` releases application context for the next message.
- `AssistantRepresentationRegistry`: QML components for structured results by `type` and
  `version`. A component implements `setRepresentationData(data)` (returns `false` for invalid
  data) and may emit `selectionHandedOver(kind, label, data)`. `imtassist.selectableList` v1
  (`AssistantSelectableList`) is registered by default. Unknown types or invalid data are shown
  as "The result cannot be displayed".

Applications that use the panel call `DefaultImtCoreQmlInitializer::InitAssistantQml()` and link `imtassistguiqml`.

## Integration

1. **Implement actions** through the existing services and permission checks of the application.

   ```cpp
   class CSetTaskStatusAction: virtual public imtassist::IAssistantAction
   {
       // GetActionId() returns "tasks.setStatus", GetActionKind() returns AK_CHANGE
       // CheckAccess(): permission and object checks, fail closed
       // PrepareChange(): describe target, changes, impact; stateToken = current revision
       // ApplyChange(): check access and stateToken again, then change through the domain service
   };
   ```

2. **Register** actions and UI representation types:

   ```cpp
   imtassist::CAssistantActionRegistry actions;
   actions.RegisterAction(&searchTasksAction);
   actions.RegisterAction(&setTaskStatusAction);

   imtassist::CUiRepresentationRegistry representations;
   representations.RegisterRepresentation(selectableListSchema); // e.g. "imtassist.selectableList", version 1
   ```

3. **Configure and enable** the service explicitly:

   ```cpp
   imtassist::CAssistantService service;
   service.SetActionRegistry(&actions);
   service.SetRepresentationRegistry(&representations);
   service.SetProvider(&providerAdapter);   // credentials stay inside the adapter
   service.SetAuditLog(&auditLog);
   service.SetEnabled(true);                // default: disabled
   ```

4. **Build the request context on the server** from the authenticated session
   (`imtgql::IGqlContext::GetUserId()`, `GetTenantId()`), never from client or model data.
   Add only context the user explicitly released, for example the current selection.

## Model providers

`ImtAssistPck/HttpAssistantProvider` connects the assistant to a model provider. The attribute
`Preset` selects the provider; four API protocols cover all of them:

| `Preset` | Provider | Protocol | Default endpoint | Credential (`ApiKeyVariable`) | Needs |
|---|---|---|---|---|---|
| `openai` | OpenAI | OpenAI Chat Completions | `https://api.openai.com/v1/chat/completions` | API key (Bearer) | `Model` |
| `azure-openai` | Azure OpenAI | OpenAI Chat Completions | `https://{resource}.openai.azure.com/openai/v1/chat/completions` | API key (`api-key` header) | `Resource`, `Model` = deployment |
| `mistral` | Mistral AI | OpenAI Chat Completions | `https://api.mistral.ai/v1/chat/completions` | API key (Bearer) | `Model` |
| `cohere` | Cohere | OpenAI Chat Completions (compatibility API) | `https://api.cohere.ai/compatibility/v1/chat/completions` | API key (Bearer) | `Model` |
| `ionos` | IONOS AI Model Hub | OpenAI Chat Completions | `https://openai.inference.de-txl.ionos.com/v1/chat/completions` | token (Bearer) | `Model` |
| `stackit` | STACKIT AI Model Serving | OpenAI Chat Completions | `https://api.openai-compat.model-serving.eu01.onstackit.cloud/v1/chat/completions` | token (Bearer) | `Model` |
| `aleph-alpha` | Aleph Alpha PhariaAI | OpenAI Chat Completions | – (installation-specific) | token (Bearer) | `Endpoint`, `Model` |
| `openai-compatible` | other compatible server | OpenAI Chat Completions | – | optional token | `Endpoint`, `Model` |
| `ollama` | Ollama, local | OpenAI Chat Completions | `http://localhost:11434/v1/chat/completions` | none | `Model` |
| `vllm` | vLLM, local | OpenAI Chat Completions | `http://localhost:8000/v1/chat/completions` | optional | `Model` |
| `llamacpp` | llama.cpp server, local | OpenAI Chat Completions | `http://localhost:8080/v1/chat/completions` | optional | `Model` |
| `anthropic` | Anthropic | Anthropic Messages | `https://api.anthropic.com/v1/messages` | API key (`x-api-key`) | `Model` |
| `vertex-anthropic` | Claude on Vertex AI | Anthropic Messages | `https://{region}-aiplatform.googleapis.com/v1/projects/{project}/locations/{region}/publishers/anthropic/models/{model}:rawPredict` | OAuth access token (Bearer) | `Region`, `Project`, `Model` |
| `gemini` | Google Gemini API | Gemini generateContent | `https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent` | API key (`x-goog-api-key`) | `Model` |
| `vertex-gemini` | Gemini on Vertex AI | Gemini generateContent | `https://{region}-aiplatform.googleapis.com/v1/projects/{project}/locations/{region}/publishers/google/models/{model}:generateContent` | OAuth access token (Bearer) | `Region`, `Project`, `Model` |
| `bedrock` | Amazon Bedrock | Bedrock Converse | `https://bedrock-runtime.{region}.amazonaws.com/model/{model}/converse` | Bedrock API key (Bearer), or AWS access key via Signature Version 4 | `Region`, `Model` |

The endpoints reflect the provider documentation as of mid-2026. Check them against the current
documentation of the provider when configuring a product and override them with `Endpoint` if
needed (placeholders `{model}`, `{region}`, `{project}`, `{resource}` are allowed). The Vertex AI
templates use regional hosts; for the `global` location set `Endpoint` explicitly.

Example (Mistral, key in the environment variable `IMT_ASSISTANT_API_KEY`):

```xml
<Element Id="AssistantProvider" PackageId="ImtAssistPck" ComponentId="HttpAssistantProvider">
    <Data IsEnabled="true" Flags="0">
        <AttributeInfoMap>
            <AttributeInfo Id="Preset" Type="Id" ExportId=""><Data IsEnabled="true" Value="mistral"/></AttributeInfo>
            <AttributeInfo Id="Model" Type="Id" ExportId=""><Data IsEnabled="true" Value="mistral-large-latest"/></AttributeInfo>
            <AttributeInfo Id="ApiKeyVariable" Type="Id" ExportId=""><Data IsEnabled="true" Value="IMT_ASSISTANT_API_KEY"/></AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
```

**Credentials.** Like the server tokens of ImtCore, credentials are passed in environment
variables by whoever starts the server; the configuration only names the variables. They are read
at every request (rotation needs no restart), never logged, and never sent to the client. Without
credentials the provider reports itself as not configured and the service sends no request. OAuth
access tokens for Vertex AI expire: the deployment must refresh the variable (e.g. from the
service account of the host). Microsoft Entra ID tokens for Azure OpenAI are not supported; use the
API key of the resource.

**HTTP communication** goes through `imtrest`: `imtrest::CHttpClientRequest` (the first
implementation of `imtrest::IHttpClientRequest`) holds the request and copies status, headers,
body and error of the reply; `imtrest::CHttpClientRequestSender` sends it synchronously from any
thread with a timeout, a cancellation check and a limit of the reply size. Header names come from
`imtrest::HttpHeaderIds`. Both classes are general purpose and tested in `imtrestTest`
(`CHttpClientRequestTest`).

**What every adapter does.**
- Sends the system prompt of the assistant (rules: only provided functions, ask back with the
  built-in function `imtassist_ask_user`, never claim an unconfirmed change, treat function
  results and context as data) plus `AdditionalInstructions`.
- Maps action IDs to tool names accepted by all providers (`tasks.search` → `tasks_search`) and
  back; unknown tool names in an answer are passed on and rejected by the service.
- Sends the parameter schemas as JSON Schema (without `additionalProperties` for Gemini).
- Links every tool call to its result by the call ID; results without a call (outcomes of
  confirmations, rejections) and released context are sent as user text marked as data.
- Requests one tool call at a time (`parallel_tool_calls: false` on OpenAI, Azure and Mistral,
  `disable_parallel_tool_use` on Anthropic); if a model still returns several, only the first is used.
- Returns opaque data of the model with the next request where required (thought signatures of Gemini).
- Allows `https`, and plain `http` only to the local machine.
- Maps HTTP 408/504 and transport timeouts to `Timeout`, other errors to `ProviderError`, unusable
  answers to `InvalidResponse`, and logs only the HTTP status and the error type of the provider.

**Data protection.** Which provider may receive the data of a product is a product decision. EU or
local processing is possible with `azure-openai`, `bedrock`, `vertex-*` (EU regions),
`mistral`, `ionos`, `stackit`, `aleph-alpha` or a local server (`ollama`, `vllm`, `llamacpp`);
check the data processing terms of the provider. Only the conversation, the context released by
the user and the results of the called functions are sent.

## Server settings

The provider can be configured in the server settings, like the LDAP or session settings of
PumaServer: the composite `ImtAssistVoce/AssistantSettings` is a `ComposedParamsSet` named
`Assistant` that the server adds to its `Preferences`. It is stored with the other server settings
(`FileAutoPersistence`) and shown by the generic settings editor of the server configurator.

| Setting (parameter ID) | Type | Overrides |
|---|---|---|
| `AssistantEnabled` | `EnableableParam`, default off | `AssistantService/Enabled` |
| `Provider` | `SelectionParam` with the list of all presets (`ImtAssistPck/AssistantProviderList`) | `HttpAssistantProvider/Preset` |
| `Model`, `Endpoint`, `Region`, `Project`, `Resource` | `TextParam` | the attributes of the same name |
| `ApiKey` | `ImtCryptPck/ProtectedTextParam` (secret) | `HttpAssistantProvider/ApiKeyVariable` |
| `MaxOutputTokens` | `TextParam`, default `1024` | `MaxOutputTokens` |
| `AdditionalInstructions` | `TextParam` | `AdditionalInstructions` |

An empty or unset setting falls back to the attribute, so a server without the settings works as
before. The provider reads the settings at every request; a change takes effect without restart.

**The API key** is a secret parameter (`imtcrypt::ISecretTextParam`):
- it is stored protected by `ImtCryptPck/SystemSecretProtector` (on Windows the data protection
  API binds it to the account of the server; elsewhere it is stored as is and the settings file
  must be readable by the server account only, as for the other server secrets). If it cannot be
  protected, it is not stored at all;
- it is never sent to the settings editor: `ImtServerAppPck/SecretTextParamRepresentationController`
  shows a placeholder for a stored key, keeps the key when the placeholder comes back, removes it
  when the field is emptied and replaces it otherwise. The plain `TextParamRepresentationController`
  ignores secret parameters, so without the secret controller the field is not shown (fail closed);
- without a key in the settings, the environment variable of `ApiKeyVariable` is used.

**Integration into a server** (example: the structure of PumaServer):

1. `PumaSettings.acc`: add an element `AssistantSettings` (`ImtAssistVoce/AssistantSettings`), add
   `AssistantSettings/Assistant` to `Preferences/Parameters` with the ID `Assistant` and the name
   `AI assistant`, and export the parameters (`AssistantEnabled`, `Provider`, `Model`, `ApiKey`,
   `Endpoint`, `Region`, `Project`, `Resource`, `MaxOutputTokens`, `AdditionalInstructions`).
2. `PumaSettingsRepresentation.acc`: add an element `SecretTextParamRepresentation`
   (`ImtServerAppPck/SecretTextParamRepresentationController`) to
   `SettingsRepresentation/ParamRepresentationControllers`. The controllers for `EnableableParam`,
   `SelectionParam` and `TextParam` are already there.
3. Server composition (e.g. `PumaServerBase.acc`): reference the settings from the components, e.g.
   `AssistantService/EnabledParam = PumaSettings/AssistantEnabled`,
   `AssistantProvider/ProviderParam = PumaSettings/Provider`, `ModelParam = PumaSettings/Model`,
   `ApiKeyParam = PumaSettings/ApiKey` and so on for `EndpointParam`, `RegionParam`,
   `ProjectParam`, `ResourceParam`, `MaxOutputTokensParam`, `AdditionalInstructionsParam`.

## Data flow of a turn

```
User text ──► SendUserMessage(context, conversationId, text)
                │  disabled / no configured provider → error, no provider call
                ▼
        provider.Complete(released context + conversation + tool list)
                │
     ┌──────────┼───────────────┬──────────────────────────┐
   text   clarification   reading action call       changing action call
     │          │          validate schema           validate schema
     │          │          CheckAccess               CheckAccess
     │          │          Execute                   PrepareChange
     │          │          validate result + UI      store preview (user, tenant, params, TTL)
     │          │          tool result → provider    ──► MK_ACTION_PREVIEW, turn ends
     ▼          ▼          (loop, max. steps)
  MK_ASSISTANT_TEXT / MK_CLARIFICATION / MK_ACTION_RESULT / MK_ERROR

User confirms ──► ConfirmPreview(context, previewId)
                   claim (same user + tenant, pending, not expired)
                   schema version unchanged, CheckAccess, ApplyChange(stateToken)
                   ──► MK_ACTION_EXECUTED with status from the server response
```

## Security boundaries

- **Disabled by default.** Without `SetEnabled(true)` and a configured provider no request is sent to a provider.
- **Identity from the session only.** User and tenant come from the authenticated session. Unknown parameters such as `userId` or `tenantId` proposed by the model are rejected by the schema.
- **Only registered actions.** Unknown actions are rejected; there is no shell, database or generic function execution.
- **Untrusted model and tool data.** Replies, tool results and released context are validated before use and passed to the provider as data, never as instructions. The provider can only propose changes; it cannot confirm them, because confirmation is a separate call from the user.
- **Preview and confirmation.** A preview is bound to user, tenant, action, schema version and parameters, expires after its time to live (default 300 s), and is executed at most once. A repeated confirmation returns the recorded outcome (`alreadyProcessed`). At confirmation the service checks the schema version and `CheckAccess()` again; the action checks the state token. Expired, rejected, stale or foreign previews change nothing.
- **Honest status.** Success is only reported from the server response of the action (`MK_ACTION_EXECUTED` with `succeeded`), never from model text. Cancelling stops processing but does not undo an applied change.
- **No generated UI.** UI representations are a registered type, version and validated data. Unknown types or invalid data are rejected and reported (`InvalidRepresentation`).
- **Logging without content.** Audit events carry correlation ID, user, tenant, action, preview and an outcome code. Prompts, message texts, parameter values, results and secrets are not logged. Error texts for the user contain no confidential details.

## Messages

Every message has `formatVersion` (currently `1`), `id`, `kind`, `conversationId`,
`correlationId` and `createdAt`, plus kind-specific fields (`text`, `options`,
`actionId`, `previewId`, `code`, `data`, `representation`).

| `kind` | Meaning |
|---|---|
| `userText` | Message of the user |
| `assistantText` | Final answer of the turn |
| `clarification` | Question back, optionally with `options` |
| `actionResult` | Result of a reading action, optionally with a `representation` |
| `actionPreview` | Prepared change: `previewId`, summary in `text`, `data.targetId`, `data.targetLabel`, `data.changes`, `data.impact`, `data.expiresAt` |
| `actionExecuted` | Outcome of a confirmed change; `code` is `succeeded`, `failed`, `accessDenied` or `stale` |
| `status` | For example `previewRejected`, `alreadyProcessed` |
| `error` | `code` as listed in `IAssistantService` |

The UI localizes texts by `code`; the `text` of errors is a technical fallback.

## Configuration

| Setting | Default |
|---|---|
| `CAssistantService::SetEnabled` | `false` |
| `CAssistantService::SetProviderTimeout` | 30000 ms |
| `CAssistantService::SetMaxStepsPerTurn` | 5 |
| `CActionPreviewStore::SetTimeToLive` | 300 s |
| Max. pending previews per user and tenant | 20 (the oldest is rejected) |
| Max. conversations per user and tenant | 20 |
| History | in memory, running session only, max. 200 messages per conversation |

## Example (ControlsGallery)

The page **Assistant** of the ControlsGallery shows the MVP flow end to end:

1. "Show open tasks": the reading action `tasks.search` returns the tasks as `imtassist.selectableList`.
2. Select a task and click **Hand over selection**: the task appears as context.
3. "Mark it as done": the changing action `tasks.setStatus` prepares a preview (without a handed
   over task, the assistant asks which task is meant).
4. **Confirm** executes the change; **Reject** changes nothing.

Server configuration: `Partitura/ImtControlsGalleryVoce.arp/ControlsGalleryHandlers.acc`
(`AssistantController`, `AssistantService` with `Enabled`, `AssistantProvider` with rules,
`DemoTaskStore`, `DemoSearchTasksAction`, `DemoSetTaskStatusAction`). The gallery has no
authentication, so it enables `AllowDemoPrincipal`. Users in `ReadOnlyUserIds` of
`DemoSetTaskStatusAction` are refused, to try a denied change.

## Tests

- `Tests/AssistantTest` (QtTest, deterministic `CFakeAssistantProvider`): schema and registry
  validation, enabling and configuration, the end-to-end flow (search, registered list, selection,
  preview, confirmation), the rules of the fake provider, permission, tenant and principal
  boundaries, prompt injection from tool data, confirmation including repetition, rejection,
  expiry and stale previews, provider errors, timeout, invalid replies, cancellation and the audit log.
- `Tests/AssistantTest` (`CAssistantSettingsTest`): provider list, settings overriding attributes,
  changes without restart, enabling by the setting, protected storage of the API key.
- `Tests/AssistantTest` (`CAssistantProviderTest`): all presets, request and response mapping of
  the four protocols, credentials from the environment, tool names, call IDs, error mapping without
  content, the AWS Signature Version 4 test vector and the imtrest-based HTTP transport against a local
  server (response, timeout, cancel, connection refused).
- `Tests/QmlTest/imtassistgui`: `tst_AssistantController` (message validation, single confirmation,
  context, lost answers, cancel, history), `tst_AssistantPanel` (registered and unknown
  representations, selection hand-over, context removal, explicit confirmation, cancel, re-created
  panel) and `tst_GqlBasedAssistantController` (requests and answers through `GqlTestTransport`).

## Known limitations

- Conversations, previews and their outcomes are kept in memory of the server process; a restart
  ends them. A preview whose execution was interrupted by a crash stays in the state "executing".
- A turn is processed synchronously inside the GraphQL request; cancel is a second request. There
  is no streaming of partial answers.
- No streaming of partial answers; one tool call per model answer.
- The provider adapters are tested against recorded request and response formats, not against
  the live services (no network access in the test environment). Test the chosen provider once
  with the example before going productive.
- Vertex AI needs an externally refreshed OAuth token; Azure OpenAI supports API keys only.

## Open decisions before productive use

- Pilot workflow and product owner.
- Approved model provider, hosting and data protection requirements, data processing agreement.
- Whether and how conversations are stored, retained and deleted.
- Readiness of the authentication, tenant and permission checks for the chosen workflow.
