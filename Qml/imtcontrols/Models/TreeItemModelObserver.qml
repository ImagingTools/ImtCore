import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
    id: container;

    property TreeItemModel observedModel: null;

    property QtObject private_: QtObject{
            property TreeItemModel beginModel: TreeItemModel {}; // Model before changes
            property bool block: false;
    }

    signal modelChanged(var changeList);

    function registerModel(model){
        container.observedModel = model;

        container.private_.beginModel = model.copyMe();
    }

    function observedModelDataChanged(){
        container.private_.block = true;

        let changeList = compare(container.private_.beginModel, container.observedModel);
        container.modelChanged(changeList);

        container.private_.beginModel = container.observedModel.copyMe();

        container.private_.block = false;
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

        let countItems1 = model1.getItemsCount();
        let countItems2 = model2.getItemsCount();

        if (countItems1 !== countItems2){
            let changeObj = {}

            changeObj["operation"] = "";
            changeObj["key"] = "";
            changeObj["curVal"] = "";
            changeObj["newVal"] = "";
            changeObj["index"] = -1;
            changeObj["id"] = "";

            changeList.push(changeObj)

            return;
        }

        for (let i = 0; i < model1.getItemsCount(); i++){
            let dataId = model1.getData("id", i);

            let keys = model1.getKeys(i);
            for (let j = 0; j < keys.length; j++){
                let key = keys[j];
                let globalId = parentKey + '/' + dataId;
                if (model1.containsKey(key, i) && model2.containsKey(key, i)){

                    let model1Value = model1.getTreeItemModel(key, i);
                    let model2Value = model2.getTreeItemModel(key, i);

                    if (model1Value != null && model2Value != null){
                        container.compareRecursive(model1Value, model2Value, changeList, globalId);
                    }
                    else{
                        let value1 = String(model1.getData(key, i));
                        let value2 = String(model2.getData(key, i));

                        if (value1 !== value2){
                            let changeObj = {}

                            changeObj["operation"] = "change";
                            changeObj["key"] = key;
                            changeObj["curVal"] = value1;
                            changeObj["newVal"] = value2;
                            changeObj["index"] = i;
                            changeObj["id"] = globalId;

                            changeList.push(changeObj)
                        }
                    }
                }
                else if (model1.containsKey(key, i) && !model2.containsKey(key, i)){
                    let model1Value = model1.getData(key, i);
                    let changeObj = {}

                    changeObj["operation"] = "remove";
                    changeObj["key"] = key;
                    changeObj["curVal"] = model1Value;
                    changeObj["index"] = i;
                    changeObj["id"] = globalId;

                    changeList.push(changeObj)
                }
                else if (!model1.containsKey(key, i) && model2.containsKey(key, i)){
                    let model2Value = model2.getData(key, i);
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
}
