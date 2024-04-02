pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    property var documentManagers: ({})

    signal documentOpened(string typeId, string documentId, string documentTypeId);

    Component.onCompleted: {
        Events.subscribeEvent("RegisterDocumentManager", registerDocumentManager);
        Events.subscribeEvent("GetDocumentManager", registerDocumentManager);
        Events.subscribeEvent("OpenDocument", openDocument);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("RegisterDocumentManager", registerDocumentManager);
        Events.unSubscribeEvent("OpenDocument", openDocument);
    }

    function clear(){
        root.documentManagers = {};
    }

    function getDocumentManager(typeId){
        return root.documentManagers[typeId];
    }

    function registerDocumentManager(typeId, documentManager){
        root.documentManagers[typeId] = documentManager;
        return true;
    }

    function openDocument(parameters){
        console.log("main openDocument", parameters);

        let typeId = parameters["TypeId"];
        let documentId = parameters["DocumentId"];
        let documentTypeId = parameters["DocumentTypeId"];
        let viewTypeId = parameters["ViewTypeId"];

        documentOpened(typeId, documentId ,documentTypeId);

        if (typeId in root.documentManagers){
            let documentManager = root.documentManagers[typeId];
            if (documentManager){
                documentManager.openDocument(documentId, documentTypeId, viewTypeId);
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
        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            let typeId = typeIds[i];
            let documentManager = root.documentManagers[typeId];
            if (documentManager && documentManager !== null){
                if (documentManager.dirtyDocumentsExists()){
                    return true;
                }
            }
        }

        return false;
    }

    property var documents;

    function closeAllDocuments(){
        documents = {}

        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            let typeId = typeIds[i];
            let documentManager = root.documentManagers[typeId];
            if (documentManager && documentManager !== null){
                let documentIds = documentManager.getDocumentIDs();

                documents[typeId] = documentIds;
            }
        }

        closeDocument();
    }

    function closeDocument(){
        console.log("closeDocument");
        let keys = Object.keys(documents);
        if (keys.length > 0){
            let typeId = keys[0];
            let documentManager = root.documentManagers[typeId];
            if (documentManager){
                let documentIds = root.documents[typeId];

                if (documentIds.length > 0){
                    let documentId = documentIds[documentIds.length - 1];

                    documentManager.closeDocument(documentId, false,
                                                  function (result){
                                                      console.log("closing result", result);
                                                      if (result){
                                                          root.documents[typeId].pop();

                                                          if (root.documents[typeId].length === 0){
                                                              delete root.documents[typeId];
                                                          }

                                                          closeDocument();
                                                      }
                                                  }
                                                  );
                }
            }
        }
    }
}

