import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtguigql 1.0

DocumentWorkspacePageView {
	id: root
	
	documentManagerView: documentManagerView
	
	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView.addInitialItem(startItemSourceComp, pageName)
		}
	}
	
	SingleDocumentWorkspaceView {
		id: documentManagerView
		anchors.fill: parent
		documentManager: root.documentManager
	}
}
