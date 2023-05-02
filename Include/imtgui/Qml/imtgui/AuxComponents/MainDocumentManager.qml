import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: root;

    property var documentManagers: ({})
    property MenuPanel menuPanelRef: null;

    property bool openingDocument: false;
    property string openingDocumentTypeId;

    property var openingDocumentInfo: null;

    Component.onDestruction: {
        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            Events.unSubscribeEvent(typeIds[i] + "CollectionUpdated", root.collectionUpdated);
        }
    }

    function registerDocumentManager(typeId, documentManager){
        console.log("registerDocumentManager", typeId, documentManager);
        root.documentManagers[typeId] = documentManager;

        Events.subscribeEvent(typeId + "CollectionUpdated", root.collectionUpdated);

        return true;
    }

    function collectionUpdated(){
        if (root.openingDocumentInfo != null){

            openDocument(root.openingDocumentInfo["DocumentTypeId"], root.openingDocumentInfo["DocumentId"]);
            root.openingDocumentInfo = null;
        }
    }

    function openDocument(typeId, documentId){
        console.log('openDocument', typeId, documentId)
        let typeIds = Object.keys(root.documentManagers);
        console.log('typeIds', typeIds)
        let index = typeIds.indexOf(typeId);
        if (index >= 0){
            let documentManager = root.documentManagers[typeId];
            if (documentManager){
                let mainCollection = documentManager.getMainCollectionView();
                if (mainCollection !== null){
                    mainCollection.selectItem(documentId, "Order");
                }
            }
            else{
                root.openingDocumentInfo = {"DocumentId": documentId, "DocumentTypeId": typeId}
            }

            if (root.menuPanelRef != null){
                root.menuPanelRef.activePageIndex = index;
            }
        }
    }

    function saveDirtyDocuments(){
        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            let typeId = typeIds[i];
            let documentManager = root.documentManagers[typeId];
            if (documentManager){
                documentManager.saveDirtyDocuments();
            }
        }
    }

    function dirtyDocumentsExists(){
        console.log('dirtyDocumentsExists')
        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            let typeId = typeIds[i];
            console.log('typeId', typeId);
            let documentManager = root.documentManagers[typeId];
            if (documentManager && documentManager !== null){
                console.log('documentManager', documentManager)
                if (documentManager.dirtyDocumentsExists()){
                    return true;
                }
            }
        }

        return false;
    }
}

