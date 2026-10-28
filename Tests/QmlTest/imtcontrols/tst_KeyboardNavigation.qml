import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0

TestCase {
	id: testCase

	name: "KeyboardNavigation"
	when: windowShown

	Component {
		id: formComp

		Column {
			width: 400
			spacing: 4

			property var controls: [firstField, secondField, textField, textArea, comboBox, checkBox, switchControl, spinBox, button]
			property alias readOnlyField: readOnlyField
			property int buttonClicks: 0

			CustomTextField { id: firstField; objectName: "FirstField"; width: 300 }
			CustomTextField { id: readOnlyField; objectName: "ReadOnlyField"; width: 300; readOnly: true }
			CustomTextField { id: secondField; objectName: "SecondField"; width: 300 }
			TextField { id: textField; objectName: "TextField"; width: 300 }
			TextArea { id: textArea; objectName: "TextArea"; width: 300; height: 60 }
			ComboBox { id: comboBox; objectName: "ComboBox"; width: 300 }
			CheckBox { id: checkBox; objectName: "CheckBox"; text: "Check" }
			SwitchCustom { id: switchControl; objectName: "Switch"; text: "Switch" }
			// The test style has no spin box decorator, so the one of the application style is set here
			SpinBox { id: spinBox; objectName: "SpinBox"; startValue: 5; decorator: Component { SpinBoxComp {} } }
			Button { id: button; objectName: "Button"; text: "Button"; onClicked: parent.buttonClicks++ }
		}
	}

	function createForm(){
		// TestCase itself is invisible, the form needs a visible parent
		let form = createTemporaryObject(formComp, testCase.parent)
		waitForRendering(form)
		return form
	}

	function focusedName(form){
		let all = form.controls.concat([form.readOnlyField])
		for (let i = 0; i < all.length; i++){
			if (all[i].activeFocus)
				return all[i].objectName
		}

		return "none"
	}

	function test_tabVisitsEveryEditableControlInOrder(){
		let form = createForm()
		form.controls[0].forceActiveFocus()
		for (let i = 1; i < form.controls.length; i++){
			keyClick(Qt.Key_Tab)
			compare(focusedName(form), form.controls[i].objectName, "Tab " + i)
		}
	}

	function test_backtabVisitsThemInReverse(){
		let form = createForm()
		let last = form.controls.length - 1
		form.controls[last].forceActiveFocus()
		for (let i = last - 1; i >= 0; i--){
			keyClick(Qt.Key_Backtab, Qt.ShiftModifier)
			compare(focusedName(form), form.controls[i].objectName, "Shift+Tab to " + i)
		}
	}

	function test_readOnlyFieldIsSkipped(){
		let form = createForm()
		form.controls[0].forceActiveFocus()
		keyClick(Qt.Key_Tab)
		compare(focusedName(form), "SecondField")
	}

	function test_tabInATextAreaMovesOn(){
		let form = createForm()
		form.controls[3].forceActiveFocus()
		keyClick(Qt.Key_Tab)
		compare(form.controls[3].text, "", "no tab character is typed")
		compare(focusedName(form), "ComboBox")
	}

	function test_spaceTogglesACheckBox(){
		let form = createForm()
		let checkBox = form.controls[5]
		checkBox.forceActiveFocus()
		keyClick(Qt.Key_Space)
		compare(checkBox.checkState, Qt.Checked)
		keyClick(Qt.Key_Space)
		compare(checkBox.checkState, Qt.Unchecked)
	}

	function test_spaceTogglesASwitch(){
		let form = createForm()
		let switchControl = form.controls[6]
		switchControl.forceActiveFocus()
		keyClick(Qt.Key_Space)
		tryCompare(switchControl, "checked", true)
		keyClick(Qt.Key_Space)
		tryCompare(switchControl, "checked", false)
	}

	function test_arrowsStepASpinBox(){
		let form = createForm()
		let spinBox = form.controls[7]
		form.controls[6].forceActiveFocus()
		keyClick(Qt.Key_Tab)
		compare(focusedName(form), "SpinBox", "the spin box field is reached by Tab")
		keyClick(Qt.Key_Up)
		compare(spinBox.value, 6)
		keyClick(Qt.Key_Down)
		keyClick(Qt.Key_Down)
		compare(spinBox.value, 4)
	}

	function test_enterAndSpaceClickAButton(){
		let form = createForm()
		form.controls[8].forceActiveFocus()
		keyClick(Qt.Key_Return)
		keyClick(Qt.Key_Space)
		compare(form.buttonClicks, 2)
	}
}
