pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    property var documentManagers: ({})

    signal documentOpened(string typeId, string documentId, string documentTypeId);

    function clear(){
        root.documentManagers = {};
    }

    function getDocumentManager(typeId){
        return root.documentManagers[typeId]["DocumentManager"];
    }

    function registerDocumentManager(typeId, documentManager, defaultDocumentTypeId, defaultViewTypeId){
        if (!defaultDocumentTypeId){
            defaultDocumentTypeId = ""
        }

        if (!defaultViewTypeId){
            defaultViewTypeId = ""
        }

        let obj = {}
        obj["DocumentManager"] = documentManager;
        obj["DefaultDocumentTypeId"] = defaultDocumentTypeId;
        obj["DefaultViewTypeId"] = defaultViewTypeId;

        root.documentManagers[typeId] = obj;

        return true;
    }

    function registerDefaultDocumentData(typeId, defaultDocumentTypeId, defaultViewTypeId){
        if (typeId in root.documentManagers){
            root.documentManagers[typeId]["DefaultDocumentTypeId"] = defaultDocumentTypeId;
            root.documentManagers[typeId]["DefaultViewTypeId"] = defaultViewTypeId;
        }
    }

    function openDocument(typeId, documentId, documentTypeId, viewTypeId){
        documentOpened(typeId, documentId, documentTypeId);

        if (typeId in root.documentManagers){
            if (!documentTypeId){
                documentTypeId = root.documentManagers[typeId]["DefaultDocumentTypeId"]
            }

            if (!viewTypeId){
                viewTypeId = root.documentManagers[typeId]["DefaultViewTypeId"]
            }

            let documentManager = root.documentManagers[typeId]["DocumentManager"];
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

