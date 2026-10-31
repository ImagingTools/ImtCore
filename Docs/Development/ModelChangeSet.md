# Model changeSet

`BaseClass.modelChanged(changeSet)` tells what changed in a data model, down to a field of an element of a nested list.
This page defines the format of `changeSet`, what is reported and what is not, and how to read it with `ChangeSetHelper`.

The format is the same in Qt and on the web (JQML v3).
`Tests/QmlTest/imtcontrols/tst_ModelChangeSet.qml` and `Tools/JQML/v3/tests/ModelChangeSet_001` check this.

## Record format

`changeSet` is an array of records, in the order the changes were made:

```js
{
	path: ["permissions", 4, "direct"],  // where: steps from the object you listen to
	operation: "set",                    // what: "set", "insert" or "remove"
	name: "m_direct",                    // the changed property, as before
	sender: <object>                     // the object whose property changed, or the list for insert/remove
}
```

### path

- A string step is the **JSON key of the SDL schema**, the same key the server reads. It is not the `m_` property name. Example: `TimeRange` declares `Begin`, so the path is `timeRange.Begin`.
- A number step is the index of an element in a list, **at the moment of the change**. Records are ordered, so after `remove permissions[0]` the next record calls the old element 2 `permissions[1]`.
- The path starts at the object whose `modelChanged` you connected to. The same change is `steps[0].id` for an `AssignmentPath` and `permissions[0].sources[1].steps[0].id` for the role that holds it.
- `null` means the place is unknown. Treat it as "anything may have changed".

### operation

| Operation | Meaning | Path points to |
|---|---|---|
| `set` | The value was replaced. If it is an object or a list, anything below it may have changed. | the property, or a list element for `swapItems` |
| `insert` | An element was inserted. | the new element: `[list key, index]` |
| `remove` | An element was removed. | the removed position: `[list key, index]` |

A record at a path covers everything below it: `set permissions` means any permission may have changed.

## What is reported

| Action | Records |
|---|---|
| `role.m_name = "A"` | `set name` |
| `role.m_permissions.get(1).item.m_direct = true` | `set permissions[1].direct` |
| a field four levels deep | `set permissions[0].sources[1].steps[2].name` |
| `appendElement`, `addElement`, `insertElement(i, x)` | `insert permissions[i]` |
| `removeElement(i)` | `remove permissions[i]` |
| `swapItems(i, j)` | `set permissions[i]`, `set permissions[j]` |
| `emplacePermissions()`, `removePermissions()`, assigning another object or list | `set permissions` |
| assigning a new array to a primitive array property | `set filterOperations` |
| `fromObject` / `createFromJson` on a filled model | `set` for every changed field; a list filled again is not reported (a load, not an edit), a list the model did not have is `set` |

Changes made between `beginChanges()` and `endChanges()` come in **one** `modelChanged`, in their order. A child transaction inside the owner's transaction joins the owner's one.

## What is not reported

- Assigning the value a property already has.
- Changes through the plain `ListModel` API: `append`, `insert`, `remove`, `clear`, `set`, `setProperty`. Use the `BaseModel` functions above.
- Changing an array in place, e.g. `m_filterOperations.push(x)`. Assign a new array instead.
- Changes while `enableNotifications` of the object or of its owner is `false`.
- Changes of an object before it is attached to its owner. For example, fields set on a new element before `insertElement`: the `insert` record covers them.

`modelChanged([])`, called by hand, still means "something changed, place unknown". Several editors use it to force an update.

## Reading a changeSet

`ChangeSetHelper` is a singleton of `imtcontrols`. Patterns are paths written as text:

- `"permissions[*].direct"`;
- `"groupFilters[0].logicalOperation"`;
- `""` for the whole model.

`*` and `[*]` match any key or index. Unknown paths and an empty `changeSet` are treated as "may have changed anything", so no helper can tell you that nothing relevant changed when it does not know.

