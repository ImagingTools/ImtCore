# Inventory: Imperative Model-Observer Patterns in the QML Layer

Status snapshot for the declarative model-binding migration
(see ADR-001_DeclarativeModelBinding.md). Regenerate the lists with the grep
commands given in each section.

## 1. C++ bases implementing the imperative pattern (deprecated)

| Class | File | Mechanism |
|---|---|---|
| `imtqml::TQmlModelEditorCompBase<T>` | `Include/imtqml/TQmlModelEditorCompBase.h` | `OnUpdate()` serializes the model to JSON and pushes it via `invokeMethod(root, "setRepresentation", ...)`; reverse path `representationChanged()` + `getRepresentation()`; `UpdateBlocker` echo suppression |
| `imtqml::CQmlModelEditorCompBase` | `Include/imtqml/CQmlModelEditorCompBase.h` | Non-template base of the `setRepresentation`/`representationChanged` contract |
| `imtqml::TQmlGuiObserverCompWrap` | `Include/imtqml/TQmlGuiObserverCompWrap.h` | Per-editor `QmlAdapter` QObject pushing data into the QQuickItem |

No concrete subclasses exist inside ImtCore itself — the users live in
downstream product repositories (a cross-repository search is required before
removal; until then the bases stay as deprecated facades).

## 2. C++ code driving QML by name-based `invokeMethod` calls

Regenerate: `grep -rln "invokeMethod" Include --include="*.cpp" --include="*.h"`

- `Include/imtqml/TQmlModelEditorCompBase.h` — `setRepresentation` / `getRepresentation`
- `Include/imtqml/CDocumentServiceController.cpp` — `updateRepresentationForAllViews`, `setBlockingUpdateModel`, `doUpdateGui`
- `Include/imtqml/CDocumentServiceBridge.cpp` — document-service choreography
- `Include/imtqml/CCommandsObserverQmlComp.cpp` — `onCommandActivated`
- `Include/imtauthgui/CAdministrationObserverQmlComp.cpp` — administration login/update callbacks

## 3. QML files implementing `function updateModel()` (44 files)

Regenerate: `grep -rln "function updateModel" Qml --include="*.qml"`

imtauthgui (14): AdministrationView, ContractView, CrossOrgGrantView,
MessageView, RelationshipView, RoleView, TenantEditor, TenantGeneralPage,
TenantPermissionsPage, TenantSimpleCollectionPage, UserGeneralEditor,
UserGroupView, UserManagementProvider, UserView

imtcolgui (7): CollectionDataProvider, CollectionStructure,
CollectionStructureBase, CollectionStructureBaseGqlModels,
CollectionStructureCommandsDelegateBase, CollectionStructureDataProvider,
CollectionViewBaseGqlModels

imtgui (7): Application/ApplicationInfoProvider, Application/PagesManager,
Application/PagesProvider, Application/ThumbnailDecorator, Panels/ButtonPanel,
Settings/SettingsProvider, View/ViewBase

imtguigql (5): CollectionRepresentation, ComboBoxGql, ComboBoxGqlSimple,
PopupMenuDialogGqlLegacy, WebSocketPortProvider

imtlicgui (4): FeatureEditor, LicenseEditor, LicensesProvider, ProductView

others (7): imtchatgui/ConversationEditor, imtcontrols/Models/SortFilterProxyModel,
imtcontrols/Views/TableHeaderParamComp, imtdeskgui/TicketEditor,
imtdocgui/CommandsProvider, imtdocgui/DocumentService,
imtgeogui/AddressTreeView

## 4. QML files with manual update-blocker logic

Regenerate: `grep -rln "blockingUpdateModel" Qml --include="*.qml"`

- `Qml/imtgui/View/ViewBase.qml` (deprecated contract)
- `Qml/imtdocgui/DocumentDecorator.qml`, `DocumentService.qml`,
  `DocumentServiceBase.qml`, `UndoRedoManager.qml`
- `Qml/imtauthgui/UserView.qml`
- `Qml/imtdeskgui/TicketEditor.qml`

## 5. Imperative web/GraphQL data flow

