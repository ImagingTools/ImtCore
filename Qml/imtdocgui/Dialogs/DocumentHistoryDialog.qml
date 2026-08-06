import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Dialog {
	id: documentHistoryDialog;
	
	property string documentId: "";

	// Set on the dialog itself (not on the content item) and mirrored by the
	// content below - matching ProfileTokensPage's addDialog, which is what
	// actually keeps this reactive to live window resizes.
	width: Math.max(Style.sizeHintXXL, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXXXL));
	height: Math.max(Style.sizeHintXL, Math.min(ModalDialogManager.activeView.height - 100, Style.sizeHintXXL));

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("OK"), true)
	}

	contentComp: Component {
		id: productPairEditor;

		DocumentHistoryView {
			width: documentHistoryDialog.width;
			// The dialog's own height above is the TOTAL popup size; the title bar
			// and button row are stacked above/below this content (~120px), so the
			// content must be shorter than the dialog by that much - otherwise the
			// whole popup grows past what was computed as "fits the screen" and
			// the button row is pushed off it.
			height: documentHistoryDialog.height - 120;
			documentId: documentHistoryDialog.documentId;
		}
	}
}

