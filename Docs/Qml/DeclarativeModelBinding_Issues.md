# GitHub Issue Package: Declarative QML Migration (Phase 5 + Products)

Ready-to-file issue texts for the Phase 5 migration. File the parent
issue in `ImagingTools/ImtCore`, then the sub-issues (WP1–WP9 in
ImtCore, one product issue per product repository) and link them as
sub-issues of the parent. Each sub-issue is scoped so that Copilot can
process it in a separate session.

Example filing commands (run by a user with credentials):

```
gh issue create -R ImagingTools/ImtCore --title "<title>" --body-file <file>
```

---

## Parent issue (ImagingTools/ImtCore)

**Title:** Phase 5: Complete declarative migration of the QML layer (ADR-001)

**Body:**

All QML components must follow the declarative model-binding principles
of `Docs/Qml/ADR-001_DeclarativeModelBinding.md`: display = property
binding, write = ViewModel setter, no `updateGui()`/`updateModel()`
pairs, no `blockingUpdateModel` guards, no name-based `invokeMethod`
contracts, no imperative GraphQL plumbing in QML.

Plan: `Docs/Qml/DeclarativeModelBinding_Phase5_Plan.md`
Cookbook: `Docs/Qml/DeclarativeModelBinding_MigrationGuide.md`
Products: `Docs/Qml/DeclarativeModelBinding_ProductMigration.md`

Sub-issues (each independently workable by Copilot):

- [ ] WP1 — imtauthgui: migrate remaining editors
- [ ] WP2 — imtlicgui: migrate license editors
- [ ] WP3 — imtdeskgui / imtchatgui / imtgeogui: migrate small-module editors
- [ ] WP4 — imtcolgui: declarative collection framework
- [ ] WP5 — imtguigql: retire imperative GraphQL plumbing
- [ ] WP6 — imtdocgui: document lifecycle & undo/redo
- [ ] WP7 — imtgui/imtcontrols: infrastructure providers
- [ ] WP8 — imtsdlgenqml: typed ViewModel code generation
- [ ] Product migration: ImagingTools/Lisa
- [ ] Product migration: ImagingTools/Puma
- [ ] Product migration: ImagingTools/ProLife
- [ ] Product migration: ImagingTools/Agentino
- [ ] WP9 — remove deprecated imperative bases (last; gated on all of the above)

Ordering: WP1–WP3 parallel; WP4 before WP5; WP6 after WP1; WP8 anytime;
product issues after the ImtCore WPs they depend on; WP9 strictly last.

---

## Sub-issue WP1 (ImagingTools/ImtCore)

**Title:** WP1: Migrate remaining imtauthgui editors to declarative model binding

**Body:**

Per `Docs/Qml/DeclarativeModelBinding_Phase5_Plan.md` (WP1) and the
cookbook `Docs/Qml/DeclarativeModelBinding_MigrationGuide.md`, migrate
all remaining imperative editors under `Qml/imtauthgui` to
`DeclarativeViewBase` + `DataModelController` (+ `ListViewModel` for
list fields). Follow the pilots `UserGeneralDeclarativeEditor.qml`,
`TenantGeneralDeclarativePage.qml`, `TenantPermissionsDeclarativePage.qml`.

Scope:
- Fold the three pilots back (declarative variant becomes the only
  variant; imperative files removed; call sites repointed).
- `UserView.qml` (roles/groups/system-info lists), `RoleView.qml`,
  `UserGroupView.qml`, `CrossOrgGrantView.qml`, `RelationshipView.qml`
  — collection editors via `ListViewModel`.
- `TenantEditor.qml` — remove `updateModel` forwarding.
- `ContractView.qml`, `MessageView.qml`, `TenantSimpleCollectionPage.qml`
  — delete dead `updateGui`/`updateModel` stubs.
- `AdministrationView.qml`, `UserManagementProvider.qml`,
  `AuthorizationPage.qml`, `AuthorizationController.qml`,
  `SuperuserPasswordPage.qml` — remove protocol hooks; login /
  password flows stay on their dedicated mutations.
- Add field-level validation state to `CObjectViewModel`
  (`SetFieldErrors()`), needed by the form editors.
