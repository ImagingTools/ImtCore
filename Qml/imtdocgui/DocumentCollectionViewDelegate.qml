import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionViewCommandsDelegateBase {
    id: root;

    property DocumentManager documentManager: null;

    property string documentTypeId; // default document type-ID
    property string viewTypeId; // default document view type-ID

    onCollectionViewChanged: {
        if (collectionView && collectionView.dataController){
            collectionView.dataController.removed.connect(internal.onRemoved);
        }
    }

    QtObject {
        id: internal;

        function onRemoved(objectId){
            if (root.documentManager){
                let index = root.documentManager.getDocumentIndexByDocumentId(objectId);
                if (index >= 0){
                    root.documentManager.closeDocument(objectId, true);
                }
            }
        }
    }

    function createNewObject(typeId, viewTypeId){
        if (root.documentManager){
            root.documentManager.insertNewDocument(typeId, viewTypeId);
        }
        else{
            console.error("Unable to create new object:", typeId, viewTypeId, ". Error: Document manager is invalid")
        }
    }

    function openDocumentEditor(objectId, typeId, viewTypeId){
        if (root.documentManager){
            root.documentManager.openDocument(objectId, typeId, viewTypeId);
        }
        else{
            console.error("Unable to open document for editing", typeId, viewTypeId, ". Error: Document manager is invalid");
        }
    }

    function onEdit(){
        let elementsModel = root.collectionView.table.elements;
        if (!elementsModel){
            console.error("Unable to edit document. Error: Elements for collection view is invalid");
            return;
        }

        let indexes = root.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let index = indexes[0];
            if (elementsModel.containsKey("Id", index)){
                let itemId = elementsModel.getData("Id", index);

                root.openDocumentEditor(itemId, documentTypeId, viewTypeId);
            }
        }
    }

    function onNew(){
        root.createNewObject(documentTypeId, viewTypeId);
    }

    function onImport(){
        console.log("DocumentCollectionViewDelegate onImport()")
        if (collectionView && collectionView.dataController){
            collectionView.dataController.importDocument();
        }
    }
}
