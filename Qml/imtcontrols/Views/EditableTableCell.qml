import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// One cell of an inline-editable table row. It is pure text - no mouse area of
// its own - until the row it belongs to is switched into edit mode, so clicking
// a row always reaches the row itself and only ever changes the selection.
//
// The cell never writes to the model. While the row is open it only holds a
// draft; the row reads every draft back with pendingValue() and applies them
// together, so one edit produces one change and Cancel produces none.
Item {
	id: cell

	property string value: ""
	property string placeHolderText: ""
	// Shown instead of an empty cell while the row is closed, so a column with
	// nothing in it still reads as an answer rather than as a gap.
	property string emptyText: ""
	property bool readOnly: false
	property bool bold: false
	property bool editing: false
	// A value that has to be filled in. While it is empty the cell shows its
	// placeholder in the error colour and the field keeps a red border, which
	// is what points at the blanks of a freshly created row.
	property bool required: false
	property bool missing: cell.required && (cell.editing ? cell.draft === "" : cell.value === "")
	// Live text of the open editor - the row watches it to derive other cells.
	property string draft: editor.text
	property real textInset: Style.marginXS
	// Tab targets. They are the neighbours' editors, not the neighbouring cells,
	// because focus has to land on the text input itself.
	property alias editorItem: editor
	property Item nextEditor: null
	property Item previousEditor: null

	// Enter: commit the row and leave edit mode.
	signal editingEndRequested()
	// Escape: drop the whole row's drafts, not just this cell's.
	signal editingCancelRequested()

	function focusEditor() {
		if (cell.editing)
			editor.forceActiveFocus()
	}

	function pendingValue() {
		return cell.editing ? editor.text : cell.value
	}

	function setDraft(newValue) {
		editor.text = newValue
	}

	onEditingChanged: {
		if (cell.editing)
			editor.text = cell.value
	}

	BaseText {
		anchors.left: parent.left
		anchors.leftMargin: cell.textInset
		anchors.right: parent.right
		anchors.rightMargin: cell.textInset
		anchors.verticalCenter: parent.verticalCenter
		visible: !cell.editing
		text: cell.value !== "" ? cell.value
			: cell.emptyText !== "" ? cell.emptyText : cell.placeHolderText
		font.family: cell.bold ? Style.fontFamilyBold : Style.fontFamily
		color: cell.value !== "" ? Style.textColor
			: cell.missing ? Style.errorTextColor
			: cell.emptyText !== "" ? Style.inactiveTextColor : Style.placeHolderTextColor
		elide: Text.ElideRight
	}

	// Marks a still empty required cell while the row is not being edited,
	// where there is no field border to carry the message.
	Rectangle {
		anchors.fill: parent
		visible: cell.missing && !cell.editing
		radius: Style.radiusM
		color: "transparent"
		border.width: 1
		border.color: Style.errorTextColor
		opacity: Style.opacityHigh
	}

	CustomTextField {
		id: editor
		anchors.fill: parent
		visible: cell.editing
		readOnly: cell.readOnly
		margin: cell.textInset
		radius: Style.radiusM
		borderColorConst: cell.missing ? Style.errorTextColor : ""
		autoEditingFinished: false
		placeHolderText: cell.placeHolderText
		KeyNavigation.tab: cell.nextEditor
		KeyNavigation.backtab: cell.previousEditor

		// Both routes end in the same idempotent request: the signal fires when
		// the text input consumes the key itself, the handler below when it
		// lets it through. Marking the event accepted is what stops an Enter
		// from closing the row and then opening the level underneath it.
		onAccepted: cell.editingEndRequested()

		Keys.onPressed: {
			if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
				event.accepted = true
				cell.editingEndRequested()
			}
			else if (event.key === Qt.Key_Escape) {
				event.accepted = true
				cell.editingCancelRequested()
			}
		}
	}
}
