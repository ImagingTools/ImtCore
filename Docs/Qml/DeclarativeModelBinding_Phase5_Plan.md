# Phase 5 Plan: Complete Declarative Migration of the QML Layer

Status: Planned
Date: 2026-07-12
Prerequisites: ADR-001_DeclarativeModelBinding.md (foundation, phases 0–2),
DeclarativeModelBinding_Inventory.md (call-site inventory),
DeclarativeModelBinding_MigrationGuide.md (cookbook),
DeclarativeModelBinding_ProductMigration.md (downstream products).

## Goal

Every QML component in ImtCore follows the declarative principles of
ADR-001: display = property binding, write = ViewModel setter, no
`updateGui()`/`updateModel()` pairs, no `blockingUpdateModel` guards, no
name-based `invokeMethod` contracts, no imperative GraphQL plumbing in QML.
At the end of Phase 5 the deprecated imperative bases are removed.

Regenerate the work inventory at any time:

```
grep -rln "function updateModel" Qml --include="*.qml"
grep -rln "blockingUpdateModel"  Qml --include="*.qml"
grep -rln "invokeMethod" Include --include="*.cpp" --include="*.h"
```

## Work packages

Each package is independently executable in a separate Copilot session
(see the issue breakdown in DeclarativeModelBinding_Issues.md). The
packages are ordered so that shared infrastructure lands before its
consumers and the removal of the deprecated bases comes last.

### WP1 — `imtauthgui`: remaining scalar and collection editors

Migrate the remaining imperative editors to `DeclarativeViewBase` +
`DataModelController` (+ `CListViewModel` adapters for list fields),
following the three existing pilots
(`UserGeneralDeclarativeEditor.qml`, `TenantGeneralDeclarativePage.qml`,
`TenantPermissionsDeclarativePage.qml`):

- `UserGeneralEditor.qml`, `TenantGeneralPage.qml`,
  `TenantPermissionsPage.qml` — fold the pilots back: the declarative
  variant becomes the only variant, the imperative file is removed and
  call sites (e.g. `TenantEditor.qml`, `UserView.qml`) are repointed.
- `UserView.qml` — roles / groups / system-info lists via
  `CListViewModel`; keep the password-change flow on the dedicated
  `ChangePassword` mutation (`AuthorizationController`), it is not model
  write-back.
- `RoleView.qml` (parentRoles / permissions), `UserGroupView.qml`,
  `CrossOrgGrantView.qml` (roleIds), `RelationshipView.qml`
  (roles / scope) — collection editors on `CListViewModel`.
- `TenantEditor.qml` — page host; drop `updateModel` forwarding once the
  child pages are declarative.
- `ContractView.qml`, `MessageView.qml`,
  `TenantSimpleCollectionPage.qml` — empty stubs; delete the dead
  `updateGui`/`updateModel` functions.
- `AdministrationView.qml`, `UserManagementProvider.qml`,
  `AuthorizationPage.qml`, `AuthorizationController.qml`,
  `SuperuserPasswordPage.qml` — orchestration, not field copying:
  remove the `updateModel` protocol hooks, keep command / navigation
  logic; login and password flows stay on their dedicated mutations.
- C++: retire `CAdministrationObserverQmlComp` `invokeMethod` callbacks
  in favour of controller signals/properties.

Definition of done: no `function updateModel` and no
`blockingUpdateModel` left under `Qml/imtauthgui`; migrated editors pass
the checklist of the migration guide.

### WP2 — `imtlicgui`: license editors

`FeatureEditor.qml`, `LicenseEditor.qml`, `LicensesProvider.qml`,
`ProductView.qml` — same pattern as WP1; scalar fields via ViewModel
properties (`modelId` e.g. `imtlic.License`, `imtlic.Product`), feature /
option lists via `CListViewModel`.

### WP3 — small modules: `imtdeskgui`, `imtchatgui`, `imtgeogui`

- `imtdeskgui/TicketEditor.qml` — form editor incl. blocker removal.
- `imtchatgui/ConversationEditor.qml` — scalar + message list.
- `imtgeogui/AddressTreeView.qml` — address fields; tree display stays
  on its existing role model, only the editor part moves to the
  ViewModel.

### WP4 — `imtcolgui`: collection framework

`CollectionDataProvider.qml`, `CollectionStructure.qml`,
`CollectionStructureBase.qml`, `CollectionStructureBaseGqlModels.qml`,
`CollectionStructureCommandsDelegateBase.qml`,
`CollectionStructureDataProvider.qml`, `CollectionViewBaseGqlModels.qml`,
`CollectionView.qml`.

These are framework bases, not leaf editors: replace the
`updateModel`-protocol between provider / structure / view with a
controller-owned `CListViewModel` per collection; commands mutate through
the adapter slots. GraphQL-backed variants move onto
`CGqlDataModelBridgeComp` (the `modelId` selects the bridge, QML stays
transport-independent). Downstream products consume these bases, so the
old API surface stays as a deprecated facade until WP9.

### WP5 — `imtguigql`: retire the imperative GraphQL plumbing

