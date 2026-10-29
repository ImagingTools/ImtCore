import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0

TestCase {
	id: testCase

	name: "ElementViewFocus"
	when: windowShown

	// Rows linked with KeyNavigation, as the editors do
	Component {
		id: linkedFormComp

		Column {
			width: 500
			property alias first: firstInput
			property alias second: secondInput

			TextInputElementView { id: firstInput; width: 500; name: "First"; KeyNavigation.tab: secondInput }
			TextInputElementView { id: secondInput; width: 500; name: "Second"; KeyNavigation.backtab: firstInput }
		}
	}

	// Rows without any keyboard wiring
	Component {
		id: plainFormComp

		Column {
			width: 500
			property alias first: firstInput
			property alias second: secondInput

			TextInputElementView { id: firstInput; width: 500; name: "First" }
			TextInputElementView { id: secondInput; width: 500; name: "Second" }
		}
	}

	function createForm(comp){
		// TestCase itself is invisible, the form needs a visible parent
		let form = createTemporaryObject(comp, testCase.parent)
		waitForRendering(form)
		return form
	}

	function test_focusingARowFocusesItsField(){
		let form = createForm(plainFormComp)
		form.first.forceActiveFocus()
		verify(form.first.controlItem.activeFocus)
		keyClick(Qt.Key_A)
		compare(form.first.controlItem.text, "a")
	}

	function test_keyNavigationTypesIntoTheNextField(){
		let form = createForm(linkedFormComp)
		form.first.forceActiveFocus()
		keyClick(Qt.Key_Tab)
		verify(form.second.controlItem.activeFocus)
		keyClick(Qt.Key_Z)
		compare(form.second.controlItem.text, "z")
		compare(form.first.controlItem.text, "")

		keyClick(Qt.Key_Backtab, Qt.ShiftModifier)
		verify(form.first.controlItem.activeFocus)
	}

	function test_tabWithoutWiringReachesTheNextField(){
		let form = createForm(plainFormComp)
		form.first.forceActiveFocus()
		keyClick(Qt.Key_Tab)
		verify(form.second.controlItem.activeFocus)
		keyClick(Qt.Key_Backtab, Qt.ShiftModifier)
		verify(form.first.controlItem.activeFocus)
	}
}