- C++: replace `CAdministrationObserverQmlComp` `invokeMethod`
  callbacks with controller signals/properties.

Done when: `grep -rln "function updateModel" Qml/imtauthgui` and
`grep -rln "blockingUpdateModel" Qml/imtauthgui` are empty and every
migrated editor passes the migration-guide checklist.

---

## Sub-issue WP2 (ImagingTools/ImtCore)

**Title:** WP2: Migrate imtlicgui license editors to declarative model binding

**Body:**

Per Phase 5 plan WP2: migrate `FeatureEditor.qml`, `LicenseEditor.qml`,
`LicensesProvider.qml`, `ProductView.qml` in `Qml/imtlicgui` to
`DeclarativeViewBase` + `DataModelController`; scalar fields via
ViewModel properties, feature/option lists via `ListViewModel`.
Cookbook: `Docs/Qml/DeclarativeModelBinding_MigrationGuide.md`.

Done when the WP1-style greps are empty under `Qml/imtlicgui` and the
checklist passes.

---

## Sub-issue WP3 (ImagingTools/ImtCore)

**Title:** WP3: Migrate imtdeskgui/imtchatgui/imtgeogui editors to declarative model binding

**Body:**

Per Phase 5 plan WP3: migrate `Qml/imtdeskgui/TicketEditor.qml`
(incl. removal of its `blockingUpdateModel` guard),
`Qml/imtchatgui/ConversationEditor.qml` (scalar + message list) and
`Qml/imtgeogui/AddressTreeView.qml` (editor part only; tree display
stays on its role model). Cookbook:
`Docs/Qml/DeclarativeModelBinding_MigrationGuide.md`.

Done when the WP1-style greps are empty under the three modules.

---

## Sub-issue WP4 (ImagingTools/ImtCore)

**Title:** WP4: Declarative collection framework in imtcolgui

**Body:**

Per Phase 5 plan WP4: replace the `updateModel` protocol between
provider / structure / view in `Qml/imtcolgui`
(`CollectionDataProvider`, `CollectionStructure*`,
`CollectionViewBaseGqlModels`, `CollectionView`) with a
controller-owned `ListViewModel` per collection; commands mutate
through the adapter slots. GraphQL variants move onto
`CGqlDataModelBridgeComp`. Keep the old API surface as a deprecated
facade until WP9 (products consume these bases).

---

## Sub-issue WP5 (ImagingTools/ImtCore)

**Title:** WP5: Retire imperative GraphQL plumbing in imtguigql

**Body:**

Per Phase 5 plan WP5 (depends on WP4): migrate `ComboBoxGql.qml`,
`ComboBoxGqlSimple.qml`, `CollectionRepresentation.qml`,
`PopupMenuDialogGqlLegacy.qml`, `WebSocketPortProvider.qml` to
`DataModelController` / `ListViewModel` over `CGqlDataModelBridgeComp`.
Mark `GqlRequestSender.qml`, `GqlBased*.qml`,
`Qml/web/GraphQLRequest.js`, `CGqlModel.*` as deprecated facades; new
code must not use them.

---

## Sub-issue WP6 (ImagingTools/ImtCore)

**Title:** WP6: Declarative document lifecycle and undo/redo in imtdocgui

**Body:**

Per Phase 5 plan WP6: remove the view-registration /
`setBlockingUpdateModel` / `doUpdateGui` choreography from
`DocumentService.qml`, `DocumentServiceBase.qml`,
`DocumentDecorator.qml`, `UndoRedoManager.qml`, `CommandsProvider.qml`;
the document controller owns the lifecycle and views bind to its
ViewModel. Map undo/redo onto the transactional bridge write-back (one
transaction per submit ⇒ one undo step). C++:
`CDocumentServiceController`, `CCommandsObserverQmlComp`,
`CDocumentServiceBridge` stop driving QML via `invokeMethod`.

Gate: no regression in document open/save/undo/redo (validated in a
product before the old choreography is deleted — see WP9).

---

## Sub-issue WP7 (ImagingTools/ImtCore)

