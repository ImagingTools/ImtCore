pragma Singleton
import QtQuick 2.12

/*
	Reads the changeSet of BaseClass.modelChanged, see Docs/Development/ModelChangeSet.md.
	A DocumentRepresentationController gets it in updateDocumentFromRepresentation(changeSet).

	A changeSet lists what was edited in a model since the last update, in the order of the edits:
		[
			{"path": ["name"], "operation": "set", "name": "m_name", "sender": <object>},
			{"path": ["permissions", 4, "direct"], "operation": "set", ...},
			{"path": ["permissions", 2], "operation": "insert", ...},
			{"path": ["permissions", 0], "operation": "remove", ...}
		]
	- path: JSON keys of the SDL fields ("permissions", not "m_permissions") and list indexes, from the model root;
	  an index is the one at the time of the edit, later inserts and removes may move it (see mapPathForward());
	- operation: "set" a value, "insert" or "remove" a list element at the index;
	- name: the QML property ("m_name"); sender: the edited object, valid only while the changeSet is handled.
	An empty or undefined changeSet tells nothing about what changed: treat it as a change of the whole model.

	Patterns are paths written as text:
		"permissions"              the field permissions and everything inside it (its elements and their fields)
		"permissions[4].direct"    the field direct of the 5th element of permissions
		"permissions[*].direct"    the field direct of any element of permissions ("*" matches any key or index)
		""                         the whole model
	An array of path steps (["permissions", 4]) is accepted as a pattern as well.

	The examples use RoleData (imtauthRolesSdl, Sdl/imtauth/1.0/Roles.sdl):
		type RoleData { id, name, description, roleId, productId, parentRoles: [Assignment], permissions: [Assignment], isDefault, isGuest }
		type Assignment { id, name, direct, sources: [AssignmentPath] }
		type AssignmentPath { steps: [AssignmentStep] }
		type AssignmentStep { kind, id, name }
	and TimeFilter (imtbaseComplexCollectionFilterSdl): { timeRange: TimeRange { Begin, End }, timeUnit, ... }.

	What an edit reports (pathToString() of each change):
		role.m_name = "Admins"                                         set name
		role.m_permissions.get(1).item.m_direct = true                 set permissions[1].direct
		role.m_permissions.get(0).item.m_sources.get(1).item
			.m_steps.get(2).item.m_name = "Renamed"                    set permissions[0].sources[1].steps[2].name
		role.m_permissions.appendElement(assignment)                   insert permissions[3]   (the index it got)
		role.m_permissions.insertElement(0, assignment)                insert permissions[0]
		role.m_permissions.removeElement(1)                            remove permissions[1]
		role.m_permissions.swapItems(0, 2)                             set permissions[0], set permissions[2]
		role.emplacePermissions() / role.removePermissions()           set permissions          (the whole list)
		role.createFromJson(...) on a filled role                      set name, ...            (the changed fields; a list filled again is not reported)
		timeFilter.m_timeRange.m_begin = "2026-01-01"                  set timeRange.Begin      (the JSON key of the SDL: "Begin")
		timeFilter.emplaceTimeRange()                                  set timeRange            (the whole object)
		model.modelChanged([])                                         []                       (unknown: anything may have changed)
	The same value assigned again reports nothing. Edits between beginChanges() and endChanges() come in one changeSet,
	in their order: [set name, remove permissions[2], insert permissions[2], set permissions[1].direct].
	An object removed from the model reports "set ?" (no path) if it is edited afterwards.

	Catching an edit in updateDocumentFromRepresentation(changeSet) of a role:
		// Only the name was edited
		if (ChangeSetHelper.isOnlyWithin(changeSet, "name")){ ... }

		// Which fields of the role were edited: ["name", "permissions"]; null when unknown
		let keys = ChangeSetHelper.getRootKeys(changeSet)

		// Only the permissions were edited (any element, any field, inserts and removes): send only them
		// (UpdateRolePermissions is an example command, the role service has none)
		if (ChangeSetHelper.isOnlyWithin(changeSet, "permissions")){
			updatePermissionsInput.m_roleId = representationModel.m_id
			updatePermissionsInput.m_permissions = representationModel.m_permissions
			updatePermissionsRequest.send(updatePermissionsInput)
			return
		}

		// Only the "direct" flags of permissions were switched, no element was added or removed
		if (ChangeSetHelper.isOnlyWithin(changeSet, "permissions[*].direct")){
			let indexes = ChangeSetHelper.getChangedIndexes(changeSet, "permissions")   // e.g. [1, 4]
			for (let i = 0; i < indexes.length; ++i){
				let permission = representationModel.m_permissions.get(indexes[i]).item
				// permission.m_id, permission.m_direct
			}
		}

		// The default flag was among the edits (with others or alone)
		if (ChangeSetHelper.isAffected(changeSet, "isDefault")){ ... }

		// Every change of the permission at index 4, with its sources and steps
		let changes = ChangeSetHelper.filter(changeSet, "permissions[4]")

	Catching an edit in a view, e.g. a permissions table of a role editor:
		Connections {
			target: roleEditor
			function onModelDataChanged(view, model, changeSet){
				if (ChangeSetHelper.isAffected(changeSet, "permissions")){
					permissionsTable.updateGui()   // true also for an unknown changeSet
				}
			}
		}

	Send one request per update: DocumentDecorator expects one answer for it. When the changeSet holds
	more than a request can carry (permissions and the name, say), send the whole model.
	An index in a path is the one at the time of the edit: after "insert permissions[0]" the element of an earlier
	"set permissions[1].direct" is at index 2, see mapPathForward(); getChangedIndexes() returns the indexes as reported.

	All changes for a server that takes RFC 6902 patches:
		let patch = ChangeSetHelper.toJsonPatch(changeSet, representationModel)
		// [{"op": "add", "path": "/permissions/2", "value": {"id": "p3", ...}}, {"op": "add", "path": "/permissions/1/direct", "value": true}]
*/
QtObject {
	id: root

	// Path as text, e.g. for a log:
	//   pathToString(["permissions", 4, "direct"]) -> "permissions[4].direct"
	//   pathToString([]) -> "", pathToString(undefined) -> "?" (unknown path)
	function pathToString(path){
		if (!path){
			return "?"
		}

		let text = ""
		for (let i = 0; i < path.length; ++i){
			if (typeof path[i] === "number"){
				text += "[" + path[i] + "]"
			}
			else{
				text += text === "" ? path[i] : "." + path[i]
			}
		}

		return text
	}

	// Pattern text to path steps; "*" and "[*]" match any key or index. An array is taken as it is.
	//   parsePattern("permissions[*].direct") -> ["permissions", "*", "direct"]
	//   parsePattern("") -> [] (the whole model)
	function parsePattern(pattern){
		if (pattern === undefined || pattern === null || pattern === ""){
			return []
		}

		if (typeof pattern !== "string"){
			return pattern
		}

		let steps = []
		let parts = pattern.replace(/\[/g, ".[").split(".")
		for (let i = 0; i < parts.length; ++i){
			let part = parts[i]
			if (part === ""){
				continue
			}

			if (part[0] === "["){
				let index = part.substring(1, part.length - 1)
				steps.push(index === "*" ? "*" : Number(index))
			}
			else{
				steps.push(part)
			}
		}

		return steps
	}

	function stepMatches(patternStep, pathStep){
		return patternStep === "*" || patternStep === pathStep
	}

	// True when one change may have changed the value at pattern: at it, inside it, or above it.
	// For pattern "permissions[4]": true for ["permissions", 4, "direct"] (inside), ["permissions"] (above),
	// a change without a path (unknown); false for ["name"] and ["permissions", 5].
	function changeAffects(change, pattern){
		let steps = parsePattern(pattern)
		let path = change.path
		if (!path){
			return true
		}

		let count = Math.min(steps.length, path.length)
		for (let i = 0; i < count; ++i){
			if (!stepMatches(steps[i], path[i])){
				return false
			}
		}

		return true
	}

	// True when one change lies at pattern or inside it, never above it.
	// For pattern "permissions": true for ["permissions"] and ["permissions", 4, "direct"];
	// false for [] (the whole model was replaced), ["name"] and a change without a path.
	function changeIsWithin(change, pattern){
		let steps = parsePattern(pattern)
		let path = change.path
		if (!path || path.length < steps.length){
			return false
		}

		for (let i = 0; i < steps.length; ++i){
			if (!stepMatches(steps[i], path[i])){
				return false
			}
		}

		return true
	}

	// True when the changeSet may have changed the value at pattern: use it to decide whether to refresh something.
	// An empty changeSet tells nothing, so it may have: true.
	//   isAffected([{"path": ["permissions", 4, "direct"], ...}], "permissions") -> true
	//   isAffected([{"path": ["name"], ...}], "permissions") -> false
	function isAffected(changeSet, pattern){
		if (!changeSet || changeSet.length === 0){
			return true
		}

		for (let i = 0; i < changeSet.length; ++i){
			if (changeAffects(changeSet[i], pattern)){
				return true
			}
		}

		return false
	}

	// True when every change lies at pattern or inside it: use it to decide whether a part of the model is enough to send.
	// An empty changeSet tells nothing, so not only: false.
	//   isOnlyWithin([{"path": ["permissions", 3, "sources", 0, "steps", 1, "name"], ...}], "permissions") -> true
	//   isOnlyWithin([{"path": ["permissions", 1, "direct"], ...}, {"path": ["name"], ...}], "permissions") -> false
	function isOnlyWithin(changeSet, pattern){
		if (!changeSet || changeSet.length === 0){
			return false
		}

		for (let i = 0; i < changeSet.length; ++i){
			if (!changeIsWithin(changeSet[i], pattern)){
				return false
			}
		}

		return true
	}

	// The changes that may have changed the value at pattern, in their order.
	// An empty result is ambiguous (nothing there, or an unknown changeSet came in): ask isAffected() first.
	//   filter(changeSet, "permissions") -> the changes of permissions, of its elements and of the whole model
	function filter(changeSet, pattern){
		let result = []
		if (!changeSet){
			return result
		}

		for (let i = 0; i < changeSet.length; ++i){
			if (changeAffects(changeSet[i], pattern)){
				result.push(changeSet[i])
			}
		}

		return result
	}

	// Changed fields of the model itself, in the order of their first change; null when unknown
	// (an empty changeSet, a change without a path or of the whole model).
	//   getRootKeys([{"path": ["name"]}, {"path": ["permissions", 4, "direct"]}, {"path": ["name"]}]) -> ["name", "permissions"]
	function getRootKeys(changeSet){
		if (!changeSet || changeSet.length === 0){
			return null
		}

		let keys = []
		for (let i = 0; i < changeSet.length; ++i){
			let path = changeSet[i].path
			if (!path || path.length === 0){
				return null
			}

			if (keys.indexOf(path[0]) < 0){
				keys.push(path[0])
			}
		}

		return keys
	}

	// Indexes of the changed elements of the list at listPattern, in the order of the changes, each once;
	// null when the whole list may have changed (an empty changeSet, the list itself, the model or an unknown path).
	// The indexes are those at the time of each change: after inserts and removes use mapPathForward().
	//   getChangedIndexes([{"path": ["permissions", 4, "direct"]}, {"path": ["permissions", 2], "operation": "insert"}], "permissions") -> [4, 2]
	//   getChangedIndexes([{"path": ["permissions"]}], "permissions") -> null
	//   getChangedIndexes([{"path": ["name"]}], "permissions") -> []
	function getChangedIndexes(changeSet, listPattern){
		if (!changeSet || changeSet.length === 0){
			return null
		}

		let steps = parsePattern(listPattern)
		let indexes = []
		for (let i = 0; i < changeSet.length; ++i){
			let change = changeSet[i]
			if (!changeAffects(change, steps)){
				continue
			}

			if (!change.path || change.path.length <= steps.length){
				return null
			}

			let index = change.path[steps.length]
			if (indexes.indexOf(index) < 0){
				indexes.push(index)
			}
		}

		return indexes
	}

	// Value at path in a model (SDL object): JSON keys are read as m_ properties, indexes as list elements;
	// undefined when the path does not exist.
	//   getValue(role, ["permissions", 4, "direct"]) -> role.m_permissions.get(4).item.m_direct
	//   getValue(role, "name") -> role.m_name
	function getValue(model, path){
		let steps = parsePattern(path)
		let value = model
		for (let i = 0; i < steps.length; ++i){
			if (value === null || value === undefined){
				return undefined
			}

			if (typeof steps[i] === "number"){
				if (value.get === undefined || steps[i] < 0 || steps[i] >= value.count){
					return undefined
				}

				value = value.get(steps[i]).item
			}
			else{
				value = value["m_" + steps[i][0].toLowerCase() + steps[i].slice(1)]
			}
		}

		return value
	}

	// JSON Pointer (RFC 6901) of a path; "~" and "/" in keys are escaped.
	//   toJsonPointer(["permissions", 4, "direct"]) -> "/permissions/4/direct"
	//   toJsonPointer([]) -> "" (the whole model)
	function toJsonPointer(path){
		let pointer = ""
		for (let i = 0; i < path.length; ++i){
			pointer += "/" + String(path[i]).replace(/~/g, "~0").replace(/\//g, "~1")
		}

		return pointer
	}

	// True when path starts with prefix: isPathPrefix(["permissions"], ["permissions", 4]) -> true
	function isPathPrefix(prefix, path){
		if (prefix.length > path.length){
			return false
		}

		for (let i = 0; i < prefix.length; ++i){
			if (prefix[i] !== path[i]){
				return false
			}
		}

		return true
	}

	// Path of the change at fromIndex in the model as it is now: the later inserts and removes of changeSet move its index;
	// null when its element was removed later.
	//   changeSet: [set ["permissions", 4, "direct"], insert ["permissions", 1], remove ["permissions", 0]]
	//   mapPathForward(changeSet, ["permissions", 4, "direct"], 0) -> ["permissions", 4, "direct"] (+1 by the insert, -1 by the remove)
	//   mapPathForward(changeSet, ["permissions", 0, "direct"], 0) -> null (the element was removed)
	function mapPathForward(changeSet, path, fromIndex){
		let result = path.slice()
		for (let i = fromIndex + 1; i < changeSet.length; ++i){
			let change = changeSet[i]
			if (change.operation !== "insert" && change.operation !== "remove"){
				continue
			}

			let listLength = change.path.length - 1
			if (result.length <= listLength || !isPathPrefix(change.path.slice(0, listLength), result)){
				continue
			}

			let position = result[listLength]
			let index = change.path[listLength]
			if (change.operation === "insert"){
				if (index <= position){
					result[listLength] = position + 1
				}
			}
			else if (index === position){
				return null
			}
			else if (index < position){
				result[listLength] = position - 1
			}
		}

		return result
	}

	// Value at path in the JSON of a model: {"exists": bool, "value": ...}
	//   getJsonValue({"permissions": [{"direct": true}]}, ["permissions", 0, "direct"]) -> {"exists": true, "value": true}
	function getJsonValue(json, path){
		let value = json
		for (let i = 0; i < path.length; ++i){
			if (value === null || typeof value !== "object" || !(path[i] in value)){
				return {"exists": false, "value": null}
			}

			value = value[path[i]]
		}

		return {"exists": true, "value": value}
	}

	// JSON Patch (RFC 6902) that turns the model as it was before changeSet into the model now.
	// Inserts and removes keep their order and indexes; changed values come after them with the current values.
	// Without a known path the patch replaces the whole model. Null when the model cannot be serialized.
	//   toJsonPatch([{"path": ["permissions", 4, "direct"], "operation": "set"}], role)
	//     -> [{"op": "add", "path": "/permissions/4/direct", "value": true}]
	//   ("add" of an object member replaces it; an element of a list by its index gets "replace")
	//   toJsonPatch([], role) -> [{"op": "replace", "path": "", "value": <the whole model>}]
	function toJsonPatch(changeSet, model){
		let text = model.toJson()
		if (text === ""){
			return null
		}

		let json = JSON.parse(text)
		if (!changeSet || changeSet.length === 0){
			return [{"op": "replace", "path": "", "value": json}]
		}

		for (let i = 0; i < changeSet.length; ++i){
			if (!changeSet[i].path){
				return [{"op": "replace", "path": "", "value": json}]
			}
		}

		// Changed values and inserted elements in the paths of the current model
		let setPaths = []
		let insertPaths = []
		for (let i = 0; i < changeSet.length; ++i){
			let change = changeSet[i]
			if (change.operation === "set"){
				let finalPath = mapPathForward(changeSet, change.path, i)
				if (finalPath !== null){
					setPaths.push(finalPath)
				}
			}
			else if (change.operation === "insert"){
				let finalPath = mapPathForward(changeSet, change.path, i)
				if (finalPath !== null){
					insertPaths.push(finalPath)
				}
			}
		}

		let patch = []
		for (let i = 0; i < changeSet.length; ++i){
			let change = changeSet[i]
			if (change.operation !== "insert" && change.operation !== "remove"){
				continue
			}

			// A list inside a removed element, a replaced value or an inserted element is written by that value
			let listPath = mapPathForward(changeSet, change.path.slice(0, change.path.length - 1), i)
			if (listPath === null || isCovered(setPaths, listPath, true) || isCovered(insertPaths, listPath, true)){
				continue
			}

			if (change.operation === "remove"){
				patch.push({"op": "remove", "path": toJsonPointer(change.path)})
			}
			else{
				let elementPath = mapPathForward(changeSet, change.path, i)
				let element = elementPath !== null ? getJsonValue(json, elementPath).value : null
				patch.push({"op": "add", "path": toJsonPointer(change.path), "value": element})
			}
		}

		let written = []
		for (let i = 0; i < setPaths.length; ++i){
			let path = setPaths[i]
			if (isCovered(setPaths, path, false) || isCovered(insertPaths, path, true) || isCovered(written, path, true)){
				continue
			}

			written.push(path)
			let current = getJsonValue(json, path)
			if (typeof path[path.length - 1] === "number"){
				patch.push({"op": "replace", "path": toJsonPointer(path), "value": current.value})
			}
			else if (current.exists){
				// "add" of an object member replaces it or creates it
				patch.push({"op": "add", "path": toJsonPointer(path), "value": current.value})
			}
			else{
				patch.push({"op": "remove", "path": toJsonPointer(path)})
			}
		}

		return patch
	}

	// True when one of paths contains path: as a shorter prefix, or also as the same path when orEqual is true.
	//   isCovered([["permissions"]], ["permissions", 4], false) -> true; isCovered([["permissions"]], ["permissions"], false) -> false
	function isCovered(paths, path, orEqual){
		for (let i = 0; i < paths.length; ++i){
			if (isPathPrefix(paths[i], path) && (orEqual || paths[i].length < path.length)){
				return true
			}
		}

		return false
	}
}
