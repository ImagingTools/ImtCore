# ADR-001: Declarative Model Binding for the QML Layer (Desktop + Web)

Status: Accepted
Date: 2026-07-11

## Context

The QML UI layer of ImtCore historically ported the imperative Model-Observer
pattern from the widget world (ImagingTools/Acf) almost verbatim into QML:

- C++ observers (`imtqml::TQmlModelEditorCompBase`, `TQmlGuiObserverCompWrap`,
  `CCommandsObserverQmlComp`, `CDocumentServiceController`) push data into
  QQuickItems imperatively via `QMetaObject::invokeMethod` with name-based
  function contracts (`setRepresentation`, `getRepresentation`,
  `doUpdateGui`, `setBlockingUpdateModel`, ...).
- QML editors implement `updateGui()` / `updateModel()` pairs and copy data
  field-by-field between the model and the input controls, guarded by manual
  update blockers (`blockingUpdateModel`, `UpdateBlocker`).
- The web path (`CGqlModel` / `GqlRequestSender.qml`) pulls values out of a
  `CTreeItemModel` imperatively in `onStateChanged` handlers, and
  `GraphQLRequest.js` assembles GraphQL queries via string concatenation.

This bypasses the core mechanics QML is built on: property bindings, role
models (`QAbstractItemModel`) and automatic dependency tracking. The result is
fragile string contracts, duplicated state (model + JSON representation +
input fields), manual feedback-loop blockers in every editor, and no reuse
between the desktop (in-process `imod` models) and the web (GraphQL).

## Decision

A QML editor knows only a QObject-based ViewModel with Q_PROPERTYs and role
models. Where the data comes from (in-process `imod` model on the desktop or
GraphQL in the web) is decided exclusively by the bridge layer behind it.

The layered architecture (bottom-up):

1. **Domain model (unchanged):** `istd::IChangeable` / `imod::IModel` remain
   the source of truth on the desktop; SDL models / GraphQL on the server.
   No changes to Acf.

2. **Adapter / ViewModel layer (`imtqml::CObjectViewModel`):** a
   `QQmlPropertyMap`-based ViewModel. Every model field appears as a regular
   QML property with automatic change notification. Two flows are strictly
   separated:
   - *Source updates* enter only through `SetSourceValues()`; they reset the
     dirty state and never emit `valueEdited` — this is the single, central
     place where the update-feedback loop is broken. QML never contains
     blocker logic.
   - *User edits* are plain property writes from QML; they mark the ViewModel
     dirty and emit `valueEdited` for write-back.

3. **Transport / bridge layer (`imtqml::IDataModelBridge` +
   `CDataModelBridgeDemultiplexer`):** the single gateway. The interface
   supports `GetModel` (fetch), `SetModel` (store) and
   `SubscribeModel`/`UnsubscribeModel` (live updates, pushed on the GUI
   thread).
   - Desktop bridge: `TLocalDataModelBridgeComp<ModelInterface>` binds a
     local `imod` model. The `imod::IObserver` mechanics stay fully
     encapsulated inside the bridge; write-back happens in a single
     `BeginChanges`/`EndChanges` transaction (`istd::CChangeNotifier`).
   - GraphQL bridge (`CGqlDataModelBridgeComp`): implements the same
     interface with Query (get), Mutation (set) and GraphQL Subscription
     over WebSocket (live) on top of `imtclientgql` (async client +
     subscription manager). All callbacks are marshalled to the GUI
     thread; payloads are decoded generically from the response `data`
     object.

4. **Controller (`imtqml::CDataModelController`):** the single QML entry
   point: `modelId`, `parameters`, `viewModel`, `isLoading`, `error`,
   `isDirty`, `live`, `autoSubmit`, `submit()`, `revert()`. Two edit modes:
   - *Form mode* (default): edits accumulate in the ViewModel; `submit()`
     writes them back, `revert()` discards them.
   - *Live mode* (`autoSubmit: true`): each edit is written back
     immediately.

5. **QML layer:** editors derive from `DeclarativeViewBase` and bind input
   controls directly to ViewModel properties. Display via property binding;
   writing via the ViewModel setter in the control handler
   (`onEditingFinished`, `onActivated`). No `updateGui()`/`updateModel()`
   pairs, no `blockingUpdateModel`, no JSON representations. Only own
   controls from `imtcontrols`, simple binding style (at most ternary
   operators), access strictly via `id`.

## Naming and structure conventions

- ViewModels: `C<Domain>ViewModel` (C++, generated or hand-written) exposed
  to QML as controller `viewModel` property; ViewModel property names use the
  camelCase field names of the SDL / GraphQL schema (no `m_` prefixes in the
  declarative world).
- Bridges: `C<Domain>DataModelBridgeComp` per model, derived from
  `CDataModelBridgeBase` or `TLocalDataModelBridgeComp`, routed by `modelId`
  through `CDataModelBridgeDemultiplexer`.
- `modelId` values: `<module>.<Type>` (e.g. `imtauth.User`), identical for
  desktop and web so QML screens are transport-independent.

## Consequences

- Desktop and web editors share the same QML code; only the plugged bridge
  differs.
- Echo suppression exists in exactly one place (the ViewModel). Any blocker
  logic remaining in QML is a migration error and must be rejected in review.
- The legacy imperative bases (`TQmlModelEditorCompBase`,
  `TQmlGuiObserverCompWrap`, `CQmlModelEditorCompBase`, `ViewBase.qml`
  hooks) are deprecated but remain functional until downstream products are
  migrated (facade strategy — both worlds coexist for several releases).
- GraphQL-/WebSocket-pushed updates must be delivered to the ViewModel on the
  GUI thread (existing `IDataModelBridge` contract).
- Undo/redo semantics of `imtdocgui` must be mapped onto the transactional
  write-back of the bridge before the old choreography is removed.

## Follow-up work (tracked, not part of the foundation)

- Extend the `imtsdlgenqml` code generator to emit typed ViewModels
  (properties, list roles, dirty/validation state) from SDL definitions.
- ~~Implement the GraphQL bridge (Query/Mutation/Subscription) on top of
  `imtclientgql`~~ — done: `imtqml::CGqlDataModelBridgeComp` (async client
  + subscription manager, GUI-thread callback contract). Generating
  queries from SDL metadata instead of the attribute-configured field
  lists remains open.
- Unify list adapters with roles on top of `TListModelBase` /
  `TSdlAbstractListModel`.
- Migrate editors module by module (see the migration guide) and finally
  remove the deprecated imperative bases.