| Function | Returns |
|---|---|
| `isAffected(changeSet, pattern)` | true when the value at `pattern` may have changed: a change at it, inside it or above it, or an insert/remove before its index in the same list (the element moves) |
| `isOnlyWithin(changeSet, pattern)` | true when every change lies inside `pattern`; false for an empty or unknown `changeSet` |
| `filter(changeSet, pattern)` | the records that may have changed `pattern` |
| `getRootKeys(changeSet)` | changed keys of the model itself, e.g. `["name", "permissions"]`; `null` when a path is unknown |
| `getChangedIndexes(changeSet, listPattern)` | indexes of the changed elements of a list; `null` when the whole list may have changed |
| `getValue(model, path)` | the current value at a path or pattern without wildcards |
| `pathToString(path)` | `"permissions[4].direct"`, `"?"` for an unknown path |
| `changeAffects(change, pattern)`, `changeIsWithin(change, pattern)` | the same checks for one record |

Example: a role editor sends only the permissions when nothing else was edited.

```qml
function updateDocumentFromRepresentation(changeSet){
	if (ChangeSetHelper.isOnlyWithin(changeSet, "permissions")){
		sendPermissions()
	}
	else{
		sendRole()
	}
}
```

## Sending a changeSet as JSON Patch

`ChangeSetHelper.toJsonPatch(changeSet, model)` turns the changes into a standard [JSON Patch](https://www.rfc-editor.org/rfc/rfc6902) with [JSON Pointer](https://www.rfc-editor.org/rfc/rfc6901) paths. Pass all the changeSets since the last send, concatenated in order, and the model they came from. Applied to the JSON of the model as it was before them, the patch gives `model.toJson()` as it is now.

```json
[
	{"op": "add", "path": "/permissions/0", "value": {"id": "first", "direct": false}},
	{"op": "remove", "path": "/permissions/3"},
	{"op": "add", "path": "/permissions/2/name", "value": "One"}
]
```

How the patch is built:

- **Inserts and removes come first.** They keep their order and their indexes, which are right when applied one after another. An `add` carries the element as it is now.
- **Changed values come after them.** Their paths are moved to the current indexes, and the values are the current ones. A recorded index is not enough: in "change `permissions[1]`, then insert at 0", the changed element is `permissions[2]` now.
- **An object member is written with `add`.** It replaces the member, or creates it if it was not there. A member that is gone from the JSON now gets `remove`. A list element written in place (`swapItems`) gets `replace`.
- **Covered records are dropped.** A replaced list or object, or an inserted element, is written whole with its current value, so the changes inside it are not repeated. Changes inside an element that was removed later are dropped too.
- **Unknown path or empty changeSet.** The patch is `[{"op": "replace", "path": "", "value": <the whole model>}]`.
- **`null`** is returned when the model cannot be serialized: `toJson()` returns `""` for a required array that is missing.

`toJsonPointer(path)` gives the pointer of one path, `"/a~1b/c~0d"` for the keys `a/b` and `c~d`.

A member that is created and removed again within the patched changes ends as `remove` of a member that the receiver may not have. RFC 6902 calls this an error, so let the receiver ignore a `remove` of a missing object member.

The tests apply every patch with a strict RFC 6902 implementation and compare the result with the model. Besides fixed cases, there are 40 random runs: 30 edits each, in one transaction or one by one, over nested lists, swaps and replaced lists.

## Implementation

| Runtime | Files |
|---|---|
| Qt | `Qml/imtcontrols/Base/BaseClass.qml`, `BaseModel.qml`, `ChangeSetBuilder.js` |
| Web, JQML v3 | `Tools/JQML/v3/QtQml/BaseClass.js`, `BaseModel.js`. Rebuild `dist/main.js` with `npm run build-core-dev` after a change. |
| Web, JQML v2 | not implemented: v2 still sends the old `{name, sender}` records |

Every object forwards the changes of a child to its owner with the step of the child in front. The owner finds that step by looking for the child among its properties and lists. In Qt this code lives in `ChangeSetBuilder.js` and not in `BaseClass`. Each function declared in `BaseClass` makes every SDL object slower to create: seven extra functions cost 8% of `fromJSON` time in a Debug build.
