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

## Migration order (Phase 4 of the plan)

1. `imtauthgui` editors (largest inventory, pilot: `UserGeneralEditor.qml`)
2. `imtdocgui` document lifecycle (`DocumentService`, `DocumentDecorator`,
   `UndoRedoManager`) — controller-centric lifecycle, remove view
   registration and `setBlockingUpdateModel` choreography
3. `imtcolgui` / `imtguigql` — move `GqlRequestSender` /
   `GqlBasedDataModelController` users onto the GraphQL bridge; keep the old
   QML types as deprecated facades
4. `CCommandsObserverQmlComp`, `CDocumentServiceController` — replace
   `invokeMethod` name calls with ViewModel properties/signals
