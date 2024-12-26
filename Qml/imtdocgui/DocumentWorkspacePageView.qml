import QtQuick 2.12
import Acf 1.0
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

    function onMenuModelChanged(model){
        let id = root.pageId;
        for(let i = 0; i < model.getItemsCount(); i++){
            let curr_id = model.getData("Id",i);
            let curr_name = model.getData("Name",i);
            if(curr_id == id && curr_name && documentManager.documentsModel){
                documentManager.documentsModel.setProperty(0, "Title", curr_name);
                break;
            }
        }
    }
}
