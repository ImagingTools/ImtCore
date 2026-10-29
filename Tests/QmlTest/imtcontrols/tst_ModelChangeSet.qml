import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtauthAssignmentsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

// The changeSet of BaseClass.modelChanged: paths, operations and ChangeSetHelper, see Docs/Development/ModelChangeSet.md
TestCase {
	id: testCase

	name: "ModelChangeSet"

	property var records: []
	property var lastChangeSet: []
	property int emissions: 0

	readonly property string roleJson: '{"id":"r1","name":"Role","description":"d","permissions":['
		+ '{"id":"p0","direct":true,"sources":['
		+ '{"steps":[{"kind":"Role","id":"a","name":"A"},{"kind":"Role","id":"b","name":"B"},{"kind":"Role","id":"c","name":"C"}]},'
		+ '{"steps":[{"kind":"Role","id":"d","name":"D"},{"kind":"Role","id":"e","name":"E"},{"kind":"Role","id":"f","name":"F"}]}]},'
		+ '{"id":"p1","direct":false},'
		+ '{"id":"p2","direct":true}]}'

	readonly property string groupJson: '{"logicalOperation":"And","groupFilters":['
		+ '{"logicalOperation":"Or","groupFilters":['
		+ '{"logicalOperation":"And"},'
		+ '{"logicalOperation":"Or","fieldFilters":['
		+ '{"__typename":"ArrayFieldFilter","fieldId":"tags","filterValues":["a"],"filterValueType":"String","filterOperations":["ArrayHasAny"]},'
		+ '{"__typename":"FieldFilter","fieldId":"name","filterValue":"x","filterValueType":"String","filterOperations":["Contains"]}]}]}]}'

	Component {
		id: roleComp

		RoleData {}
	}

	Component {
		id: assignmentComp

		Assignment {}
	}

	Component {
		id: timeFilterComp

		TimeFilter {}
	}

	Component {
		id: groupFilterComp

		GroupFilter {}
	}

	function recordChanges(changeSet){
		testCase.emissions++
		testCase.lastChangeSet = changeSet
		let list = testCase.records
		for (let i = 0; i < changeSet.length; ++i){
			list.push(changeSet[i].operation + " " + ChangeSetHelper.pathToString(changeSet[i].path))
		}
		testCase.records = list
	}

	function watch(model){
		testCase.records = []
		testCase.lastChangeSet = []
		testCase.emissions = 0
		model.modelChanged.connect(testCase.recordChanges)
	}

	function createRole(){
		let role = createTemporaryObject(roleComp, testCase)
		verify(role.createFromJson(testCase.roleJson))
		watch(role)

		return role
	}

	function createAssignment(id){
		let assignment = createTemporaryObject(assignmentComp, testCase)
		assignment.m_id = id

		return assignment
	}

	function permission(role, index){
		return role.m_permissions.get(index).item
	}

	// --- Paths and operations

	function test_rootProperty(){
		let role = createRole()
		role.m_name = "Changed"

		compare(testCase.records, ["set name"])
		compare(testCase.emissions, 1)
	}

	function test_sameValueIsNotReported(){
		let role = createRole()
		role.m_name = "Role"
		permission(role, 0).m_direct = true

		compare(testCase.records, [])
		compare(testCase.emissions, 0)
	}

	function test_compatibilityFields(){
		let role = createRole()
		permission(role, 1).m_direct = true

		compare(testCase.lastChangeSet[0].name, "m_direct")
		verify(testCase.lastChangeSet[0].sender === permission(role, 1))
	}

	function test_nestedObject(){
		let timeFilter = createTemporaryObject(timeFilterComp, testCase)
		timeFilter.emplaceTimeRange()
		watch(timeFilter)

		timeFilter.m_timeRange.m_begin = "2026-01-01"
		timeFilter.m_timeUnit = "Day"

		// Path steps are the JSON keys of the SDL schema: TimeRange declares "Begin"
		compare(testCase.records, ["set timeRange.Begin", "set timeUnit"])
		compare(ChangeSetHelper.getValue(timeFilter, "timeRange.Begin"), "2026-01-01")
	}

	function test_nestedObjectReplaced(){
		let timeFilter = createTemporaryObject(timeFilterComp, testCase)
		timeFilter.emplaceTimeRange()
		let oldRange = timeFilter.m_timeRange
		watch(timeFilter)

		timeFilter.emplaceTimeRange()
		timeFilter.m_timeRange.m_end = "2026-02-01"
		timeFilter.m_timeRange = null

		compare(testCase.records, ["set timeRange", "set timeRange.End", "set timeRange"])

		// The replaced object is not in the model any more
		testCase.records = []
		oldRange.m_begin = "stale"
		compare(testCase.records, ["set ?"])
	}

	function test_listElementProperty(){
		let role = createRole()
		permission(role, 1).m_direct = true
		permission(role, 2).m_name = "Two"

		compare(testCase.records, ["set permissions[1].direct", "set permissions[2].name"])
	}

	function test_deepListPath(){
		let role = createRole()
		let step = permission(role, 0).m_sources.get(1).item.m_steps.get(2).item
		step.m_name = "Renamed"

		compare(testCase.records, ["set permissions[0].sources[1].steps[2].name"])
	}

	function test_insertElement(){
		let role = createRole()
		role.m_permissions.appendElement(createAssignment("p3"))
		role.m_permissions.insertElement(0, createAssignment("first"))
		role.m_permissions.addElement(createAssignment("last"))
		role.m_permissions.insertElement(1, null)

		compare(testCase.records, ["insert permissions[3]", "insert permissions[0]", "insert permissions[5]", "insert permissions[1]"])
		compare(testCase.lastChangeSet[0].name, "m_permissions")
	}

	function test_insertedElementReportsItsChanges(){
		let role = createRole()
		let added = createAssignment("p3")
		added.m_name = "set before insert"
		role.m_permissions.insertElement(1, added)
		added.m_direct = true

		compare(testCase.records, ["insert permissions[1]", "set permissions[1].direct"])
	}

	function test_removeElement(){
		let role = createRole()
		role.m_permissions.removeElement(1)
		role.m_permissions.removeElement(0)

		compare(testCase.records, ["remove permissions[1]", "remove permissions[0]"])
	}

	function test_indexesAfterRemoval(){
		let role = createRole()
		let last = permission(role, 2)
		role.m_permissions.removeElement(0)
		last.m_direct = false

		compare(testCase.records, ["remove permissions[0]", "set permissions[1].direct"])
	}

	function test_removedElementHasUnknownPath(){
		let role = createRole()
		let removed = permission(role, 1)
		role.m_permissions.removeElement(1)
		removed.m_direct = true

		compare(testCase.records, ["remove permissions[1]", "set ?"])
	}

	function test_swapItems(){
		let role = createRole()
		verify(role.m_permissions.swapItems(0, 2))

		compare(testCase.records, ["set permissions[0]", "set permissions[2]"])
		compare(testCase.emissions, 1)
		compare(permission(role, 0).m_id, "p2")
		compare(permission(role, 2).m_id, "p0")

		// The swapped elements are copies, they report their own path
		testCase.records = []
		permission(role, 2).m_sources.get(0).item.m_steps.get(0).item.m_name = "Z"
		compare(testCase.records, ["set permissions[2].sources[0].steps[0].name"])
	}

	function test_transactionKeepsOrder(){
		let role = createRole()
		role.beginChanges()
		role.m_name = "Changed"
		role.m_permissions.removeElement(2)
		role.m_permissions.appendElement(createAssignment("p3"))
		permission(role, 1).m_direct = true
		role.endChanges()

		compare(testCase.emissions, 1)
		compare(testCase.records, ["set name", "remove permissions[2]", "insert permissions[2]", "set permissions[1].direct"])
	}

	function test_childTransactionInsideOwnerTransaction(){
		let role = createRole()
		let first = permission(role, 0)
		role.beginChanges()
		first.beginChanges()
		first.m_name = "Zero"
		first.m_direct = false
		first.endChanges()
		role.m_description = "Changed"
		role.endChanges()

		compare(testCase.emissions, 1)
		compare(testCase.records, ["set permissions[0].name", "set permissions[0].direct", "set description"])
	}

	function test_emptyTransactionIsNotReported(){
		let role = createRole()
		role.beginChanges()
		role.m_name = "Role"
		role.endChanges()

		compare(testCase.emissions, 0)
	}

	function test_recursivePolymorphicTree(){
		let groupFilter = createTemporaryObject(groupFilterComp, testCase)
		verify(groupFilter.createFromJson(testCase.groupJson))
		watch(groupFilter)

		let inner = groupFilter.m_groupFilters.get(0).item.m_groupFilters.get(1).item
		inner.m_fieldFilters.get(0).item.m_filterValues = ["a", "b"]
		inner.m_fieldFilters.get(1).item.m_filterValue = "y"
		inner.m_logicalOperation = "And"
		groupFilter.m_groupFilters.get(0).item.m_groupFilters.removeElement(0)

		compare(testCase.records, [
			"set groupFilters[0].groupFilters[1].fieldFilters[0].filterValues",
			"set groupFilters[0].groupFilters[1].fieldFilters[1].filterValue",
			"set groupFilters[0].groupFilters[1].logicalOperation",
			"remove groupFilters[0].groupFilters[0]"])
	}

	function test_primitiveArray(){
		let groupFilter = createTemporaryObject(groupFilterComp, testCase)
		verify(groupFilter.createFromJson(testCase.groupJson))
		let fieldFilter = groupFilter.m_groupFilters.get(0).item.m_groupFilters.get(1).item.m_fieldFilters.get(1).item
		watch(groupFilter)

		fieldFilter.m_filterOperations = ["Contains", "Not"]
		compare(testCase.records, ["set groupFilters[0].groupFilters[1].fieldFilters[1].filterOperations"])

		// An array changed in place does not notify: assign a new array
		testCase.records = []
		fieldFilter.m_filterOperations.push("Equal")
		compare(testCase.records, [])
	}

	function test_pathIsRelativeToListener(){
		let role = createRole()
		let first = permission(role, 0)
		let source = first.m_sources.get(1).item
		let roleRecords = []
		let sourceRecords = []
		role.modelChanged.connect(function(changeSet){
			roleRecords.push(ChangeSetHelper.pathToString(changeSet[0].path))
		})
		source.modelChanged.connect(function(changeSet){
			sourceRecords.push(ChangeSetHelper.pathToString(changeSet[0].path))
		})

		source.m_steps.get(0).item.m_id = "x"

		compare(sourceRecords, ["steps[0].id"])
		compare(roleRecords, ["permissions[0].sources[1].steps[0].id"])
	}

	function test_disabledNotifications(){
		let role = createRole()
		role.enableNotifications = false
		role.m_name = "Changed"
		permission(role, 1).m_direct = true
		role.m_permissions.removeElement(0)
		role.enableNotifications = true
		compare(testCase.emissions, 0)

		let element = permission(role, 0)
		element.enableNotifications = false
		element.m_direct = false
		compare(testCase.emissions, 0)
	}

	function test_emplaceAndRemoveKey(){
		let role = createTemporaryObject(roleComp, testCase)
		watch(role)

		role.emplacePermissions()
		role.m_permissions.appendElement(createAssignment("p0"))
		role.removePermissions()

		compare(testCase.records, ["set permissions", "insert permissions[0]", "set permissions"])
	}

	function test_fromObjectOnFilledModel(){
		let role = createRole()
		verify(role.createFromJson('{"id":"r1","name":"Other","description":"d","permissions":[{"id":"p0","direct":false}]}'))

		compare(testCase.emissions, 1)
		verify(testCase.records.indexOf("set name") >= 0)
		compare(testCase.records.indexOf("set permissions"), -1, "A list filled again is a load, not an edit")
		compare(testCase.records.indexOf("set description"), -1)
		compare(testCase.records.indexOf("set id"), -1)
	}

	// A list the model did not have is a new value of its property
	function test_fromObjectFillsAMissingList(){
		let role = createTemporaryObject(roleComp, testCase)
		verify(role.createFromJson('{"id":"r1","name":"Role"}'))
		watch(role)

		verify(role.createFromJson('{"id":"r1","name":"Role","permissions":[{"id":"p0","direct":false}]}'))

		compare(testCase.records, ["set permissions"])
	}

	function test_manualEmptyChangeSet(){
		let role = createRole()
		permission(role, 0).modelChanged([])

		compare(testCase.emissions, 1)
		compare(testCase.lastChangeSet.length, 0)
		verify(ChangeSetHelper.isAffected(testCase.lastChangeSet, "name"))
	}

	// --- ChangeSetHelper

	function test_pathToString(){
		compare(ChangeSetHelper.pathToString(["permissions", 4, "direct"]), "permissions[4].direct")
		compare(ChangeSetHelper.pathToString([]), "")
		compare(ChangeSetHelper.pathToString(null), "?")
	}

	function test_parsePattern(){
		compare(ChangeSetHelper.parsePattern("permissions[*].sources[1].steps"), ["permissions", "*", "sources", 1, "steps"])
		compare(ChangeSetHelper.parsePattern("*.name"), ["*", "name"])
		compare(ChangeSetHelper.parsePattern(""), [])
		compare(ChangeSetHelper.parsePattern(["a", 0]), ["a", 0])
	}

	function test_changeAffectsAndIsWithin(){
		let change = {"path": ["permissions", 4, "direct"], "operation": "set"}
		verify(ChangeSetHelper.changeAffects(change, "permissions"))
		verify(ChangeSetHelper.changeAffects(change, "permissions[*].direct"))
		verify(ChangeSetHelper.changeAffects(change, "permissions[4].direct.deeper"))
		verify(!ChangeSetHelper.changeAffects(change, "permissions[3]"))
		verify(!ChangeSetHelper.changeAffects(change, "name"))

		verify(ChangeSetHelper.changeIsWithin(change, "permissions"))
		verify(ChangeSetHelper.changeIsWithin(change, "permissions[*]"))
		verify(!ChangeSetHelper.changeIsWithin(change, "permissions[4].direct.deeper"))

		let unknown = {"path": null, "operation": "set"}
		verify(ChangeSetHelper.changeAffects(unknown, "name"))
		verify(!ChangeSetHelper.changeIsWithin(unknown, ""))
	}

	function test_roleEditorScenario(){
		let role = createRole()
		role.beginChanges()
		permission(role, 1).m_direct = true
		role.m_permissions.removeElement(2)
		role.m_permissions.appendElement(createAssignment("p3"))
		role.endChanges()
		let permissionsOnly = testCase.lastChangeSet

		verify(ChangeSetHelper.isOnlyWithin(permissionsOnly, "permissions"))
		verify(ChangeSetHelper.isAffected(permissionsOnly, "permissions[*].direct"))
		verify(!ChangeSetHelper.isAffected(permissionsOnly, "name"))
		compare(ChangeSetHelper.getRootKeys(permissionsOnly), ["permissions"])
		compare(ChangeSetHelper.getChangedIndexes(permissionsOnly, "permissions"), [1, 2])
		compare(ChangeSetHelper.filter(permissionsOnly, "permissions[1]").length, 1)

		role.m_name = "Changed"
		let nameOnly = testCase.lastChangeSet
		verify(!ChangeSetHelper.isOnlyWithin(nameOnly, "permissions"))
		compare(ChangeSetHelper.getRootKeys(nameOnly), ["name"])
	}

	function test_unknownAndEmptyChangeSets(){
		let unknown = [{"path": null, "operation": "set"}]
		verify(ChangeSetHelper.isAffected(unknown, "name"))
		verify(!ChangeSetHelper.isOnlyWithin(unknown, "permissions"))
		compare(ChangeSetHelper.getRootKeys(unknown), null)
		compare(ChangeSetHelper.getChangedIndexes(unknown, "permissions"), null)

		verify(ChangeSetHelper.isAffected([], "name"))
		verify(!ChangeSetHelper.isOnlyWithin([], "permissions"))
		compare(ChangeSetHelper.getRootKeys([]), null)
	}

	function test_getChangedIndexesOfWholeList(){
		let role = createRole()
		role.removePermissions()

		compare(ChangeSetHelper.getChangedIndexes(testCase.lastChangeSet, "permissions"), null)
		compare(ChangeSetHelper.getChangedIndexes([{"path": ["name"], "operation": "set"}], "permissions"), [])
	}

	// --- JSON Patch: the snapshot before the changes with the patch applied must be the model after them

	// Strict RFC 6902 add/remove/replace: a missing target or a wrong index throws
	function applyPatch(document, patch){
		let result = JSON.parse(JSON.stringify(document))
		for (let i = 0; i < patch.length; ++i){
			let operation = patch[i]
			let value = operation.value === undefined ? undefined : JSON.parse(JSON.stringify(operation.value))
			if (operation.path === ""){
				result = value
				continue
			}

			let steps = operation.path.split("/").slice(1)
			let parent = result
			for (let k = 0; k < steps.length - 1; ++k){
				let step = steps[k].replace(/~1/g, "/").replace(/~0/g, "~")
				if (parent === null || typeof parent !== "object" || !(step in parent)){
					throw new Error("No target for " + operation.op + " " + operation.path)
				}
				parent = parent[step]
			}

			let last = steps[steps.length - 1].replace(/~1/g, "/").replace(/~0/g, "~")
			if (Array.isArray(parent)){
				let index = last === "-" ? parent.length : Number(last)
				let limit = operation.op === "add" ? parent.length : parent.length - 1
				if (index < 0 || index > limit){
					throw new Error("Wrong index for " + operation.op + " " + operation.path)
				}
				if (operation.op === "add"){
					parent.splice(index, 0, value)
				}
				else if (operation.op === "remove"){
					parent.splice(index, 1)
				}
				else{
					parent[index] = value
				}
			}
			else{
				if (operation.op !== "add" && !(last in parent)){
					throw new Error("No member for " + operation.op + " " + operation.path)
				}
				if (operation.op === "remove"){
					delete parent[last]
				}
				else{
					parent[last] = value
				}
			}
		}

		return result
	}

	// JSON text with sorted keys: member order does not matter in JSON
	function canonical(value){
		if (value === null || typeof value !== "object"){
			return JSON.stringify(value === undefined ? null : value)
		}

		if (Array.isArray(value)){
			let items = []
			for (let i = 0; i < value.length; ++i){
				items.push(canonical(value[i]))
			}

			return "[" + items.join(",") + "]"
		}

		let keys = Object.keys(value).sort()
		let members = []
		for (let i = 0; i < keys.length; ++i){
			members.push(JSON.stringify(keys[i]) + ":" + canonical(value[keys[i]]))
		}

		return "{" + members.join(",") + "}"
	}

	// Collects every changeSet of model, in order, from the snapshot taken now
	function startPatch(model){
		testCase.collected = []
		testCase.before = JSON.parse(model.toJson())
		model.modelChanged.connect(testCase.collectChanges)
	}

	property var collected: []
	property var before: null

	function collectChanges(changeSet){
		testCase.collected = testCase.collected.concat(changeSet)
	}

	function verifyPatch(model){
		let patch = ChangeSetHelper.toJsonPatch(testCase.collected, model)
		verify(patch !== null)
		compare(canonical(applyPatch(testCase.before, patch)), canonical(JSON.parse(model.toJson())))

		return patch
	}

	function test_jsonPointer(){
		compare(ChangeSetHelper.toJsonPointer(["permissions", 4, "direct"]), "/permissions/4/direct")
		compare(ChangeSetHelper.toJsonPointer(["a/b", "c~d"]), "/a~1b/c~0d")
		compare(ChangeSetHelper.toJsonPointer([]), "")
	}

	function test_jsonPatchOfSimpleChanges(){
		let role = createRole()
		startPatch(role)
		role.m_name = "Changed"
		permission(role, 1).m_direct = true
		permission(role, 0).m_sources.get(1).item.m_steps.get(2).item.m_name = "Renamed"

		compare(verifyPatch(role), [
			{"op": "add", "path": "/name", "value": "Changed"},
			{"op": "add", "path": "/permissions/1/direct", "value": true},
			{"op": "add", "path": "/permissions/0/sources/1/steps/2/name", "value": "Renamed"}])
	}

	function test_jsonPatchOfListOperations(){
		let role = createRole()
		startPatch(role)
		role.m_permissions.removeElement(0)
		let added = createAssignment("p3")
		role.m_permissions.insertElement(1, added)
		added.m_direct = true

		let patch = verifyPatch(role)
		compare(patch.length, 2)
		compare(patch[0], {"op": "remove", "path": "/permissions/0"})
		compare(patch[1].op, "add")
		compare(patch[1].path, "/permissions/1")
		compare(patch[1].value.direct, true)
	}

	function test_jsonPatchUsesFinalIndexes(){
		let role = createRole()
		startPatch(role)
		permission(role, 1).m_name = "One"
		role.m_permissions.insertElement(0, createAssignment("first"))
		permission(role, 3).m_direct = false
		role.m_permissions.removeElement(3)

		let patch = verifyPatch(role)
		compare(patch, [
			{"op": "add", "path": "/permissions/0", "value": JSON.parse(role.m_permissions.toJson())[0]},
			{"op": "remove", "path": "/permissions/3"},
			{"op": "add", "path": "/permissions/2/name", "value": "One"}])
	}

	function test_jsonPatchOfReplacedList(){
		let role = createRole()
		startPatch(role)
		permission(role, 0).m_direct = false
		role.emplacePermissions()
		role.m_permissions.appendElement(createAssignment("only"))
		permission(role, 0).m_name = "Only"

		compare(verifyPatch(role), [{"op": "add", "path": "/permissions", "value": JSON.parse(role.m_permissions.toJson())}])
	}

	function test_jsonPatchOfRemovedMember(){
		let role = createRole()
		startPatch(role)
		role.removePermissions()

		compare(verifyPatch(role), [{"op": "remove", "path": "/permissions"}])
	}

	// A member created and removed again: the receiver gets a remove of a member it may not have
	function test_jsonPatchOfMemberCreatedAndRemoved(){
		let role = createTemporaryObject(roleComp, testCase)
		verify(role.createFromJson('{"id":"r1","name":"Role"}'))
		startPatch(role)
		verify(!("permissions" in testCase.before))

		role.emplacePermissions()
		role.m_permissions.appendElement(createAssignment("p0"))
		role.removePermissions()

		compare(ChangeSetHelper.toJsonPatch(testCase.collected, role), [{"op": "remove", "path": "/permissions"}])
		verify(!("permissions" in JSON.parse(role.toJson())))
	}

	function test_jsonPatchOfSwap(){
		let role = createRole()
		startPatch(role)
		role.m_permissions.swapItems(0, 2)

		let patch = verifyPatch(role)
		compare(patch.length, 2)
		compare(patch[0].op, "replace")
		compare(patch[0].path, "/permissions/0")
	}

	function test_jsonPatchOfTree(){
		let groupFilter = createTemporaryObject(groupFilterComp, testCase)
		verify(groupFilter.createFromJson(testCase.groupJson))
		startPatch(groupFilter)

		let outer = groupFilter.m_groupFilters.get(0).item
		let inner = outer.m_groupFilters.get(1).item
		inner.m_fieldFilters.get(0).item.m_filterValues = ["a", "b"]
		inner.m_fieldFilters.removeElement(1)
		outer.m_groupFilters.removeElement(0)
		outer.m_groupFilters.get(0).item.m_logicalOperation = "And"
		groupFilter.m_logicalOperation = "Or"

		verifyPatch(groupFilter)
	}

	function test_jsonPatchOfChangesInRemovedElement(){
		let role = createRole()
		startPatch(role)
		let first = permission(role, 0)
		first.m_sources.get(0).item.m_steps.removeElement(1)
		first.m_name = "Zero"
		role.m_permissions.removeElement(0)

		compare(verifyPatch(role), [{"op": "remove", "path": "/permissions/0"}])
	}

	function test_jsonPatchOfReload(){
		let role = createRole()
		startPatch(role)
		verify(role.createFromJson('{"id":"r1","name":"Other","description":"d","permissions":[{"id":"p9","direct":false}]}'))

		// The list filled again is not in the patch, the changed fields are
		let patch = ChangeSetHelper.toJsonPatch(testCase.collected, role)
		let paths = []
		for (let i = 0; i < patch.length; ++i){
			paths.push(patch[i].path)
		}
		verify(paths.indexOf("/name") >= 0)
		compare(paths.indexOf("/permissions"), -1)
	}

	function test_jsonPatchOfUnknownPath(){
		let role = createRole()
		startPatch(role)
		permission(role, 0).modelChanged([])

		compare(verifyPatch(role), [{"op": "replace", "path": "", "value": JSON.parse(role.toJson())}])
	}

	// Random edits of a role, in one transaction and one by one: the patch must always rebuild the model
	property int seed: 1

	function random(count){
		testCase.seed = (testCase.seed * 1103515245 + 12345) % 2147483648

		return Math.floor(testCase.seed / 65536) % count
	}

	function editRandomly(role, step){
		let permissions = role.m_permissions
		let count = permissions ? permissions.count : 0
		let action = random(12)
		if (action === 0){
			role.m_name = "name" + step

			return
		}

		if (action === 1 && count > 0){
			let item = permission(role, random(count))
			item.m_direct = !item.m_direct

			return
		}

		if (action === 2 && count > 0){
			permission(role, random(count)).m_name = "p" + step

			return
		}

		if ((action === 3 || action === 4) && permissions){
			let added = createTemporaryObject(assignmentComp, testCase)
			verify(added.createFromJson('{"id":"n' + step + '","direct":true,"sources":[{"steps":[{"kind":"Role","id":"s","name":"S' + step + '"}]}]}'))
			permissions.insertElement(random(count + 1), added)

			return
		}

		if (action === 5 && count > 0){
			permissions.removeElement(random(count))

			return
		}

		if (action === 6 && count > 1){
			let first = random(count)
			let second = (first + 1 + random(count - 1)) % count
			permissions.swapItems(first, second)

			return
		}

		if (action === 7 && count > 0){
			let sources = permission(role, random(count)).m_sources
			if (sources && sources.count > 0){
				let steps = sources.get(random(sources.count)).item.m_steps
				if (steps && steps.count > 0){
					steps.get(random(steps.count)).item.m_name = "step" + step
				}
			}

			return
		}

		if (action === 8 && count > 0){
			let sources = permission(role, random(count)).m_sources
			if (sources && sources.count > 0){
				let steps = sources.get(random(sources.count)).item.m_steps
				if (steps && steps.count > 0){
					steps.removeElement(random(steps.count))
				}
			}

			return
		}

		if (action === 9 && count > 0){
			permission(role, random(count)).removeSources()

			return
		}

		if (action === 10 && random(4) === 0){
			role.emplacePermissions()

			return
		}

		if (action === 11){
			role.m_description = "d" + step
		}
	}

	function test_jsonPatchOfRandomEdits_data(){
		let rows = []
		for (let i = 1; i <= 40; ++i){
			rows.push({"tag": "seed " + i + (i % 2 === 0 ? " in a transaction" : " one by one"), "seed": i, "transaction": i % 2 === 0})
		}

		return rows
	}

	function test_jsonPatchOfRandomEdits(data){
		testCase.seed = data.seed
		let role = createRole()
		startPatch(role)
		if (data.transaction){
			role.beginChanges()
		}

		for (let step = 0; step < 30; ++step){
			editRandomly(role, step)
		}

		if (data.transaction){
			role.endChanges()
		}

		verifyPatch(role)
	}

	function test_getValue(){
		let role = createRole()

		compare(ChangeSetHelper.getValue(role, ["permissions", 0, "sources", 1, "steps", 2, "name"]), "F")
		compare(ChangeSetHelper.getValue(role, "permissions[2].id"), "p2")
		verify(ChangeSetHelper.getValue(role, "permissions[1]") === permission(role, 1))
		compare(ChangeSetHelper.getValue(role, "permissions[9].id"), undefined)
		verify(ChangeSetHelper.getValue(role, "") === role)
	}
}
