# Product Migration Instructions: Declarative QML Model Binding

Audience: the product repositories consuming ImtCore's QML layer —
**ImagingTools/Lisa**, **ImagingTools/Puma**, **ImagingTools/ProLife**,
**ImagingTools/Agentino**. Execute these steps in each product
repository once the corresponding ImtCore work packages
(Docs/Qml/DeclarativeModelBinding_Phase5_Plan.md) are available in the
ImtCore version the product builds against.

Reference documents (in ImtCore):

- `Docs/Qml/ADR-001_DeclarativeModelBinding.md` — architecture.
- `Docs/Qml/DeclarativeModelBinding_MigrationGuide.md` — before/after
  cookbook, bridge configuration (desktop and GraphQL), checklist.

## Why products must act

ImtCore deprecates the imperative Model-Observer QML stack
(`TQmlModelEditorCompBase`, `TQmlGuiObserverCompWrap`,
`CQmlModelEditorCompBase`, the `ViewBase.qml`
`updateGui()`/`updateModel()`/`blockingUpdateModel` contract, and the
imperative GraphQL plumbing `CGqlModel`/`GqlRequestSender`). These
facades remain functional during the transition and are **removed** in
ImtCore Phase 5 / WP9. Products that still use them will no longer
build afterwards.

The bridge wiring is a **product responsibility**: ImtCore ships the
reusable declarative stack; the product partitura decides per model
whether data comes from an in-process `imod` model (desktop) or GraphQL
(web) by plugging the corresponding bridge. The QML screens are
identical in both cases.

## Step-by-step per product repository

### Step 1 — Inventory

Run in the product repository root:

```
grep -rln "function updateModel"   . --include="*.qml"
grep -rln "blockingUpdateModel"    . --include="*.qml"
grep -rln "setRepresentation\|getRepresentation" . --include="*.qml"
grep -rln "TQmlModelEditorCompBase\|TQmlGuiObserverCompWrap\|CQmlModelEditorCompBase" . --include="*.h" --include="*.cpp"
grep -rln "GqlRequestSender\|GqlBasedDataModelController\|CGqlModel" . --include="*.qml" --include="*.h" --include="*.cpp"
```

Record the results in the product's migration issue. This list is the
scope; everything found must be migrated or consciously retired.

### Step 2 — Update ImtCore and register the QML module

Update the ImtCore dependency to a version containing the declarative
stack (`com.imtcore.imtqml 1.0`: `DataModelController`,
`ObjectViewModel`, `ListViewModel`; `imtgui`: `DeclarativeViewBase`).
No product-side registration code is needed — the types are registered
by `CStaticQmlTypeRegistratorComp`.

### Step 3 — Provide the bridges in the partitura

One `CDataModelBridgeDemultiplexer` per application, with one delegate
per model in its `ModelDelegates` slot:

- **Desktop (in-process `imod` model):** derive one component per model
  from `imtqml::TLocalDataModelBridgeComp<ModelInterface>`, implement
  `ReadObject()` / `WriteObject()` (field name = camelCase SDL name),
  set the `ModelId` attribute (`<module>.<Type>`, e.g. `imtauth.User`),
  reference the data model in the `DataModel` slot.
- **Web/GraphQL:** configure `imtqml::CGqlDataModelBridgeComp`
  instances — `ModelId` (same id as desktop), `GqlClient`,
  `SubscriptionManager` (only if `live` editors are used),
  `QueryCommandId` / `MutationCommandId` / `SubscriptionCommandId`,
  `Fields`.

Keep the `ModelId` values identical across desktop and web so the QML
screens stay transport-independent.

### Step 4 — Migrate the product QML editors

For every file found in Step 1, apply the migration-guide cookbook:

1. Root the editor on `DeclarativeViewBase` with an inline
   `DataModelController { modelId: "..." }`.
2. Replace `updateGui()` copying with property bindings
   (`text: editor.model ? editor.model.field : ""`).
3. Replace `updateModel()` copying with ViewModel setters in the
   control handlers (`onEditingFinished: editor.model.field = text`).
4. Replace collection rebuilds with a `ListView`/`Repeater` bound to
   `editor.model.<listField>` and edits via `append` / `insert` /
   `remove` / `setProperty`.
5. Delete all `blockingUpdateModel` / update-guard logic — echo
   suppression is central in the ViewModel; any surviving blocker is a
   migration error.
6. Form dialogs: `submit()` / `revert()` on OK/Cancel, `enabled:
   editor.isDirty` on Apply. Parameter panels: `autoSubmit: true`.
   Multi-view live sync: `live: true`.
7. QML style rules: only own controls from `imtcontrols` (no Quick
   Controls), no `prop: { ... }` constructs, at most ternary operators,
   access other items strictly via `id`, follow the structure of the
   existing ImtCore screens.

### Step 5 — Retire product C++ observers

Replace product subclasses of `TQmlModelEditorCompBase` /
`TQmlGuiObserverCompWrap` with the Step-3 bridges. The typical mapping:

- `OnUpdate()` JSON serialization → `ReadObject()` returning a
  `QVariantMap`.
- `representationChanged()` write-back → `WriteObject()` (runs inside
  one `BeginChanges`/`EndChanges` transaction automatically).
- `UpdateBlocker` — delete; not needed.

### Step 6 — Verify

- Per editor: the migration-guide checklist (no `updateModel`, no
  blockers, bindings only, works against both bridges where relevant).
- Rerun the Step-1 greps — all must return empty.
- Manual regression: open/edit/save each migrated screen; for document
  based products additionally undo/redo, multi-view sync (`live`) and
  cancel/revert flows.
- Component tests: organize like in ImagingTools/Acf.

### Step 7 — Report readiness

Comment on the product's migration sub-issue (see
`DeclarativeModelBinding_Issues.md`) that the product no longer uses
any deprecated ImtCore facade. ImtCore WP9 (removal of the facades) is
executed only after all four products have reported readiness.

## Product-specific notes

- **Lisa, Puma, ProLife, Agentino** — the concrete inventory differs
  per product and must be produced by Step 1 in each repository. Any
  product that embeds the `imtdocgui` document lifecycle must schedule
  its migration after ImtCore WP6 (document controller / undo-redo) and
  validate undo/redo behavior explicitly.
- Products using the web/GraphQL path must wire
  `CGqlDataModelBridgeComp` (Step 3) before migrating the affected
  screens; desktop-only products can skip the GraphQL bridge entirely.
