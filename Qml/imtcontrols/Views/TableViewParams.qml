import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TreeItemModel {
    id: root;

    function getHeaderIds(){
        let headerIds = []
        for (let i = 0; i < root.getItemsCount(); i++){
            if (root.containsKey("HeaderId", i)){
                let headerId = root.getData("HeaderId", i)
                headerIds.push(headerId)
            }
        }

        return headerIds;
    }

    function getHeaderIndex(headerId){
        for (let i = 0; i < root.getItemsCount(); i++){
            if (root.containsKey("HeaderId", i)){
                let id = root.getData("HeaderId", i)
                if (id === headerId){
                    return i;
                }
            }
        }

        return -1;
    }

    function setHeaderVisible(headerId, visible){
        internal.setHeaderInfo(headerId, "Visible", visible);
    }

    function setHeaderSize(headerId, size){
        internal.setHeaderInfo(headerId, "Size", size);
    }

    function getHeaderSize(headerId){
        let index = root.getHeaderIndex(headerId);
        if (index >= 0){
            return root.getData("Size", index);
        }

        return -1;
    }

    function headerIsVisible(headerId){
        let index = root.getHeaderIndex(headerId);

        if (index >= 0){
            let isVisible = root.getData("Visible", index)

            return isVisible;
        }

        return false;
    }

    function headerIsExists(headerId){
        return getHeaderIds().includes(headerId)
    }

    function copyFromTreeModel(model){
        console.log("copyFromTreeModel", model.toJson());
        if (!model){
            return false;
        }
        clear();
        console.log("copy", model.toJson());

        copy(model);
    }

    function copyFrom(obj){
        if (!obj){
            return false;
        }

        clear();
        let headerIds = obj.getHeaderIds();

        for (let i = 0; i < headerIds.length; i++){
            setHeaderVisible(headerIds[i], obj.headerIsVisible(headerIds[i]));
            setHeaderSize(headerIds[i], obj.getHeaderSize(headerIds[i]));
        }

        return true;
    }

    property QtObject internal: QtObject{
        id: internal;

        function setHeaderInfo(headerId, infoKey, infoValue){
            let index = root.getHeaderIndex(headerId);
            if (index < 0){
                index = root.insertNewItem();
                root.setData("HeaderId", headerId, index);
            }

            root.setData(infoKey, infoValue, index);
        }
    }
}

