import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0

TestCase {
	id: testCase

	name: "TabPanel"
	when: windowShown

	Component {
		id: panelComp

		TabPanel {
			width: 800

			model: ListModel {
			}

			tabDelegateDecorator: Component {
				TabPanelDecorator {
				}
			}
		}
	}

	function createPanel(tabCount){
		// TestCase itself is invisible, the panel needs a visible parent
		let panel = createTemporaryObject(panelComp, testCase.parent)
		for (let i = 0; i < tabCount; ++i){
			addTab(panel, i)
		}

		return panel
	}

	function addTab(panel, index){
		panel.model.append({"id": "tab" + index, "name": "Tab " + index, "icon": "", "description": "", "waitName": false, "pinned": index === 0, "loading": false})
	}

	function decoratorAt(panel, index){
		// The strip creates its tabs on its first layout, not when the model changes
		tryVerify(function(){ return panel.tabsList.itemAtIndex(index) !== null }, 1000, "Tab " + index + " is created")
		let tab = panel.tabsList.itemAtIndex(index)

		return tab.decorator_
	}

	function test_singleTabIsSelected(){
		let panel = createPanel(1)

		let decorator = decoratorAt(panel, 0)
		verify(decorator.isSelected, "A single tab is selected like any other")
	}

	function test_selectionWithSeveralTabs(){
		let panel = createPanel(2)
		panel.selectedIndex = 1

		verify(!decoratorAt(panel, 0).isSelected)
		verify(decoratorAt(panel, 1).isSelected)
	}

	function test_addedTabKeepsSelection(){
		let panel = createPanel(1)

		addTab(panel, 1)

		verify(decoratorAt(panel, 0).isSelected, "Still selected")
		verify(!decoratorAt(panel, 1).isSelected)
	}

	function test_tabsAreAdjacent(){
		let panel = createPanel(3)
		decoratorAt(panel, 2)

		for (let i = 1; i < 3; ++i){
			let previous = panel.tabsList.itemAtIndex(i - 1)
			compare(panel.tabsList.itemAtIndex(i).x, previous.x + previous.width, "Tab " + i + " starts where the previous one ends")
		}
	}
}
