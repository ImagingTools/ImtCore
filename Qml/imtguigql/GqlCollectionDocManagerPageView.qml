import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0

PageContainer {
	id: root

	property DocumentManagerBase documentManager: GqlBasedCollectionDocumentManager {
		collectionId: root.pageId
	}

	onPageIdChanged: {
		if (pageId !== ""){
			MainDocumentManager.registerDocumentManager(pageId, documentManager)
		}
	}

	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView_.setCollectionViewComp(pageName, startItemSourceComp)
		}
	}

	onVisibleChanged: {
		checkCreation();
	}
	
	onStartItemSourceChanged: {
		checkCreation();
	}
	
	function checkCreation(){
		if (!startItemIsCreated()){
			createStartItemComp();
		}
	}

	function setDocumentName(documentId, name){}

	MultiDocumentCollectionView {
		id: documentManagerView_
		anchors.fill: root
		documentManager: root.documentManager
		visualStatusProvider: GqlBasedObjectVisualStatusProvider {
			collectionId: root.pageId
		}
	}
}