- `Qml/web/GraphQLRequest.js` — string-concatenated query building
- `Qml/imtguigql/GqlRequestSender.qml`, `GqlBasedDataModelController.qml` and
  the other `GqlBased*.qml` services — pull values out of the
  `CTreeItemModel` response in `onStateChanged` handlers
- `Include/imtqml/CGqlModel.*` — HTTP transport + tree-model response

Replacement available: `imtqml::CGqlDataModelBridgeComp` implements
`IDataModelBridge` over `imtclientgql` (query = GetModel, mutation =
SetModel, WebSocket subscription = SubscribeModel), so declarative editors
reach GraphQL through `CDataModelBridgeDemultiplexer` without any of the
above QML plumbing. The legacy types stay as deprecated facades until the
`imtcolgui` / `imtguigql` consumers are migrated.

## Migration order (Phase 4 of the plan)

1. `imtauthgui` editors (largest inventory, pilot: `UserGeneralEditor.qml`).
   Declarative facades added alongside the imperative editors (facade
   coexistence — the imperative editors stay until Phase 5):
   - `UserGeneralDeclarativeEditor.qml` — scalar user fields
     (`username`, `name`, `email`) of `imtauth.User`.
   - `TenantGeneralDeclarativePage.qml` — scalar tenant fields
     (`name`, `description`, `isActive`) of `imtauth.Tenant`.

   **Scalar-only editors are the migratable set today.** The remaining
   `imtauthgui` editors are blocked and are intentionally not migrated in
   this phase:
   - *List/collection editors* — `UserView` roles/groups/system-info,
     `RoleView` (parentRoles/permissions), `UserGroupView`,
     `CrossOrgGrantView` (roleIds), `RelationshipView` (roles/scope),
     `TenantPermissionsPage` (tenantPermissions). These need the
     ViewModel *list adapters with roles* follow-up (tracked separately
     in ADR-001, "Follow-up work"); `CObjectViewModel` currently exposes
     scalar `QVariant` properties only, so a faithful multi-select /
     table binding is not yet expressible.
   - *Non-field views* — `AdministrationView`, `UserManagementProvider`
     (page/permission orchestration, not field copying),
     `ContractView` / `MessageView` / `TenantSimpleCollectionPage`
     (already empty `updateGui`/`updateModel` stubs).
   - *Special flows* — the password-change flow of `UserGeneralEditor`
     uses a dedicated `ChangePassword` mutation via
     `AuthorizationController`, not model write-back, and stays imperative.
   - *Concrete bridge wiring* (`ModelId "imtauth.User"` /
     `"imtauth.Tenant"` on a `TLocalDataModelBridgeComp` subclass or a
     configured `CGqlDataModelBridgeComp`, plugged into
     `CDataModelBridgeDemultiplexer`) is composed in the **downstream
     product partitura**, not in ImtCore — ImtCore has no concrete
     imperative editor users either (see §1). ImtCore ships the reusable
     declarative facades; products switch their screens over and provide
     the bridge.

2. `imtdocgui` document lifecycle (`DocumentService`, `DocumentDecorator`,
   `UndoRedoManager`) — controller-centric lifecycle, remove view
   registration and `setBlockingUpdateModel` choreography. **Not migrated:**
   this is a replacement refactor of deeply interlocked view-registration /
   undo-redo choreography; under the facade strategy it is removed only in
   Phase 5 once the declarative controller lifecycle owns undo/redo.
3. `imtcolgui` / `imtguigql` — move `GqlRequestSender` /
   `GqlBasedDataModelController` users onto the GraphQL bridge; keep the old
   QML types as deprecated facades. **Not migrated:** blocked on the same
   list-adapter follow-up (collection views are list-centric) and on the
   downstream partitura wiring for `CGqlDataModelBridgeComp`.
4. `CCommandsObserverQmlComp`, `CDocumentServiceController` — replace
   `invokeMethod` name calls with ViewModel properties/signals. **Not
   migrated:** paired with item 2; the imperative `invokeMethod` contract
   stays until the `imtdocgui` lifecycle is switched over in Phase 5.

