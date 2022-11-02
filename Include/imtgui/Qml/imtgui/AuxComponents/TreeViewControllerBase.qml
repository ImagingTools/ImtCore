import QtQuick 2.15
import Acf 1.0

Item {
    id: root;

    Component.onCompleted: {
        treeView.itemStateChanged.connect(itemStateChanged);
    }

    Component.onDestruction: {
        treeView.itemStateChanged.disconnect(itemStateChanged);
    }

    function getParentFeatures(model, retVal){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let currentFeatureId = model.GetData("Id", i);
            let currentParentFeatureId = model.GetData("ParentId", i);

            if (!currentParentFeatureId){
                retVal.push(currentFeatureId);
            }
        }
    }

    function getFeaturesLeaves(model, parentId, retVal){
        let result = []
        for (let i = 0; i < model.GetItemsCount(); i++){
            let currentFeatureId = model.GetData("Id", i);
            let currentParentFeatureId = model.GetData("ParentId", i);

            if (currentParentFeatureId == parentId){
                result.push(currentFeatureId)
                getFeaturesLeaves(model, currentFeatureId, retVal);
            }
        }

        if (result.length == 0){
            retVal.push(parentId)
        }
    }


    function setValueToProperty(prop, value, features){
        if (features.length == 0){
            return;
        }

        for (let i = 0; i < treeView.model.GetItemsCount(); i++){
            let packageId = treeView.model.GetData("Id", i);
            let childItems = treeView.model.GetData("ChildModel", i);
            if (childItems){
                _setValueToPropertyRecursive(childItems, prop, value, features);
            }
        }
    }

    function _setValueToPropertyRecursive(model, prop, value, features){
        console.log("_makeFeaturesInactiveRecursive", model, features);

        for (let i = 0; i < model.GetItemsCount(); i++){
            let featureId = model.GetData("Id", i);

            if (features.includes(featureId)){

                if (model.ContainsKey(prop)){
                    console.log("model SetDAta", prop, value);
                    model.SetData(prop, value, i);
                }
            }

            let childModel = model.GetData("ChildModel", i);

            if (childModel){
                _setValueToPropertyRecursive(childModel, prop, value, features);
            }
        }
    }

    function resetProperties(){
        undoRedoManager.model.modelChanged.disconnect(undoRedoManager.modelUpdated);
        for (let i = 0; i < treeView.model.GetItemsCount(); i++){
            let childrenItems = treeView.model.GetData("ChildModel", i);
            if (childrenItems){
                _resetRecursive(childrenItems);
            }
        }
        undoRedoManager.model.modelChanged.connect(undoRedoManager.modelUpdated);
    }

    function _resetRecursive(model){
         for (let i = 0; i < model.GetItemsCount(); i++){
             let state = model.GetData("State", i);
             let isActive = model.GetData("Active", i);
             let isVisible = model.GetData("Visible", i);

             if (state != Qt.Unchecked){
                 model.SetData("State", Qt.Unchecked, i);
             }

             if (!isActive){
                 model.SetData("Active", true, i);
             }

             if (!isVisible){
                 model.SetData("Visible", true, i);
             }

             let childModel = model.GetData("ChildModel", i);
             if (childModel){
                 _resetRecursive(childModel);
             }
         }
    }

    function selectFeature(feature){
        _recursiveSelection(treeView.model, feature);
    }

    function _recursiveSelection(model, featureId){
         for (let i = 0; i < model.GetItemsCount(); i++){
             let itemId = model.GetData("Id", i);

             if (itemId == featureId){
                 model.SetData("State", Qt.Checked, i);
                 model.SetData("Active", true, i);

                 return;
             }

             let childModel = model.GetData("ChildModel", i);

             if (childModel){
                 _recursiveSelection(childModel, featureId);
             }
         }
    }
}