- `ComboBoxGql.qml`, `ComboBoxGqlSimple.qml`,
  `CollectionRepresentation.qml`, `PopupMenuDialogGqlLegacy.qml`,
  `WebSocketPortProvider.qml` — bind to `DataModelController` /
  `CListViewModel` backed by `CGqlDataModelBridgeComp`.
- `GqlRequestSender.qml`, `GqlBasedDataModelController.qml` and the
  other `GqlBased*.qml` services plus `Qml/web/GraphQLRequest.js`
  (string-concatenated queries) and `CGqlModel.*` become deprecated
  facades; new code must not use them.

### WP6 — `imtdocgui`: document lifecycle and undo/redo

The largest replacement refactor (inventory §2/§4):

- `DocumentService.qml`, `DocumentServiceBase.qml`,
  `DocumentDecorator.qml`, `UndoRedoManager.qml`,
  `CommandsProvider.qml` — remove view registration,
  `setBlockingUpdateModel` and `doUpdateGui` choreography; the document
  controller owns the lifecycle, views bind to its ViewModel.
- Undo/redo maps onto the transactional write-back of the bridge
  (one `istd::CChangeNotifier` transaction per submit ⇒ one undo step).
- C++: `CDocumentServiceController` and `CCommandsObserverQmlComp` stop
  driving QML via `invokeMethod`; they expose properties / signals or a
  ViewModel instead. `CDocumentServiceBridge` choreography is folded
  into an `IDataModelBridge` implementation.

Gate: no behavioral regression in document open/save/undo/redo of the
downstream products (manual test protocol in the product issues).

### WP7 — `imtgui` / `imtcontrols` infrastructure providers

`ApplicationInfoProvider.qml`, `PagesManager.qml`, `PagesProvider.qml`,
`ThumbnailDecorator.qml`, `ButtonPanel.qml`, `SettingsProvider.qml`,
`SearchCompLegacy.qml`, `CommandsDecorator.qml`,
`imtcontrols/Models/SortFilterProxyModel.qml`,
`imtcontrols/Views/TableHeaderParamComp.qml`.

These implement the `updateModel` protocol only because `ViewBase.qml`
demands it. Remove the protocol hooks; where they actually carry state
(settings, page state) expose it through a controller ViewModel.

### WP8 — `imtsdlgenqml`: typed ViewModel code generation (missing step)

Extend the SDL code generator to emit per-model typed ViewModels
(Q_PROPERTY per field, list roles, validation metadata) and the
field lists consumed by `CGqlDataModelBridgeComp`, replacing the
attribute-configured `Fields` lists and hand-written
`ReadObject`/`WriteObject` mappings of `TLocalDataModelBridgeComp`
subclasses. This removes the remaining hand-maintained mapping code and
is a prerequisite for migrating products at scale.

### WP9 — Removal of the deprecated imperative bases (final)

Preconditions: WP1–WP7 done in ImtCore **and** all four products
(Lisa, Puma, ProLife, Agentino) migrated
(DeclarativeModelBinding_ProductMigration.md).

Remove:

- `Include/imtqml/TQmlModelEditorCompBase.h`,
  `CQmlModelEditorCompBase.*`, `TQmlGuiObserverCompWrap.h`
- `Qml/imtgui/View/ViewBase.qml` `updateGui`/`updateModel`/
  `blockingUpdateModel` contract (ViewBase itself may stay as a plain
  layout base)
- `CGqlModel.*`, `GqlRequestSender.qml`, `GqlBased*.qml`,
  `Qml/web/GraphQLRequest.js`
- `invokeMethod` name contracts in `CDocumentServiceController`,
  `CCommandsObserverQmlComp`, `CDocumentServiceBridge`

Then delete inventory sections 1–5 and close the ADR follow-up list.

## Missing steps identified beyond the original plan

1. **`imtsdlgenqml` ViewModel codegen** (WP8) — was a follow-up in the
   ADR, becomes a full work package because product-scale migration is
   impractical with hand-written mappings.
2. **Parameter-driven re-subscription** — `CDataModelController` now
   re-subscribes when `parameters` change (review fix in this phase);
   bridges must treat `parameters` as part of the subscription key.
3. **Undo/redo mapping** (WP6) — explicitly gated; the old choreography
   may not be removed before the mapping is proven in a product.
4. **Validation state** — the ViewModel carries dirty state but no
   field-level validation results yet; needed by the form editors of
   WP1/WP2 (`errorText` / `showErrorWhenInvalid` today is purely
   client-side). Add `SetFieldErrors()` to `CObjectViewModel` as part
   of WP1.
5. **Cross-repo verification before WP9** — a search for subclasses of
   the deprecated bases across all product repositories is mandatory
   before removal (inventory §1).

## Sequencing and parallelism

- WP1, WP2, WP3 are independent of each other (parallelizable).
- WP4 must precede WP5 consumers inside `imtcolgui`/`imtguigql`.
- WP6 is independent but should land after WP1 (pilot experience).
- WP8 can run in parallel with everything; its output simplifies WP1–WP5
  but is not a hard prerequisite.
- WP9 is strictly last and additionally gated on the four product
  migrations.

## Definition of done (Phase 5)

- All grep inventories above return empty (except deliberately kept
  facades until WP9, then empty entirely).
- Every migrated editor satisfies the migration-guide checklist.
- Products build and run against ImtCore with only declarative editors.
