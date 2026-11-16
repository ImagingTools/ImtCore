import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtguigql 1.0

DocumentWorkspacePageView {
	id: multiDocPageView

	documentManagerView: documentManagerView_

	// Collection shown by the page; a view may keep its own collection under a page with another ID.
	property string collectionId: multiDocPageView.pageId

	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView_.addFixedView(startItemSourceComp, pageName, pageId, true, true, pageIcon)
		}
	}

	onPageIconChanged: {
		documentManagerView_.setTabIcon(pageId, pageIcon)
	}

	function setDocumentName(documentId, name){
		documentManagerView_.setDocumentName(documentId, name)
	}

	MultiDocWorkspaceView {
		id: documentManagerView_
		anchors.fill: parent
		documentManager: multiDocPageView.documentManager
		defaultDocumentIcon: multiDocPageView.pageIcon
		visualStatusProvider: GqlBasedObjectVisualStatusProvider {
			collectionId: multiDocPageView.collectionId
		}

		onViewLoaded: {
			if (view && viewId === multiDocPageView.pageId){
				view.collectionId = multiDocPageView.pageId
				multiDocPageView.collectionId = view.collectionId
			}
		}
	}
}
