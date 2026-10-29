import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "PermissionsTableView"
	when: windowShown

	Component {
		id: viewComp

		PermissionsTableView {
			width: 600
			height: 800
		}
	}

	Component {
		id: spyComp

		SignalSpy {
			signalName: "selectionChanged"
		}
	}

	// Sensors: View, Edit (requires View), Bind; Users: View, Add (requires Users/View and Sensors/Edit)
	function groups(){
		return [
			{groupId: "Sensors", groupName: "Sensor Management", entries: [
				{permissionId: "/Sensors/View", displayName: "View sensors"},
				{permissionId: "/Sensors/Edit", displayName: "Edit sensors", requirements: ["/Sensors/View"]},
				{permissionId: "/Sensors/Bind", displayName: "Bind sensors"}
			]},
			{groupId: "Users", groupName: "Users", entries: [
				{permissionId: "/Users/View", displayName: "View users", description: "Read the user list"},
				{permissionId: "/Users/Add", displayName: "Add users", requirements: ["/Users/View", "/Sensors/Edit", "/Missing"]}
			]}
		]
	}

	function createView(){
		let view = createTemporaryObject(viewComp, testCase)
		view.rebuildFromFlatArray(groups())
		return view
	}

	function createSpy(view){
		return createTemporaryObject(spyComp, testCase, {target: view})
	}

	function rowOf(view, key){
		return view.__rowsByKey[key][0]
	}

	function stateOf(view, key){
		return view.__rowState[rowOf(view, key)]
	}

	function findItem(item, check){
		if (check(item)){
			return item
		}

		for (let i = 0; i < item.children.length; ++i){
			let found = findItem(item.children[i], check)
			if (found){
				return found
			}
		}

		return null
	}

	function findByName(item, name){
		return findItem(item, function(candidate){ return candidate.objectName === name })
	}

	function test_applySelectionChecksOnlyKnownIds(){
		let view = createView()
		let spy = createSpy(view)

		view.applySelection(["/Sensors/View", "/Sensors/Bind", "/Unknown"])

		compare(view.getCheckedIds(false), ["/Sensors/Bind", "/Sensors/View"])
		compare(view.checkedCount, 2)
		compare(stateOf(view, "__group__:Sensors"), Qt.PartiallyChecked)
		compare(view.__rowBadge[rowOf(view, "__group__:Sensors")], "2/3")
		compare(stateOf(view, "__group__:Users"), Qt.Unchecked)
		compare(spy.count, 0, "programmatic selection is not a user change")
	}

	function test_requirementsAreCheckedTransitivelyAndLocked(){
		let view = createView()

		view.applySelection(["/Users/Add"])

		compare(view.getCheckedIds(false), ["/Sensors/Edit", "/Sensors/View", "/Users/Add", "/Users/View"])
		compare(view.getCheckedIds(true), ["/Users/Add"])
		compare(view.requiredPermissions["/Sensors/View"], ["/Sensors/Edit"])
		compare(view.requiredPermissions["/Missing"], undefined, "requirements outside the tree are skipped")
		compare(view.__rowBadge[rowOf(view, "/Sensors/Edit")], "Required")
		verify(view.__rowLocked[rowOf(view, "/Sensors/Edit")])

		view.__toggleRow(rowOf(view, "/Sensors/Edit"))
		compare(stateOf(view, "/Sensors/Edit"), Qt.Checked, "a required permission cannot be unchecked")

		view.__toggleRow(rowOf(view, "/Users/Add"))
		compare(view.getCheckedIds(false), [], "required permissions go with what required them")
	}

	function test_ownPermissionThatIsRequiredStaysOwn(){
		let view = createView()

		view.applySelection(["/Sensors/View", "/Sensors/Edit"])

		compare(view.getCheckedIds(true), ["/Sensors/Edit", "/Sensors/View"])
		verify(!view.__rowLocked[rowOf(view, "/Sensors/View")])
	}

	function test_lockedPermissions(){
		let view = createView()

		view.setLockedPermissions({"/Users/View": "Admin"})

		compare(stateOf(view, "/Users/View"), Qt.Checked)
		compare(view.getCheckedIds(true), [])
		compare(view.__rowHint[rowOf(view, "/Users/View")], "Inherited from: Admin")
		verify(view.__rowLocked[rowOf(view, "__group__:Users")], "a group with an inherited permission is locked")

		view.uncheckAll()
		compare(stateOf(view, "/Users/View"), Qt.Checked, "uncheckAll keeps inherited permissions")

		view.setLockedPermissions({})
		compare(stateOf(view, "/Users/View"), Qt.Checked, "unlocking keeps the check state")
		compare(view.getCheckedIds(true), ["/Users/View"])
	}

	function test_groupToggleSkipsRequiredPermissions(){
		let view = createView()
		let spy = createSpy(view)
		view.applySelection(["/Users/Add"])

		view.__toggleRow(rowOf(view, "__group__:Sensors"))
		compare(view.getCheckedIds(true), ["/Sensors/Bind", "/Users/Add"], "the required ones stay required")
		compare(stateOf(view, "__group__:Sensors"), Qt.Checked)

		view.__toggleRow(rowOf(view, "__group__:Sensors"))
		compare(view.getCheckedIds(true), ["/Users/Add"])
		compare(stateOf(view, "/Sensors/Edit"), Qt.Checked)
		compare(spy.count, 2, "one signal per user action")
	}

	function test_readOnlyBlocksChanges(){
		let view = createView()
		let spy = createSpy(view)
		view.readOnly = true

		view.__toggleRow(rowOf(view, "/Sensors/Bind"))
		view.checkAll()

		compare(view.checkedCount, 0)
		compare(spy.count, 0)
	}

	function test_filterLimitsRowsAndCheckAll(){
		let view = createView()
		view.collapseAll()
		compare(view.__visibleRows.length, 2)

		view.__filterText = "view"
		view.__applyFilter(true)
		// both groups expanded, only the matching leaves are shown
		compare(view.__visibleRows, [rowOf(view, "__group__:Sensors"), rowOf(view, "/Sensors/View"),
			rowOf(view, "__group__:Users"), rowOf(view, "/Users/View")])

		view.checkAll()
		compare(view.getCheckedIds(true), ["/Sensors/View", "/Users/View"])

		view.__filterText = "read the user"
		view.__applyFilter(true)
		compare(view.__visibleRows, [rowOf(view, "__group__:Users"), rowOf(view, "/Users/View")], "descriptions are searched")

		view.__filterText = "users"
		view.__applyFilter(true)
		compare(view.__visibleRows.length, 3, "a matching group shows all its permissions")
	}

	function test_expandAndCollapseKeepModelInSync(){
		let view = createView()
		let groupRow = rowOf(view, "__group__:Sensors")
		compare(view.__visibleRows.length, 7)

		view.__setExpanded(groupRow, false)
		compare(view.__visibleRows.length, 4)
		compare(view.__visibleRows[1], rowOf(view, "__group__:Users"))

		view.__setExpanded(groupRow, true)
		compare(view.__visibleRows.length, 7)
		compare(view.__visibleRows[1], rowOf(view, "/Sensors/View"))
	}

	function test_rowCheckBoxClick(){
		// The test case item is hidden, so the clicked view lives in the window
		let view = createTemporaryObject(viewComp, testCase.parent)
		view.rebuildFromFlatArray(groups())
		let spy = createSpy(view)
		waitForRendering(view)

		let row = findByName(view, "TreeRow_1")
		verify(row)
		let checkBox = findByName(row, "RowCheckBox")
		verify(checkBox)
		mouseClick(checkBox)

		compare(view.getCheckedIds(true), ["/Sensors/View"])
		compare(spy.count, 1)
		verify(findByName(view, "TreeRow_0").checkState === Qt.PartiallyChecked, "the group row repaints")
	}

	function test_detailsPanelStaysInsideOnALowView(){
		let view = createTemporaryObject(viewComp, testCase.parent)
		view.rebuildFromFlatArray(groups())
		let details = findByName(view, "PermissionDetails")
		verify(details)
		verify(details.visible, "a tall view shows the details")

		view.height = 260
		verify(!details.visible, "the details are hidden instead of covering the list")

		view.height = 800
		verify(details.visible)
		verify(details.y >= 0 && details.y + details.height <= view.height, "the details stay inside the view")
	}

	function test_rebuildResetsState(){
		let view = createView()
		view.applySelection(["/Sensors/View"])
		view.setLockedPermissions({"/Users/View": "Admin"})

		view.rebuildFromFlatArray(groups())

		compare(view.checkedCount, 0)
		compare(view.lockedPermissions, {})
	}

	function test_rebuildFromTree(){
		let view = createTemporaryObject(viewComp, testCase)
		view.rebuild([
			{m_id: "Sensors", m_name: "Sensors", m_children: [
				{m_id: "/Sensors/Edit", m_name: "Edit", m_children: [
					{m_id: "/Sensors/Edit/Add", m_name: "Add"}
				]}
			]}
		])

		compare(view.__names, ["Sensors", "Edit", "Edit / Add"])
		compare(view.__keys, ["__group__:Sensors", "/Sensors/Edit", "/Sensors/Edit/Add"])
	}

	function test_largeSetIsFast(){
		let large = []
		let selected = []
		let locked = ({})
		for (let g = 0; g < 50; ++g){
			let entries = []
			for (let e = 0; e < 60; ++e){
				let id = "/Group" + g + "/Permission" + e
				entries.push({permissionId: id, displayName: "Permission " + e, requirements: e > 0 ? ["/Group" + g + "/Permission0"] : []})
				if (e % 2 === 0)
					selected.push(id)
				if (e % 10 === 1)
					locked[id] = "Parent role"
			}
			large.push({groupId: "Group" + g, groupName: "Group " + g, entries: entries})
		}

		let view = createTemporaryObject(viewComp, testCase)
		let start = Date.now()
		view.rebuildFromFlatArray(large)
		view.setLockedPermissions({})
		view.applySelection(selected)
		view.setLockedPermissions(locked)
		let loadMs = Date.now() - start

		start = Date.now()
		for (let i = 0; i < 20; ++i)
			view.__toggleRow(rowOf(view, "/Group0/Permission2"))
		let toggleMs = (Date.now() - start) / 20

		start = Date.now()
		view.checkAll()
		view.uncheckAll()
		let bulkMs = Date.now() - start

		console.log("PermissionsTableView 3000 permissions: load " + loadMs + " ms, toggle " + toggleMs + " ms, checkAll+uncheckAll " + bulkMs + " ms")
		compare(view.checkedCount, 50 * 6 + 50, "locked ones and the Permission0 they require")
		verify(loadMs < 2000, "load took " + loadMs + " ms")
		verify(toggleMs < 100, "toggle took " + toggleMs + " ms")
	}
}
