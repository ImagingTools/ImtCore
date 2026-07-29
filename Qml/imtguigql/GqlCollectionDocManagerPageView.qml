import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtdocgui 1.0

PageContainer {
	id: root

	property DocumentServiceBase documentManager: GqlBasedCollectionDocumentService {
		collectionId: root.pageId
	}

	Component.onCompleted: {
		console.log("GqlCollectionDocManagerPageView.qml onCompleted", root.pageId)
	}

	onPageIdChanged: {
		if (pageId !== ""){
			MainDocumentService.registerDocumentService(pageId, documentManager)
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
	}
}
