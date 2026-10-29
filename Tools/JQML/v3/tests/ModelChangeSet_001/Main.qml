import QtQuick
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtauthAssignmentsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

// The changeSet of BaseClass.modelChanged must be the same in Qt and on the web, see Docs/Development/ModelChangeSet.md
Item {
	id: root

	property var records: []
	property var lastChangeSet: []
	property int emissions: 0
	property int failures: 0

	property string roleJson: '{"id":"r1","name":"Role","description":"d","permissions":['
		+ '{"id":"p0","direct":true,"sources":['
		+ '{"steps":[{"kind":"Role","id":"a","name":"A"},{"kind":"Role","id":"b","name":"B"},{"kind":"Role","id":"c","name":"C"}]},'
		+ '{"steps":[{"kind":"Role","id":"d","name":"D"},{"kind":"Role","id":"e","name":"E"},{"kind":"Role","id":"f","name":"F"}]}]},'
		+ '{"id":"p1","direct":false},'
		+ '{"id":"p2","direct":true}]}'

	property string groupJson: '{"logicalOperation":"And","groupFilters":['
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
		root.emissions = root.emissions + 1
		root.lastChangeSet = changeSet
		let list = root.records
		for (let i = 0; i < changeSet.length; ++i){
			list.push(changeSet[i].operation + " " + ChangeSetHelper.pathToString(changeSet[i].path))
		}
		root.records = list
	}

	function watch(model){
		root.records = []
		root.lastChangeSet = []
		root.emissions = 0
		model.modelChanged.connect(root.recordChanges)
	}

	function toText(value){
		if (Array.isArray(value)){
			let parts = []
			for (let i = 0; i < value.length; ++i){
				parts.push(root.toText(value[i]))
			}

			return "[" + parts.join("|") + "]"
		}

		return String(value)
	}

	function check(name, actual, expected){
		let actualText = root.toText(actual)
		let expectedText = root.toText(expected)
		if (actualText === expectedText){
			console.log(name + " ok " + actualText)
		}
		else{
			root.failures = root.failures + 1
			console.log(name + " MISMATCH " + actualText + " expected " + expectedText)
		}
	}

	function createRole(){
		let role = roleComp.createObject(root)
		role.createFromJson(root.roleJson)
		root.watch(role)

		return role
	}

	function createAssignment(id){
		let assignment = assignmentComp.createObject(root)
		assignment.m_id = id

		return assignment
	}

	function permission(role, index){
		return role.m_permissions.get(index).item
	}

	function testRootProperty(){
		let role = root.createRole()
		role.m_name = "Changed"
		root.check("rootProperty", root.records, ["set name"])
		root.check("rootProperty.emissions", root.emissions, 1)
	}

	function testSameValue(){
		let role = root.createRole()
		role.m_name = "Role"
		root.permission(role, 0).m_direct = true
		root.check("sameValue", root.emissions, 0)
	}

	function testCompatibilityFields(){
		let role = root.createRole()
		root.permission(role, 1).m_direct = true
		root.check("compatibility.name", root.lastChangeSet[0].name, "m_direct")
		root.check("compatibility.sender", root.lastChangeSet[0].sender === root.permission(role, 1), true)
	}

	function testNestedObject(){
		let timeFilter = timeFilterComp.createObject(root)
		timeFilter.emplaceTimeRange()
		root.watch(timeFilter)
		timeFilter.m_timeRange.m_begin = "2026-01-01"
		timeFilter.m_timeUnit = "Day"
		root.check("nestedObject", root.records, ["set timeRange.Begin", "set timeUnit"])
		root.check("nestedObject.value", ChangeSetHelper.getValue(timeFilter, "timeRange.Begin"), "2026-01-01")
	}

	function testNestedObjectReplaced(){
		let timeFilter = timeFilterComp.createObject(root)
		timeFilter.emplaceTimeRange()
		let oldRange = timeFilter.m_timeRange
		root.watch(timeFilter)
		timeFilter.emplaceTimeRange()
		timeFilter.m_timeRange.m_end = "2026-02-01"
		timeFilter.m_timeRange = null
		root.check("nestedReplaced", root.records, ["set timeRange", "set timeRange.End", "set timeRange"])

		root.records = []
		oldRange.m_begin = "stale"
		root.check("nestedReplaced.stale", root.records, ["set ?"])
	}

	function testListElementProperty(){
		let role = root.createRole()
		root.permission(role, 1).m_direct = true
		root.permission(role, 2).m_name = "Two"
		root.check("listElement", root.records, ["set permissions[1].direct", "set permissions[2].name"])
	}

	function testDeepListPath(){
		let role = root.createRole()
		root.permission(role, 0).m_sources.get(1).item.m_steps.get(2).item.m_name = "Renamed"
		root.check("deepPath", root.records, ["set permissions[0].sources[1].steps[2].name"])
	}

	function testInsertElement(){
		let role = root.createRole()
		role.m_permissions.appendElement(root.createAssignment("p3"))
		role.m_permissions.insertElement(0, root.createAssignment("first"))
		role.m_permissions.addElement(root.createAssignment("last"))
		role.m_permissions.insertElement(1, null)
		root.check("insert", root.records, ["insert permissions[3]", "insert permissions[0]", "insert permissions[5]", "insert permissions[1]"])
		root.check("insert.name", root.lastChangeSet[0].name, "m_permissions")
	}

	function testInsertedElementReports(){
		let role = root.createRole()
		let added = root.createAssignment("p3")
		added.m_name = "set before insert"
		role.m_permissions.insertElement(1, added)
		added.m_direct = true
		root.check("insertedReports", root.records, ["insert permissions[1]", "set permissions[1].direct"])
	}

	function testRemoveElement(){
		let role = root.createRole()
		role.m_permissions.removeElement(1)
		role.m_permissions.removeElement(0)
		root.check("remove", root.records, ["remove permissions[1]", "remove permissions[0]"])
	}

	function testIndexesAfterRemoval(){
		let role = root.createRole()
		let last = root.permission(role, 2)
		role.m_permissions.removeElement(0)
		last.m_direct = false
		root.check("indexesAfterRemoval", root.records, ["remove permissions[0]", "set permissions[1].direct"])
	}

	function testRemovedElementUnknown(){
		let role = root.createRole()
		let removed = root.permission(role, 1)
		role.m_permissions.removeElement(1)
		removed.m_direct = true
		root.check("removedUnknown", root.records, ["remove permissions[1]", "set ?"])
	}

	function testSwapItems(){
		let role = root.createRole()
		role.m_permissions.swapItems(0, 2)
		root.check("swap", root.records, ["set permissions[0]", "set permissions[2]"])
		root.check("swap.emissions", root.emissions, 1)
		root.check("swap.ids", [root.permission(role, 0).m_id, root.permission(role, 2).m_id], ["p2", "p0"])

		root.records = []
		root.permission(role, 2).m_sources.get(0).item.m_steps.get(0).item.m_name = "Z"
		root.check("swap.copies", root.records, ["set permissions[2].sources[0].steps[0].name"])
	}

	function testTransactionOrder(){
		let role = root.createRole()
		role.beginChanges()
		role.m_name = "Changed"
		role.m_permissions.removeElement(2)
		role.m_permissions.appendElement(root.createAssignment("p3"))
		root.permission(role, 1).m_direct = true
		role.endChanges()
		root.check("transaction.emissions", root.emissions, 1)
		root.check("transaction", root.records, ["set name", "remove permissions[2]", "insert permissions[2]", "set permissions[1].direct"])
	}

	function testChildTransaction(){
		let role = root.createRole()
		let first = root.permission(role, 0)
		role.beginChanges()
		first.beginChanges()
		first.m_name = "Zero"
		first.m_direct = false
		first.endChanges()
		role.m_description = "Changed"
		role.endChanges()
		root.check("childTransaction.emissions", root.emissions, 1)
		root.check("childTransaction", root.records, ["set permissions[0].name", "set permissions[0].direct", "set description"])
	}

	function testEmptyTransaction(){
		let role = root.createRole()
		role.beginChanges()
		role.m_name = "Role"
		role.endChanges()
		root.check("emptyTransaction", root.emissions, 0)
	}

	function testRecursiveTree(){
		let groupFilter = groupFilterComp.createObject(root)
		groupFilter.createFromJson(root.groupJson)
		root.watch(groupFilter)

		let inner = groupFilter.m_groupFilters.get(0).item.m_groupFilters.get(1).item
		inner.m_fieldFilters.get(0).item.m_filterValues = ["a", "b"]
		inner.m_fieldFilters.get(1).item.m_filterValue = "y"
		inner.m_logicalOperation = "And"
		groupFilter.m_groupFilters.get(0).item.m_groupFilters.removeElement(0)
		root.check("recursiveTree", root.records, [
			"set groupFilters[0].groupFilters[1].fieldFilters[0].filterValues",
			"set groupFilters[0].groupFilters[1].fieldFilters[1].filterValue",
			"set groupFilters[0].groupFilters[1].logicalOperation",
			"remove groupFilters[0].groupFilters[0]"])
	}

	function testPrimitiveArray(){
		let groupFilter = groupFilterComp.createObject(root)
		groupFilter.createFromJson(root.groupJson)
		let fieldFilter = groupFilter.m_groupFilters.get(0).item.m_groupFilters.get(1).item.m_fieldFilters.get(1).item
		root.watch(groupFilter)

		fieldFilter.m_filterOperations = ["Contains", "Not"]
		root.check("primitiveArray", root.records, ["set groupFilters[0].groupFilters[1].fieldFilters[1].filterOperations"])

		root.records = []
		fieldFilter.m_filterOperations.push("Equal")
		root.check("primitiveArray.inPlace", root.records, [])

		// An array of values read by fromObject is a new value, unlike a list of SDL objects filled again
		root.watch(fieldFilter)
		fieldFilter.createFromJson('{"__typename":"FieldFilter","fieldId":"name","filterValue":"x","filterValueType":"String","filterOperations":["Equal"]}')
		root.check("primitiveArray.fromObject", root.records.indexOf("set filterOperations") >= 0, true)
	}

	function testRelativeToListener(){
		let role = root.createRole()
		let source = root.permission(role, 0).m_sources.get(1).item
		let roleRecords = []
		let sourceRecords = []
		role.modelChanged.connect(function(changeSet){
			roleRecords.push(ChangeSetHelper.pathToString(changeSet[0].path))
		})
		source.modelChanged.connect(function(changeSet){
			sourceRecords.push(ChangeSetHelper.pathToString(changeSet[0].path))
		})

		source.m_steps.get(0).item.m_id = "x"
		root.check("relative.source", sourceRecords, ["steps[0].id"])
		root.check("relative.role", roleRecords, ["permissions[0].sources[1].steps[0].id"])
	}

	function testDisabledNotifications(){
		let role = root.createRole()
		role.enableNotifications = false
		role.m_name = "Changed"
		root.permission(role, 1).m_direct = true
		role.m_permissions.removeElement(0)
		role.enableNotifications = true
		root.check("disabled.root", root.emissions, 0)

		let element = root.permission(role, 0)
		element.enableNotifications = false
		element.m_direct = false
		root.check("disabled.child", root.emissions, 0)
	}

	function testEmplaceAndRemoveKey(){
		let role = roleComp.createObject(root)
		root.watch(role)
		role.emplacePermissions()
		role.m_permissions.appendElement(root.createAssignment("p0"))
		role.removePermissions()
		root.check("emplaceRemoveKey", root.records, ["set permissions", "insert permissions[0]", "set permissions"])
	}

	function testFromObjectOnFilledModel(){
		let role = root.createRole()
		role.createFromJson('{"id":"r1","name":"Other","description":"d","permissions":[{"id":"p0","direct":false}]}')
		root.check("fromObject.emissions", root.emissions, 1)
		root.check("fromObject.name", root.records.indexOf("set name") >= 0, true)
		root.check("fromObject.permissions", root.records.indexOf("set permissions"), -1)
		root.check("fromObject.description", root.records.indexOf("set description"), -1)
		root.check("fromObject.id", root.records.indexOf("set id"), -1)

		// A list the model did not have is a new value of its property
		role = roleComp.createObject(root)
		role.createFromJson('{"id":"r1","name":"Role"}')
		root.watch(role)
		role.createFromJson('{"id":"r1","name":"Role","permissions":[{"id":"p0","direct":false}]}')
		root.check("fromObject.missingList", root.records, ["set permissions"])
	}

	function testManualEmptyChangeSet(){
		let role = root.createRole()
		root.permission(role, 0).modelChanged([])
		root.check("manualEmpty.emissions", root.emissions, 1)
		root.check("manualEmpty", root.records, ["set permissions[0]"])
		root.check("manualEmpty.affected", ChangeSetHelper.isAffected(root.lastChangeSet, "name"), false)

		// An unknown change of a child is kept in the transaction of its owner
		role = root.createRole()
		role.beginChanges()
		role.m_name = "Changed"
		root.permission(role, 1).modelChanged([])
		role.endChanges()
		root.check("manualEmpty.transaction", root.records, ["set name", "set permissions[1]"])
	}

	// toJson escapes what needs it and leaves plain text as it is
	function testEscaping(){
		let role = root.createRole()
		role.m_name = "a" + String.fromCharCode(34) + "b" + String.fromCharCode(92) + "c"
		role.m_description = "plain text"
		let parsed = JSON.parse(role.toJson())
		root.check("escape.special", parsed.name === role.m_name, true)
		root.check("escape.plain", parsed.description === "plain text", true)
		root.check("escape.list", JSON.parse(role.m_permissions.toJson())[0].id, "p0")
	}

	function testHelpers(){
		root.check("pathToString", ChangeSetHelper.pathToString(["permissions", 4, "direct"]), "permissions[4].direct")
		root.check("pathToString.unknown", ChangeSetHelper.pathToString(null), "?")
		root.check("parsePattern", ChangeSetHelper.parsePattern("permissions[*].sources[1].steps"), ["permissions", "*", "sources", 1, "steps"])

		let change = {"path": ["permissions", 4, "direct"], "operation": "set"}
		root.check("affects", [
			ChangeSetHelper.changeAffects(change, "permissions"),
			ChangeSetHelper.changeAffects(change, "permissions[*].direct"),
			ChangeSetHelper.changeAffects(change, "permissions[4].direct.deeper"),
			ChangeSetHelper.changeAffects(change, "permissions[3]"),
			ChangeSetHelper.changeAffects(change, "name")], [true, true, true, false, false])
		root.check("isWithin", [
			ChangeSetHelper.changeIsWithin(change, "permissions"),
			ChangeSetHelper.changeIsWithin(change, "permissions[*]"),
			ChangeSetHelper.changeIsWithin(change, "permissions[4].direct.deeper")], [true, true, false])

		let role = root.createRole()
		role.beginChanges()
		root.permission(role, 1).m_direct = true
		role.m_permissions.removeElement(2)
		role.m_permissions.appendElement(root.createAssignment("p3"))
		role.endChanges()
		let permissionsOnly = root.lastChangeSet
		root.check("roleEditor", [
			ChangeSetHelper.isOnlyWithin(permissionsOnly, "permissions"),
			ChangeSetHelper.isAffected(permissionsOnly, "permissions[*].direct"),
			ChangeSetHelper.isAffected(permissionsOnly, "name")], [true, true, false])
		root.check("roleEditor.rootKeys", ChangeSetHelper.getRootKeys(permissionsOnly), ["permissions"])
		root.check("roleEditor.indexes", ChangeSetHelper.getChangedIndexes(permissionsOnly, "permissions"), [1, 2])

		let unknown = [{"path": null, "operation": "set"}]
		root.check("unknown", [
			ChangeSetHelper.isAffected(unknown, "name"),
			ChangeSetHelper.isOnlyWithin(unknown, "permissions"),
			ChangeSetHelper.getRootKeys(unknown),
			ChangeSetHelper.getChangedIndexes(unknown, "permissions")], [true, false, null, null])

		root.check("getValue", [
			ChangeSetHelper.getValue(role, ["permissions", 0, "sources", 1, "steps", 2, "name"]),
			ChangeSetHelper.getValue(role, "permissions[2].id"),
			ChangeSetHelper.getValue(role, "permissions[9].id")], ["F", "p3", undefined])
	}

	// --- JSON Patch: the snapshot before the changes with the patch applied must be the model after them

	property var collected: []
	property var before: null
	property int seed: 1

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
					throw new Error("no target for " + operation.op + " " + operation.path)
				}
				parent = parent[step]
			}

			let last = steps[steps.length - 1].replace(/~1/g, "/").replace(/~0/g, "~")
			if (Array.isArray(parent)){
				let index = last === "-" ? parent.length : Number(last)
				let limit = operation.op === "add" ? parent.length : parent.length - 1
				if (index < 0 || index > limit){
					throw new Error("wrong index for " + operation.op + " " + operation.path)
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
					throw new Error("no member for " + operation.op + " " + operation.path)
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

	function canonical(value){
		if (value === null || typeof value !== "object"){
			return JSON.stringify(value === undefined ? null : value)
		}

		if (Array.isArray(value)){
			let items = []
			for (let i = 0; i < value.length; ++i){
				items.push(root.canonical(value[i]))
			}

			return "[" + items.join(",") + "]"
		}

		let keys = Object.keys(value).sort()
		let members = []
		for (let i = 0; i < keys.length; ++i){
			members.push(JSON.stringify(keys[i]) + ":" + root.canonical(value[keys[i]]))
		}

		return "{" + members.join(",") + "}"
	}

	function collectChanges(changeSet){
		root.collected = root.collected.concat(changeSet)
	}

	function startPatch(model){
		root.collected = []
		root.before = JSON.parse(model.toJson())
		model.modelChanged.connect(root.collectChanges)
	}

	// Logs the operations of the patch and whether it rebuilds the model
	function checkPatch(name, model){
		let patch = ChangeSetHelper.toJsonPatch(root.collected, model)
		let operations = []
		for (let i = 0; i < patch.length; ++i){
			operations.push(patch[i].op + " " + patch[i].path)
		}

		let rebuilt = false
		try {
			rebuilt = root.canonical(root.applyPatch(root.before, patch)) === root.canonical(JSON.parse(model.toJson()))
		}
		catch (error){
			console.log(name + " MISMATCH " + error.message)
			root.failures = root.failures + 1

			return
		}

		root.check(name, operations.length > 0 && rebuilt, true)
		console.log(name + " patch " + root.toText(operations))
	}

	function random(count){
		root.seed = (root.seed * 1103515245 + 12345) % 2147483648

		return Math.floor(root.seed / 65536) % count
	}

	function editRandomly(role, step){
		let permissions = role.m_permissions
		let count = permissions ? permissions.count : 0
		let action = root.random(12)
		if (action === 0){
			role.m_name = "name" + step

			return
		}

		if (action === 1 && count > 0){
			let item = root.permission(role, root.random(count))
			item.m_direct = !item.m_direct

			return
		}

		if (action === 2 && count > 0){
			root.permission(role, root.random(count)).m_name = "p" + step

			return
		}

		if ((action === 3 || action === 4) && permissions){
			let added = assignmentComp.createObject(root)
			added.createFromJson('{"id":"n' + step + '","direct":true,"sources":[{"steps":[{"kind":"Role","id":"s","name":"S' + step + '"}]}]}')
			permissions.insertElement(root.random(count + 1), added)

			return
		}

		if (action === 5 && count > 0){
			permissions.removeElement(root.random(count))

			return
		}

		if (action === 6 && count > 1){
			let first = root.random(count)
			let second = (first + 1 + root.random(count - 1)) % count
			permissions.swapItems(first, second)

			return
		}

		if (action === 7 && count > 0){
			let sources = root.permission(role, root.random(count)).m_sources
			if (sources && sources.count > 0){
				let steps = sources.get(root.random(sources.count)).item.m_steps
				if (steps && steps.count > 0){
					steps.get(root.random(steps.count)).item.m_name = "step" + step
				}
			}

			return
		}

		if (action === 8 && count > 0){
			let sources = root.permission(role, root.random(count)).m_sources
			if (sources && sources.count > 0){
				let steps = sources.get(root.random(sources.count)).item.m_steps
				if (steps && steps.count > 0){
					steps.removeElement(root.random(steps.count))
				}
			}

			return
		}

		if (action === 9 && count > 0){
			root.permission(role, root.random(count)).removeSources()

			return
		}

		if (action === 10 && root.random(4) === 0){
			role.emplacePermissions()

			return
		}

		if (action === 11){
			role.m_description = "d" + step
		}
	}

	function testJsonPatch(){
		root.check("jsonPointer", ChangeSetHelper.toJsonPointer(["a/b", "c~d", 2]), "/a~1b/c~0d/2")

		let role = root.createRole()
		root.startPatch(role)
		root.permission(role, 1).m_name = "One"
		role.m_permissions.insertElement(0, root.createAssignment("first"))
		root.permission(role, 3).m_direct = false
		role.m_permissions.removeElement(3)
		root.checkPatch("patch.finalIndexes", role)

		role = root.createRole()
		root.startPatch(role)
		root.permission(role, 0).m_sources.get(0).item.m_steps.removeElement(1)
		role.m_permissions.removeElement(0)
		root.checkPatch("patch.removedElement", role)

		role = root.createRole()
		root.startPatch(role)
		role.removePermissions()
		root.checkPatch("patch.removedMember", role)

		role = root.createRole()
		root.startPatch(role)
		role.createFromJson('{"id":"r1","name":"Other","description":"d","permissions":[{"id":"p9","direct":false}]}')
		let reloadPatch = ChangeSetHelper.toJsonPatch(root.collected, role)
		let reloadPaths = []
		for (let i = 0; i < reloadPatch.length; ++i){
			reloadPaths.push(reloadPatch[i].path)
		}
		root.check("patch.reload", reloadPaths.indexOf("/name") >= 0 && reloadPaths.indexOf("/permissions") < 0, true)

		// The runner gives a test 5 s; the full random run is in Tests/QmlTest/imtcontrols/tst_ModelChangeSet.qml
		for (let i = 1; i <= 6; ++i){
			root.seed = i
			role = root.createRole()
			root.startPatch(role)
			let transaction = i % 2 === 0
			if (transaction){
				role.beginChanges()
			}
			for (let step = 0; step < 20; ++step){
				root.editRandomly(role, step)
			}
			if (transaction){
				role.endChanges()
			}
			root.checkPatch("patch.random" + i, role)
		}
	}

	Component.onCompleted: {
		root.testRootProperty()
		root.testSameValue()
		root.testCompatibilityFields()
		root.testNestedObject()
		root.testNestedObjectReplaced()
		root.testListElementProperty()
		root.testDeepListPath()
		root.testInsertElement()
		root.testInsertedElementReports()
		root.testRemoveElement()
		root.testIndexesAfterRemoval()
		root.testRemovedElementUnknown()
		root.testSwapItems()
		root.testTransactionOrder()
		root.testChildTransaction()
		root.testEmptyTransaction()
		root.testRecursiveTree()
		root.testPrimitiveArray()
		root.testRelativeToListener()
		root.testDisabledNotifications()
		root.testEmplaceAndRemoveKey()
		root.testFromObjectOnFilledModel()
		root.testManualEmptyChangeSet()
		root.testHelpers()
		root.testEscaping()
		root.testJsonPatch()

		console.log("failures " + root.failures)
		Qt.quit()
	}
}
