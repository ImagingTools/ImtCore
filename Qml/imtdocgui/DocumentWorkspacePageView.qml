import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0

PageContainer {
	id: root;
	
	property DocumentManager documentManager: DocumentManager {
		activeView: root.documentManagerView;
	};
	
	property Item documentManagerView: null;
	
	Component.onCompleted: {
		Events.subscribeEvent("MenuModelChanged", root.onMenuModelChanged);
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("MenuModelChanged", root.onMenuModelChanged);
	}
	
	onPageIdChanged: {
		if (documentManager){
			MainDocumentManager.unregisterDocumentManager(documentManager);
			MainDocumentManager.registerDocumentManager(pageId, documentManager);
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
	
	function onMenuModelChanged(model){
		let id = root.pageId;
		for(let i = 0; i < model.getItemsCount(); i++){
			let curr_id = model.getData("id",i);
			let curr_name = model.getData("name",i);
			if(curr_id == id && curr_name && documentManager.documentsModel){
				root.setDocumentName(curr_id, curr_name)
				break;
			}
		}
	}
}
