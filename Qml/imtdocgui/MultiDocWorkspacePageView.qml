import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtguigql 1.0

DocumentWorkspacePageView {
	id: multiDocPageView
	
	documentManagerView: documentManagerView_
	
	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView_.addFixedView(startItemSourceComp, pageName, "", true)
		}
	}
	
	function setDocumentName(documentId, name){
		documentManagerView_.setDocumentName(documentId, name)
	}
	
	MultiDocWorkspaceView {
		id: documentManagerView_
		anchors.fill: parent
		documentManager: multiDocPageView.documentManager
		visualStatusProvider: GqlBasedObjectVisualStatusProvider {}
	}
}
