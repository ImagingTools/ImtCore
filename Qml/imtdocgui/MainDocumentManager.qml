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
        return root.documentManagers[typeId];
    }

    function registerDocumentManager(typeId, documentManager){
        if (!typeId || typeId === ""){
            console.error("Unable to register document manager. Type-ID is invalid!");
            return false;
        }

        if (!documentManager){
            console.error("Unable to register document manager. Reference on the document manager is invalid!");
            return false;
        }

        root.documentManagers[typeId] = documentManager;

        return true;
    }

    function openDocument(typeId, documentId, documentTypeId, viewTypeId){
        if ((!typeId || typeId === "") || (!documentId || documentId === "") || (!documentTypeId || documentTypeId === "")){
            console.error("Unable to open document, please check input parameters");
            return;
        }

        if (!(typeId in root.documentManagers)){
            console.error("Unable to open document. Document manager with Type-ID: '" , typeId, "' unregistered!");
            return;
        }

        let documentManager = root.documentManagers[typeId];
        if (!documentManager){
            console.error("Unable to open document. Document manager with Type-ID: '" , typeId, "' is invalid!");
            return;
        }

        if (!viewTypeId || viewTypeId === ""){
            let viewTypeIds = documentManager.getViewTypeIds(documentTypeId);
            if (viewTypeIds.length === 0){
                console.error("Unable to open document. View none are registered for the manager with Type-ID: ", typeId);
                return;
            }

            viewTypeId = viewTypeIds[0];
        }

        documentOpened(typeId, documentId, documentTypeId);

        documentManager.openDocument(documentId, documentTypeId, viewTypeId);
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