**Title:** WP7: Remove updateModel protocol from imtgui/imtcontrols infrastructure providers

**Body:**

Per Phase 5 plan WP7: remove the `updateModel` protocol hooks from
`ApplicationInfoProvider.qml`, `PagesManager.qml`, `PagesProvider.qml`,
`ThumbnailDecorator.qml`, `ButtonPanel.qml`, `SettingsProvider.qml`,
`SearchCompLegacy.qml`, `CommandsDecorator.qml`,
`imtcontrols/Models/SortFilterProxyModel.qml`,
`imtcontrols/Views/TableHeaderParamComp.qml`. Where the hooks carry
real state (settings, page state), expose it through a controller
ViewModel instead.

---

## Sub-issue WP8 (ImagingTools/ImtCore)

**Title:** WP8: imtsdlgenqml — generate typed ViewModels from SDL

**Body:**

Per Phase 5 plan WP8: extend the `imtsdlgenqml` code generator to emit
per-model typed ViewModels (Q_PROPERTY per field, list roles,
validation metadata) plus the field lists consumed by
`CGqlDataModelBridgeComp` and the `ReadObject`/`WriteObject` mappings
for `TLocalDataModelBridgeComp` subclasses. Goal: no hand-maintained
mapping code per model.

---

## Product sub-issues (one per repository)

File the following issue — identically structured — in each of
**ImagingTools/Lisa**, **ImagingTools/Puma**, **ImagingTools/ProLife**,
**ImagingTools/Agentino**:

**Title:** Migrate QML screens to ImtCore declarative model binding (ADR-001)

**Body:**

ImtCore deprecates the imperative QML Model-Observer stack
(`TQmlModelEditorCompBase`, `TQmlGuiObserverCompWrap`,
`CQmlModelEditorCompBase`, the `ViewBase.qml`
`updateGui()`/`updateModel()`/`blockingUpdateModel` contract, and the
imperative GraphQL plumbing). The facades are removed in ImtCore
Phase 5 / WP9 — this product must be migrated before that.

Follow the step-by-step instructions in ImtCore
`Docs/Qml/DeclarativeModelBinding_ProductMigration.md`:

- [ ] Step 1 — run the inventory greps; attach results to this issue
- [ ] Step 2 — update the ImtCore dependency to the declarative stack
- [ ] Step 3 — provide the bridges in the partitura
      (`TLocalDataModelBridgeComp` subclasses per desktop model and/or
      configured `CGqlDataModelBridgeComp` per web model, plugged into
      `CDataModelBridgeDemultiplexer`)
- [ ] Step 4 — migrate each QML editor per the cookbook
      (`Docs/Qml/DeclarativeModelBinding_MigrationGuide.md`)
- [ ] Step 5 — retire product C++ observers
      (subclasses of the deprecated bases → bridges)
- [ ] Step 6 — verify: greps empty, per-editor checklist, manual
      regression (incl. undo/redo and live sync where applicable)
- [ ] Step 7 — report readiness on this issue

Note: screens embedding the `imtdocgui` document lifecycle must wait
for ImtCore WP6 and explicitly validate undo/redo.

---

## Sub-issue WP9 (ImagingTools/ImtCore) — file last

**Title:** WP9: Remove deprecated imperative QML bases

**Body:**

Preconditions: WP1–WP7 closed **and** readiness reported by all four
product migration issues (Lisa, Puma, ProLife, Agentino). Verify with a
cross-repository search that no subclass/user of the deprecated bases
remains.

Remove: `TQmlModelEditorCompBase.h`, `CQmlModelEditorCompBase.*`,
`TQmlGuiObserverCompWrap.h`, the `ViewBase.qml` imperative contract,
`CGqlModel.*`, `GqlRequestSender.qml`, `GqlBased*.qml`,
`Qml/web/GraphQLRequest.js`, and the `invokeMethod` name contracts in
`CDocumentServiceController` / `CCommandsObserverQmlComp` /
`CDocumentServiceBridge`. Update
`Docs/Qml/DeclarativeModelBinding_Inventory.md` (sections 1–5 empty)
and close the ADR-001 follow-up list.
