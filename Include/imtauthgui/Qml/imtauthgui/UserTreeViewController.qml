import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property int selectedIndex: collectionView.table.selectedIndex;

    signal checkBoxChanged(int state, string parentId, string childId);

    Component.onCompleted: {
        commandsDelegate.edited.connect(edited);
    }

    Component.onDestruction: {
        commandsDelegate.edited.disconnect(edited);
    }

    function synchronise(){
        let featuresModel = documentModel.GetData("Features");

        let keys = featuresModel.GetKeys();

        for (let key of keys){
            let value = featuresModel.GetData(key);
            lisensesFeaturesModel.modelLicenseFeatures.SetData(key, value);
        }
    }

    function edited(itemId, itemName){
        let elementsModel = collectionView.table.elements;
        let oldLicenseId = itemId;
        let newLicenseId = elementsModel.GetData("Id", selectedIndex);

        if (oldLicenseId != newLicenseId){
            lisensesFeaturesModel.updateLicensesDependenciesAfterLicenseEditing(oldLicenseId, newLicenseId);
        }
    }

    onSelectedIndexChanged: {

        if (selectedIndex > -1){
            let rootKey = collectionView.table.getSelectedId();

            treeView.visible = rootKey != null && rootKey != "";

            treeViewModel.resetProperties();

            let featuresModel = documentModel.GetData("Features");

            if (featuresModel){
                let strValues = featuresModel.GetData(rootKey);
                if (strValues){
                    let values = strValues.split(';');
                    for (let value of values){
                        treeViewModel.selectFeature(value);
                    }
                }

                let upFeatures = [];
                featureDependenciesModel.getFeaturesDependsByFeatureUp(rootKey, upFeatures);

                if (upFeatures.length > 0){
                    treeViewModel.updateDataFeatureList(upFeatures, 0, 0);
                }
            }
        }
    }

    onCheckBoxChanged: {

        let rootLicenseId = collectionView.table.getSelectedId();
        let rootKey = rootLicenseId;
        let value = childId;

        if (state == 0){
            treeViewModel.deselectFeature(value);
        }

        let featuresModel = documentModel.GetData("Features");

        if (!featuresModel){
            featuresModel = documentModel.AddTreeModel("Features");
        }

        if (featuresModel.ContainsKey(rootKey)){
            let str = lisensesFeaturesModel.modelLicenseFeatures.GetData(rootKey);
            let arr = str.split(";");
            if (state == 0){
                if (arr){
                    let index = arr.indexOf(value);
                    if (index > -1){
                        arr.splice(index, 1);
                    }

                    if (arr.length == 0){
                        featuresModel.RemoveData(rootKey);
                    }
                    else{
                        let resStr = arr.join(';');
                        featuresModel.SetData(rootKey, resStr);
                    }
                }
            }
            else{
                if (arr.indexOf(value) == -1){
                    arr.push(value);
                    let resStr = arr.join(';');
                    featuresModel.SetData(rootKey, resStr);
                }
            }
        }
        else{
            featuresModel.SetData(rootKey, value);
        }

        if (state == 2){
            treeViewModel.selectFeature(value);
        }

        synchronise()
    }
}
