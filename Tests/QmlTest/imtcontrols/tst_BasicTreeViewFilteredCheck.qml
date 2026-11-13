import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0

TestCase {
	id: testCase

	name: "BasicTreeViewFilteredCheck"
	when: windowShown

	Component {
		id: treeComp

		BasicTreeView {
			width: 400
			height: 400
			tristate: true
			filterRole: ["name"]
			filterDebounceMs: 0
			columns: [{id: "name", name: "Name", tree: true}]
		}
	}

	function leaf(key, name){
		return TreeModelBuilder.node({key: key, text: name, checkable: true, expanded: true, children: [], data: {id: key, name: name}})
	}

	function createTree(){
		let tree = createTemporaryObject(treeComp, testCase)
		tree.model = [
			TreeModelBuilder.node({key: "sensors", text: "Sensor Management", checkable: true, expanded: true,
				children: [leaf("/Sensors/View", "View sensors"), leaf("/Sensors/Bind", "Bind")], data: {id: "sensors", name: "Sensor Management"}}),
			TreeModelBuilder.node({key: "users", text: "Users", checkable: true, expanded: true,
				children: [leaf("/Users/View", "View users"), leaf("/Users/Add", "Add")], data: {id: "users", name: "Users"}})
		]

		return tree
	}

	function setFilter(tree, text){
		tree.filterText = text
		tryVerify(function(){ return tree.filterText === text }, 1000)
		wait(50)
		tree.buildVisibleTree()
	}

	function checkedLeaves(tree){
		return tree.getCheckedKeys().filter(function(key){ return key.indexOf("/") === 0 }).sort()
	}

	// The filter matches the group itself: checking it checks its whole content, also the rows the filter hides
	function test_checkingAMatchingGroupChecksItsContent(){
		let tree = createTree()
		setFilter(tree, "sensor management")

		tree.setCheckState("sensors", Qt.Checked)

		compare(checkedLeaves(tree), ["/Sensors/Bind", "/Sensors/View"])
	}

	// The filter matches some rows: checking their group checks only them, even with the group collapsed
	function test_checkingAGroupChecksOnlyItsMatchingRows(){
		let tree = createTree()
		setFilter(tree, "view")
		tree.collapseNode("sensors")

		tree.setCheckState("sensors", Qt.Checked)

		compare(checkedLeaves(tree), ["/Sensors/View"])
	}

	function test_checkAllUnderTheFilterChecksWhatMatches(){
		let tree = createTree()
		setFilter(tree, "view")

		tree.checkAll()

		compare(checkedLeaves(tree), ["/Sensors/View", "/Users/View"])
	}
}
