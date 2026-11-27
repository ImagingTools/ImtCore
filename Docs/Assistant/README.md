# AI Assistant (`imtassist`)

Generic, provider-independent AI assistant for ImtCore applications (issue #1206).
Users describe a task in natural language, answer clarifying questions and work on the
results with registered application components. The assistant supports existing
workflows; it never bypasses permissions or business logic.

> **Status:** stage 1 of the implementation: the server-side core library `imtassist`
> with tests. The GraphQL transport, the QML panel and the example application follow
> in later stages (see [Roadmap](#roadmap)).

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

The library depends only on Qt Core and ACF `istd`. It is independent of `imtchat`
(Service Desk chat); no storage or data model is shared.

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

## Tests

`Tests/AssistantTest` (QtTest, deterministic `CFakeAssistantProvider`) covers schema and
registry validation, enabling and configuration, the end-to-end flow (search, registered
list, selection, preview, confirmation), permission, tenant and principal boundaries,
prompt injection from tool data, confirmation including repetition, rejection, expiry and
stale previews, provider errors, timeout, invalid replies, cancellation and the audit log.

## Roadmap

1. **Core library `imtassist`** (this stage).
2. **Transport:** ACF components for service, registries and audit log (bridge to `ilog`); GraphQL API (start conversation, send message, confirm, reject, cancel, history) in a new `imtassistgql` module, context from `IGqlContext`; asynchronous execution so that cancel can arrive while a turn runs.
3. **QML panel** in a new `imtassistgui` module: dockable panel with history, input, processing state, cancel, preview with confirm/reject, visible and removable context, registry of known representation components; keyboard operation and localized texts. `imtchatgui` stays unchanged.
4. **Example application** with one search, one selection and one confirmed change end to end; UI and integration tests with the fake provider.
5. **Provider adapter** for the approved model provider.

## Open decisions before productive use

- Pilot workflow and product owner.
- Approved model provider, hosting and data protection requirements, data processing agreement.
- Whether and how conversations are stored, retained and deleted.
- Readiness of the authentication, tenant and permission checks for the chosen workflow.
