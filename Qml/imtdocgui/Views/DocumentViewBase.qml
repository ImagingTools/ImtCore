import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0

ViewBase {
	id: root;

	objectName: "DocumentViewBase"

	property string documentId;
	property string documentTypeId;
	property var documentManager: null; // DocumentServiceBase
	property Item documentManagerView: null;
	property DocumentRepresentationController representationController: null;

	signal documentSaved()
}

