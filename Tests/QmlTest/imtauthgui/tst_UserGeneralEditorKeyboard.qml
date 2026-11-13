import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtauthUsersSdl 1.0

TestCase {
	id: testCase

	name: "UserGeneralEditorKeyboard"
	when: windowShown

	Component {
		id: editorComp

		UserGeneralEditor {
			width: 800
			userData: UserData {}
		}
	}

	function createEditor(){
		// TestCase itself is invisible, the editor needs a visible parent
		let editor = createTemporaryObject(editorComp, testCase.parent)
		waitForRendering(editor)
		return editor
	}

	function focusedField(editor){
		let fields = [editor.usernameInput, editor.nameInput, editor.mailInput, editor.passwordInput, editor.confirmPasswordInput]
		for (let i = 0; i < fields.length; i++){
			if (fields[i].controlItem && fields[i].controlItem.activeFocus)
				return fields[i].objectName
		}

		return "other"
	}

	// The fields used to be chained in a ring: Tab from the e-mail went back to the user name
	function test_tabGoesThroughTheFieldsInOrder(){
		let editor = createEditor()
		verify(editor.passwordInput.visible, "a new user shows the password fields")
		editor.usernameInput.forceActiveFocus()
		let order = [focusedField(editor)]
		for (let i = 0; i < 4; i++){
			keyClick(Qt.Key_Tab)
			order.push(focusedField(editor))
		}

		compare(order, [editor.usernameInput.objectName, editor.nameInput.objectName, editor.mailInput.objectName,
			editor.passwordInput.objectName, editor.confirmPasswordInput.objectName])
	}

	function test_typingAfterTabGoesIntoTheField(){
		let editor = createEditor()
		editor.usernameInput.forceActiveFocus()
		keyClick(Qt.Key_Tab)
		keyClick(Qt.Key_J)
		compare(editor.nameInput.controlItem.text, "j")
	}
}
