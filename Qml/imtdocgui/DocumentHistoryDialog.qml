import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: documentHistoryDialog;
	
	property string documentId: "";
	
	Component.onCompleted: {
		addButton(Enums.ok, qsTr("OK"), true)
	}
	
	contentComp: Component {
		id: productPairEditor;
		
		DocumentHistoryView {
			width: 500;
			height: 500;
			documentId: documentHistoryDialog.documentId;
		}
	}
}

