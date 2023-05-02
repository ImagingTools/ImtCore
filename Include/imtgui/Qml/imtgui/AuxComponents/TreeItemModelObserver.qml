import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property TreeItemModel beginModel: TreeItemModel {}; // Model before changes
    property TreeItemModel observedModel: null;

    signal modelChanged(var changeList);

    Component.onDestruction: {
        if (container.observedModel != null){
            container.observedModel.modelChanged.disconnect(container.observedModelDataChanged);
        }
    }

    function registerModel(model){
        console.log("PreferenceObserver registerModel", model);

        container.observedModel = model;
        container.observedModel.modelChanged.connect(container.observedModelDataChanged);
        container.beginModel.Copy(container.observedModel);
    }

    function observedModelDataChanged(){
        console.log("observedModelDataChanged");
        let changeList = compare(container.beginModel, container.observedModel);
        container.modelChanged(changeList);
        container.beginModel.Copy(container.observedModel);
    }

    function compare(model1, model2){
        let changeList = []
        compareRecursive(model1, model2, changeList, "");
        return changeList;
    }

    function compareRecursive(model1, model2, changeList, parentKey){
        if (!model1 || !model2){
            return;
        }

        for (let i = 0; i < model1.GetItemsCount(); i++){
            let dataId = model1.GetData("Id", i);

            let keys = model1.GetKeys(i);
            for (let j = 0; j < keys.length; j++){
                let key = keys[j];

                let globalId = parentKey + '/' + dataId;
                if (model1.ContainsKey(key, i) && model2.ContainsKey(key, i)){
                    let model1Value = model1.GetData(key, i);
                    let model2Value = model2.GetData(key, i);

                    if(typeof model1Value === 'object' && typeof model2Value === 'object'){
                        compareRecursive(model1Value, model2Value, changeList, globalId);
                    }
                    else{
                        if (model1Value != model2Value){
                            let changeObj = {}

                            changeObj["operation"] = "change";
                            changeObj["key"] = key;
                            changeObj["curVal"] = model1Value;
                            changeObj["newVal"] = model2Value;
                            changeObj["index"] = i;
                            changeObj["id"] = globalId;

                            changeList.push(changeObj)
                        }
                    }
                }
                else if (model1.ContainsKey(key, i) && !model2.ContainsKey(key, i)){
                    let model1Value = model1.GetData(key, i);
                    let changeObj = {}

                    changeObj["operation"] = "remove";
                    changeObj["key"] = key;
                    changeObj["curVal"] = model1Value;
                    changeObj["index"] = i;
                    changeObj["id"] = globalId;

                    changeList.push(changeObj)
                }
                else if (!model1.ContainsKey(key, i) && model2.ContainsKey(key, i)){
                    let model2Value = model2.GetData(key, i);
                    let changeObj = {}

                    changeObj["operation"] = "insert";
                    changeObj["key"] = key;
                    changeObj["curVal"] = model2Value;
                    changeObj["index"] = i;
                    changeObj["id"] = globalId;

                    changeList.push(changeObj)
                }
            }
        }
    }

    function structureIsEqual(model1, model2){
        let model1ItemsCount = model1.GetItemsCount();
        let model2ItemsCount = model2.GetItemsCount();

        if (model1ItemsCount != model2ItemsCount){
            return false;
        }

        for (let i = 0; i < model1.GetItemsCount(); i++){
            let model1keys = model1.GetKeys(i);
            let model2keys = model2.GetKeys(i);

            if (model1keys.length != model2keys.length){
                return false;
            }

            for (let j = 0; j < keys.length; j++){
                let key = keys[j];

                if (!model2.ContainsKey(key)){
                    return false;
                }

                let model1Value = model1.GetData(key, i);
                let model2Value = model2.GetData(key, i);

                if(typeof model1Value === 'object' && typeof model2Value === 'object'){
                    let result = structureIsEqual(model1Value, model2Value);
                    if (!result){
                        return false;
                    }
                }
            }
        }

        return true;
    }
}
