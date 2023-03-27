import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    property TreeItemModel treeItemModel: null;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component {
        id: treeItemModelComp;
        TreeItemModel {}
    }

    function toAdaptedModel(treeItemModel){
        let adaptedModel = treeItemModelComp.createObject(null);

        root.toAdaptedModelRecursive(adaptedModel, treeItemModel, "", 0);

        return adaptedModel;
    }

    function fromAdaptedModel(adaptedModel){
        let treeItemModel = treeItemModelComp.createObject(null);

        root.fromAdaptedModelRecursive(adaptedModel, treeItemModel);

        return treeItemModel;
    }

    function toAdaptedModelRecursive(adaptedModel, treeItemModel, parentUuid, depth){
        for (let i = 0; i < treeItemModel.GetItemsCount(); i++){
            let index = adaptedModel.InsertNewItem();
            adaptedModel.CopyItemDataFromModel(index, treeItemModel, i);
            adaptedModel.RemoveData("ChildModel", index);

            let uuid = uuidGenerator.generateUUID();
            adaptedModel.SetData("Uuid", uuid, index);
            adaptedModel.SetData("Parent", parentUuid, index);
            adaptedModel.SetData("Depth", depth, index);
            adaptedModel.SetData("HasChildren", false, index);
            adaptedModel.SetData("Visible", true, index);
            adaptedModel.SetData("Active", true, index);
          //  adaptedModel.SetData("CheckedState", Qt.Unchecked, index);

            if (treeItemModel.ContainsKey("ChildModel", i)){
                let childModel = treeItemModel.GetData("ChildModel", i)
                adaptedModel.SetData("HasChildren", true, index);

                root.toAdaptedModelRecursive(adaptedModel, childModel, uuid, depth + 1);
            }
        }
    }

    function fromAdaptedModelRecursive(adaptedModel, treeItemModel){
        for (let i = 0; i < adaptedModel.GetItemsCount(); i++){
            let parent = adaptedModel.GetData("Parent", i);
            if (parent !== ""){

            }
        }
    }
}
