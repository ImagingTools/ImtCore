import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

Dialog {
	id: documentRevisionDialog;
	title: qsTr("Revisions (%1)").arg(documentRevisionDialog.contentItem ? documentRevisionDialog.contentItem.revisionsCount : 0);
	canMove: false;
	backgroundColor: Style.baseColor;

	// Set on the dialog itself (not on the content item) and mirrored by the
	// content below - matching ProfileTokensPage's addDialog, which is what
	// actually keeps this reactive to live window resizes.
	width: Math.max(Style.sizeHintXXL, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXXXL));
	height: Math.max(Style.sizeHintXL, Math.min(ModalDialogManager.activeView.height - 100, Style.sizeHintXXL));

	notClosingButtons: Enums.no | Enums.save;

	property string collectionId;
	property string documentId;
	property int selectedRevision: -1;

	Component.onCompleted: {
		if (PermissionsController.checkPermission("RestoreRevision")){
			addButton(Enums.save, qsTr("Set this revision"), false)
		}

		if (PermissionsController.checkPermission("DeleteRevision")){
			addButton(Enums.no, qsTr("Delete revision"), false)
		}

		addButton(Enums.cancel, qsTr("Close"), true)
	}

	onFinished: {
		if (buttonId == Enums.save){
			setRevisionRequest.send();
		}
		else if (buttonId == Enums.no){
			ModalDialogManager.openDialog(removeRevisionConfirmDialog, {});
		}
	}

	// The content Loader (and this dialog's own documentId/collectionId) are not
	// guaranteed to be settled yet at Component.onCompleted for modal content;
	// started() fires once the dialog is actually up, so fetch there instead.
	onStarted: {
		if (documentRevisionDialog.contentItem)
			documentRevisionDialog.contentItem.refresh();
	}

	function getHeaders(){
		return {};
	}

	function refresh(){
		documentRevisionDialog.selectedRevision = -1;
		documentRevisionDialog.buttons.setButtonState(Enums.save, false);
		documentRevisionDialog.buttons.setButtonState(Enums.no, false);
		if (documentRevisionDialog.contentItem)
			documentRevisionDialog.contentItem.refresh();
	}

	GqlSdlRequestSender {
		id: setRevisionRequest;
		requestType: 1;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_restoreRevision;
		inputObjectComp: Component {
			RestoreRevisionInput {
				m_objectId: documentRevisionDialog.documentId;
				m_revision: documentRevisionDialog.selectedRevision;
				m_collectionId: documentRevisionDialog.collectionId;
			}
		}

		sdlObjectComp: Component { RestoreRevisionResponse {
				onFinished: {
					documentRevisionDialog.refresh()
					PopupManager.addSuccessMessage(qsTr("The document revision has been successfully set"))
				}
			}
		}

		function getHeaders(){
			return {};
		}
	}

	GqlSdlRequestSender {
		id: deleteRevisionRequest;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_deleteRevision;
		inputObjectComp: Component {
			DeleteRevisionInput {
				m_objectId: documentRevisionDialog.documentId;
				m_revision: documentRevisionDialog.selectedRevision;
				m_collectionId: documentRevisionDialog.collectionId;
			}
		}

		sdlObjectComp: Component { DeleteRevisionResponse {
				onFinished: {
					documentRevisionDialog.refresh()
					PopupManager.addSuccessMessage(qsTr("The document revision has been successfully delete"));
				}
			}
		}

		function getHeaders(){
			return documentRevisionDialog.getHeaders();
		}
	}

	Component {
		id: removeRevisionConfirmDialog;
		MessageDialog {
			width: Style.sizeHintM;
			title: qsTr("Remove revision");
			message: qsTr("Delete the selected revision ?");
			onFinished: {
				if (buttonId == Enums.yes){
					deleteRevisionRequest.send()
				}
			}
		}
	}

	contentComp: Component {
		Item {
			id: contentItem;
			width: documentRevisionDialog.width;
			// The dialog's own height above is the TOTAL popup size; the title bar
			// and button row are stacked above/below this content (~120px), so the
			// content must be shorter than the dialog by that much - otherwise the
			// whole popup grows past what was computed as "fits the screen" and
			// the button row is pushed off it.
			height: documentRevisionDialog.height - 120;

			property alias revisionsCount: historyView.revisionsCount;

			function refresh(){
				historyView.sendRequest();
			}

			function selectRevision(revision){
				documentRevisionDialog.selectedRevision = revision;

				let isActive = revision === historyView.activeRevision;
				documentRevisionDialog.buttons.setButtonState(Enums.save, !isActive);
				documentRevisionDialog.buttons.setButtonState(Enums.no, !isActive);
			}

			Rectangle {
				anchors.fill: parent;
				color: Style.baseColor;
			}

			DocumentHistoryView {
				id: historyView;
				anchors.fill: parent;
				anchors.margins: Style.marginL;
				documentId: documentRevisionDialog.documentId;
				collectionId: documentRevisionDialog.collectionId;
				selectable: true;
				selectedRevision: documentRevisionDialog.selectedRevision;

				onRevisionSelected: {
					contentItem.selectRevision(revision);
				}

				function getHeaders(){
					return documentRevisionDialog.getHeaders();
				}
			}
		}
	}
}
