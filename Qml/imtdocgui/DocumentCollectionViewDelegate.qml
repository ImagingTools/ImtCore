import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionViewCommandsDelegateBase {
    id: root;

    property DocumentManager documentManager: null;

    property string documentTypeId;
    property string viewTypeId;

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
            console.log("documentManager undefined!")
        }
    }

    function openDocumentEditor(objectId, typeId, viewTypeId){
        console.log("openDocumentEditor", objectId, typeId, viewTypeId);

        if (root.documentManager){
            root.documentManager.openDocument(objectId, typeId, viewTypeId);
        }
    }

    function onEdit(){
        console.log("onEdit")
        let elementsModel = root.collectionView.table.elements;
        if (!elementsModel){
            return;
        }

        let indexes = root.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let index = indexes[0];
            if (elementsModel.ContainsKey("Id", index)){
                let itemId = elementsModel.GetData("Id", index);

                root.openDocumentEditor(itemId, documentTypeId, viewTypeId);
            }
        }
    }

    function onNew(){
        console.log("onNew", documentTypeId, viewTypeId)
        root.createNewObject(documentTypeId, viewTypeId);
    }
}
