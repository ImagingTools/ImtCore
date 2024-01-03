import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: converter;

    Component {
        id: listModelComp;
        ListModel {}
    }

    Component {
        id: treeItemModelComp;
        TreeItemModel {}
    }

    function convertToListModel(treeItemModel){
        console.log("convertToListModel");

        let obj = listModelComp.createObject(converter);

        privateBlock.convertToListModelRecursive(obj, treeItemModel);

        return obj;
    }

    function convertFromListModel(listModel){
        console.log("convertFromListModel");

        let obj = treeItemModelComp.createObject(converter);

        privateBlock.convertFromListModelRecursive(listModel, obj);

        return obj;
    }

    Item {
        id: privateBlock;

        function convertFromListModelRecursive(listModel, treeItemModel){
            if (!listModel || !treeItemModel){
                return;
            }

            for (let i = 0; i < listModel.count; i++){
                let index = treeItemModel.InsertNewItem();

                let objItem = listModel.get(i);

                let itemKeys = Object.keys(objItem);

                for (let j = 0; j < itemKeys.length; j++){
                    let key = itemKeys[j];
                    let value = objItem[key];

                    if (typeof value == 'object'){
                        let obj = treeItemModel.AddTreeModel(key, index);

                        convertFromListModelRecursive(value, obj);
                    }
                    else{
                        treeItemModel.SetData(key, value, index);
                    }
                }
            }
        }

        function convertToListModelRecursive(listModel, treeItemModel){
            if (!listModel || !treeItemModel){
                return;
            }

            for (let i = 0; i < treeItemModel.GetItemsCount(); i++){
                let itemKeys = treeItemModel.GetKeys(i);

                let objectItem = {}
                for (let j = 0; j < itemKeys.length; j++){
                    let key = itemKeys[j];

                    let value = treeItemModel.GetData(key, i);
                    if (typeof value == 'object'){
                        let obj = listModelComp.createObject(listModel);

                        objectItem[key] = obj;

                        convertToListModelRecursive(obj, value);
                    }
                    else{
                        objectItem[key] = value;
                    }
                }

                let hasKey = "CheckState" in objectItem;
                if (!hasKey){
                    objectItem["CheckState"] = Qt.Unchecked;
                }

                hasKey = "Visible" in objectItem;
                if (!hasKey){
                    objectItem["Visible"] = true;
                }

                hasKey = "Selected" in objectItem;
                if (!hasKey){
                    objectItem["Selected"] = false;
                }

                hasKey = "Active" in objectItem;
                if (!hasKey){
                    objectItem["Active"] = true;
                }

                hasKey = "CheckBoxVisible" in objectItem;
                if (!hasKey){
                    objectItem["CheckBoxVisible"] = true;
                }

                hasKey = "IsOpen" in objectItem;
                if (!hasKey){
                    objectItem["IsOpen"] = true;
                }

                listModel.insert(i, objectItem)
           }
        }
    }
}
