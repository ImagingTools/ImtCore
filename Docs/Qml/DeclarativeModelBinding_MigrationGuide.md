# Migration Guide: Imperative QML Editors → Declarative Model Binding

This guide shows how to migrate an editor from the imperative
`updateGui()` / `updateModel()` contract (widget-world Model-Observer pattern)
to the declarative binding stack introduced by
ADR-001_DeclarativeModelBinding.md.

## The stack

- `ObjectViewModel` (`imtqml::CObjectViewModel`) — QML-facing ViewModel; every
  model field is a QML property with change notification, dirty tracking and
  central echo suppression.
- `DataModelController` (`imtqml::CDataModelController`) — the only QML entry
  point: `modelId`, `parameters`, `viewModel`, `isLoading`, `error`,
  `isDirty`, `live`, `autoSubmit`, `getModel()`, `submit()`, `revert()`.
- `IDataModelBridge` — transport behind the controller. Desktop:
  `TLocalDataModelBridgeComp<ModelInterface>` (in-process `imod` model,
  observer fully encapsulated). Web: GraphQL bridge (query/mutation/
  subscription). The QML editor is identical in both worlds.
- `DeclarativeViewBase.qml` (`Qml/imtgui/View`) — slim view base exposing
  `controller`, `model` (the ViewModel), `isLoading`, `error`, `isDirty`,
  `readOnly`.

## Rules

1. Display = property binding. Never copy values into controls.
2. Write = ViewModel setter in the control handler (`onEditingFinished`,
   `onActivated`, `onCheckedChanged`).
3. No blocker logic in QML. `blockingUpdateModel`, update guards and
   `updateGui`/`updateModel` pairs must not survive the migration — echo
   suppression lives centrally in the ViewModel.
4. Only own controls from `imtcontrols`; simple constructs (at most ternary
   operators); access other items strictly via `id`.

## Before / after cookbook

### Text field

Before (imperative, field-by-field copying):

```qml
// inside a ViewBase-derived editor
function updateGui(){
	usernameInput_.text = container.userData.m_username
}

function updateModel(){
	container.userData.m_username = usernameInput_.text
}
```

After (declarative):

```qml
DeclarativeViewBase {
	id: editor

	controller: DataModelController {
		modelId: "imtauth.User"
	}

	TextInput {
		id: usernameInput_

		text: editor.model ? editor.model.username : ""
		enabled: !editor.readOnly
		onEditingFinished: editor.model.username = text
	}
}
```

### ComboBox / selection

```qml
ComboBox {
	id: roleSelector_

	currentId: editor.model ? editor.model.roleId : ""
	onActivated: editor.model.roleId = currentId
}
```

### Checkbox

```qml
CheckBox {
	id: enabledCheck_

	checked: editor.model ? editor.model.isEnabled : false
	onClicked: editor.model.isEnabled = checked
}
```

### Form mode vs. live mode

Form editors (OK/Cancel dialogs) keep the default and call `submit()` /
`revert()`:

```qml
Button {
	id: applyButton_

	enabled: editor.isDirty
	onClicked: editor.submit()
}

Button {
	id: cancelButton_

	onClicked: editor.revert()
}
```

Live editors (immediate write-through, e.g. parameter panels) enable
`autoSubmit`:

```qml
controller: DataModelController {
	modelId: "imtauth.User"
	autoSubmit: true
}
```

Live *updates* from other views / the server are enabled with `live: true`;
the controller subscribes at the bridge and pushed updates land in the
ViewModel echo-free.

### Loading / error states

```qml
BusyIndicator {
	id: busyIndicator_

	visible: editor.isLoading
}

Text {
	id: errorLabel_

	visible: editor.error !== ""
	text: editor.error
}
```

## Providing the desktop bridge (C++)

Per model, derive from `TLocalDataModelBridgeComp<ModelInterface>` and
implement the two mapping methods:

```cpp
class CUserDataModelBridgeComp: public imtqml::TLocalDataModelBridgeComp<imtauth::IUser>
{
public:
	typedef imtqml::TLocalDataModelBridgeComp<imtauth::IUser> BaseClass;

	I_BEGIN_COMPONENT(CUserDataModelBridgeComp);
	I_END_COMPONENT;

protected:
	virtual QVariantMap ReadObject(const imtauth::IUser& user) const override;
	virtual bool WriteObject(const QVariantMap& values, imtauth::IUser& user) const override;
};
```

Configure the `ModelId` attribute (e.g. `imtauth.User`), reference the data
model in the `DataModel` slot and plug the bridge into the `ModelDelegates`
slot of `CDataModelBridgeDemultiplexer`. Write-back runs inside a single
`BeginChanges`/`EndChanges` transaction; model changes are pushed to all
`live` controllers automatically.

## Providing the GraphQL bridge (web)

For the web transport no subclassing is needed — configure a
`imtqml::CGqlDataModelBridgeComp` instance per model in the partitura:

- `ModelId`: same identifier as on the desktop (e.g. `imtauth.User`), so
  the QML screens stay transport-independent.
- `GqlClient`: reference to the `imtclientgql::IAsyncGqlClient`
  (e.g. `CAsyncApiClientComp`).
- `SubscriptionManager` (optional): reference to the
  `imtclientgql::IGqlSubscriptionManager` for `live` controllers.
- `QueryCommandId` / `MutationCommandId` / `SubscriptionCommandId`: the
  GraphQL operations serving the model (e.g. `GetUser` / `SetUser` /
  `UserChanged`).
- `Fields`: payload fields to select; nested objects via dotted paths
  (e.g. `address.city`). `MutationFields` defaults to `success`.

Plug the bridge into the same `ModelDelegates` slot of
`CDataModelBridgeDemultiplexer`. The controller `parameters` map becomes
the GraphQL `input` argument object; ViewModel edits are merged into the
mutation input on `submit()`. All callbacks are delivered on the GUI
thread — the editor QML is byte-for-byte identical to the desktop
variant.

## Checklist per migrated editor

- [ ] No `function updateGui()` / `function updateModel()` left
- [ ] No `blockingUpdateModel` / update-guard logic left
- [ ] No JSON `setRepresentation` / `getRepresentation` contract left
- [ ] All displays are bindings; all writes go through ViewModel setters
- [ ] Editor works unchanged against the desktop bridge and the GraphQL
      bridge (contract parity)
