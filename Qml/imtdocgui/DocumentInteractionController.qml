import QtQuick 2.12
import imtcontrols 1.0

// Centralises the user-facing dialogs the document-service layer needs:
// confirming a dirty close, prompting for a document name, and reporting
// operation errors. Injectable on DocumentService so tests can supply a
// headless stand-in instead of showing real dialogs.
//
// Replaces three near-identical copies of the same confirmation dialog and
// name-input dialog that used to live in MultiDocumentCollectionView,
// SingleDocumentCollectionView and SingleDocumentWorkspaceShellView.
QtObject {
	id: root

	// callback(true)      -> save then close
	// callback(false)     -> close without saving
	// callback(undefined) -> cancel, do not close
	function confirmCloseDirty(callback){
		let dialogCallback = function(result){
			if (result === Enums.yes){
				callback(true)
			}
			else if (result === Enums.no){
				callback(false)
			}
			else{
				callback(undefined)
			}
		}

		ModalDialogManager.showConfirmationDialog(
					qsTr("Save document"),
					qsTr("Save all changes ?"),
					dialogCallback)
	}

	// Prompts for a document name. Calls onNameChosen(name) when the user
	// confirms; does not call it when the user cancels.
	function requestDocumentName(documentId, onNameChosen){
		ModalDialogManager.openDialog(__inputDialogComp, {documentId: documentId, onAccepted: onNameChosen})
	}

	function showError(message){
		if (message && message !== ""){
			ModalDialogManager.showErrorDialog(message)
		}
	}

	property Component __inputDialogComp: Component {
		InputDialog {
			title: qsTr("Document Name")
			placeHolderText: qsTr("Enter the document name")

			property string documentId
			property var onAccepted: null

			onFinished: {
				if (buttonId === Enums.ok && typeof onAccepted === "function"){
					onAccepted(inputValue)
				}
			}
		}
	}
}
