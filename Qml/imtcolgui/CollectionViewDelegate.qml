import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
    id: root;

    property CollectionView collectionView: null;

    function removeObject(objectId){
        if (root.collectionView){
            root.collectionView.removeElement(objectId)
        }
    }

    function renameObject(objectId, newName){
        if (root.collectionView){
            root.collectionView.setElementName(objectId, newName);
        }
    }

    function setDescriptionObject(objectId, description){
        if (root.collectionView){
            root.collectionView.setElementDescription(objectId, description);
        }
    }

    function updateObject(objectId){}
    function openDocumentEditor(objectId, viewTypeId){}
    function setupContextMenu(menu){}
    function createNewObject(typeId, objectName, description){}
    function updateItemSelection(selectedItems, selectedTypeId){}
}

