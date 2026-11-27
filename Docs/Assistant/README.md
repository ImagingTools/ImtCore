# AI Assistant (`imtassist`)

Generic, provider-independent AI assistant for ImtCore applications (issue #1206).
Users describe a task in natural language, answer clarifying questions and work on the
results with registered application components. The assistant supports existing
workflows; it never bypasses permissions or business logic.

> **Status:** MVP implemented: core library `imtassist`, GraphQL API `imtassistgql`,
> QML panel `imtassistgui` and an example in the ControlsGallery. A production model
> provider is not part of the MVP (see [Open decisions](#open-decisions-before-productive-use)).

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
| `CAssistantMessage` | Typed, versioned message exchanged with the UI. |
| `IAssistantAuditLog` | Receiver of technical events and action decisions without content. |
| `CAssistantRequestContext` | User, tenant, correlation ID and context released by the user. |

The classes without the `Comp` suffix depend only on Qt Core and ACF `istd`. The module is
independent of `imtchat` (Service Desk chat); no storage, data model or QML component is shared,
and `imtchatgui` is unchanged.

### Components (`ImtAssistPck`, `ImtAssistGqlPck`)

| Component | Attributes |
|---|---|
| `ImtAssistPck/AssistantService` | `Enabled` (default `false`), `Provider`, `ActionRegistry`, `RepresentationRegistry`, `AuditLog`, `ProviderTimeout`, `MaxStepsPerTurn`, `PreviewTimeToLive` |
| `ImtAssistPck/AssistantActionRegistry` | `Actions`: the action components of the application |
| `ImtAssistPck/UiRepresentationRegistry` | `RegisterStandardRepresentations` (default `true`), `SchemaProviders` (`IUiRepresentationSchemaProvider`) |
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
- `Tests/QmlTest/imtassistgui`: `tst_AssistantController` (message validation, single confirmation,
  context, lost answers, cancel, history), `tst_AssistantPanel` (registered and unknown
  representations, selection hand-over, context removal, explicit confirmation, cancel, re-created
  panel) and `tst_GqlBasedAssistantController` (requests and answers through `GqlTestTransport`).

## Known limitations

- Conversations, previews and their outcomes are kept in memory of the server process; a restart
  ends them. A preview whose execution was interrupted by a crash stays in the state "executing".
- A turn is processed synchronously inside the GraphQL request; cancel is a second request. There
  is no streaming of partial answers.
- The only provider is the fake provider; an adapter for a real provider follows after the
  provider decision.

## Open decisions before productive use

- Pilot workflow and product owner.
- Approved model provider, hosting and data protection requirements, data processing agreement.
- Whether and how conversations are stored, retained and deleted.
- Readiness of the authentication, tenant and permission checks for the chosen workflow.
