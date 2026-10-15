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
		let tab = panel.tabsList.itemAtIndex(index)
		verify(tab !== null, "Tab " + index + " is created")

		return tab.decorator_
	}

	function test_singleTabIsSelected(){
		let panel = createPanel(1)

		let decorator = decoratorAt(panel, 0)
		verify(decorator.isSelected, "A single tab is selected like any other")
		verify(decorator.isLastTab)
	}

	function test_selectionWithSeveralTabs(){
		let panel = createPanel(2)
		panel.selectedIndex = 1

		verify(!decoratorAt(panel, 0).isSelected)
		verify(decoratorAt(panel, 1).isSelected)
	}

	function test_dividerAfterEveryTabButTheLast(){
		let panel = createPanel(4)

		for (let i = 0; i < 3; ++i){
			verify(!decoratorAt(panel, i).isLastTab, "Divider after tab " + i)
			verify(decoratorAt(panel, i).dividerGap > 0)
		}
		verify(decoratorAt(panel, 3).isLastTab, "No divider after the last tab")
		compare(decoratorAt(panel, 3).dividerGap, 0)
	}

	function test_addedTabGivesThePreviousLastOneADivider(){
		let panel = createPanel(1)
		verify(decoratorAt(panel, 0).isLastTab)

		addTab(panel, 1)

		verify(!decoratorAt(panel, 0).isLastTab)
		verify(decoratorAt(panel, 0).isSelected, "Still selected")
	}
}
