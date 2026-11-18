pragma Singleton

import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var documentManagers: ({})

    signal documentOpened(string typeId, string documentId, string documentTypeId);
    signal tryRegisterDocumentService(string typeId, var callback);

    function clear(){
        root.documentManagers = {};
    }

    function getDocumentService(typeId){
        return root.documentManagers[typeId];
    }

	function getDocumentDataByView(viewRef){
		let typeIds = Object.keys(root.documentManagers);
		for (let i = 0; i < typeIds.length; i++){
			let manager = root.documentManagers[typeIds[i]];
			let document = manager.getDocumentDataByView(viewRef);
			if (document != null){
				return document;
			}
		}

		return null;
	}

    function unregisterDocumentService(documentManager){
        let typeIds = Object.keys(root.documentManagers);
        for (let i = 0; i < typeIds.length; i++){
            let manager = root.documentManagers[typeIds[i]];
            if (manager === documentManager){
                delete root.documentManagers[typeIds[i]];
                return true;
            }
        }

        return false;
    }

    function registerDocumentService(typeId, documentManager){
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

    function openDocument(typeId, documentId, documentTypeId){
        if ((!typeId || typeId === "") || (!documentId || documentId === "") || (!documentTypeId || documentTypeId === "")){
            console.error("Unable to open document, please check input parameters");
            return;
        }

        let documentManager = root.documentManagers[typeId];
        if (!documentManager){
            root.tryRegisterDocumentService(typeId, function(result){
                if (result){
                    root.openDocument(typeId, documentId, documentTypeId);
                }
                else{
                    console.error("Unable to open document. Document manager with type-ID: '" , typeId, "' is invalid!");
                }
            })
            return;
        }

        documentOpened(typeId, documentId, documentTypeId);

        documentManager.openDocument(documentId, documentTypeId);
    }

    // Creates a new document on the page typeId, loading the page first if it is not open yet.
    // createdCallback(documentId) is optional; it is called once the server document service has created the document.
    function insertNewDocument(typeId, documentTypeId, createdCallback){
        if (!typeId || typeId === "" || !documentTypeId || documentTypeId === ""){
            console.error("Unable to create a document, please check input parameters");
            return;
        }

        let documentManager = root.documentManagers[typeId];
        if (!documentManager){
            root.tryRegisterDocumentService(typeId, function(result){
                if (result && root.documentManagers[typeId]){
                    root.insertNewDocument(typeId, documentTypeId, createdCallback);
                }
                else{
                    console.error("Unable to create a document. Document manager with type-ID: '" , typeId, "' is invalid!");
                }
            })
            return;
        }

        documentOpened(typeId, "", documentTypeId);

        // A page on the server document service creates the document there.
        if (documentManager.createDocument){
            if (createdCallback){
                let createFailedFunc = null;
                let createdFunc = function(documentId, createdTypeId){
                    if (createdTypeId !== documentTypeId){
                        return;
                    }

                    documentManager.documentCreated.disconnect(createdFunc);
                    documentManager.createDocumentFailed.disconnect(createFailedFunc);
                    createdCallback(documentId);
                }
                createFailedFunc = function(failedTypeId, message){
                    documentManager.documentCreated.disconnect(createdFunc);
                    documentManager.createDocumentFailed.disconnect(createFailedFunc);
                }

                documentManager.documentCreated.connect(createdFunc);
                documentManager.createDocumentFailed.connect(createFailedFunc);
            }

            documentManager.createDocument(documentTypeId);
        }
        else{
            documentManager.insertNewDocument(documentTypeId);
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

