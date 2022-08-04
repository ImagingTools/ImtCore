import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property int selectedIndex: collectionView.table.selectedIndex;

    signal checkBoxChanged(int state, string parentId, string childId);

    onSelectedIndexChanged: {

        if (selectedIndex > -1){
            let rootKey = collectionView.table.getSelectedId();

            treeView.visible = rootKey != null && rootKey != "";

            treeViewModel.resetProperties();

            if (featureDependenciesModel.modelFeatureDependencies){
                let strValues = lisensesFeaturesModel.modelLicenseFeatures.GetData(rootKey);
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

        console.log("featureDependenciesModel", featureDependenciesModel);
        console.log("lisensesFeaturesModel", lisensesFeaturesModel);


        let rootLicenseId = collectionView.table.getSelectedId();
        let rootKey = rootLicenseId;
        let value = childId;

        if (state == 0){
            treeViewModel.deselectFeature(value);
        }

        if (lisensesFeaturesModel.modelLicenseFeatures.ContainsKey(rootKey)){
            let str = lisensesFeaturesModel.modelLicenseFeatures.GetData(rootKey);
            let arr = str.split(";");
            if (state == 0){
                if (arr){
                    let index = arr.indexOf(value);
                    if (index > -1){
                        arr.splice(index, 1);
                    }

                    if (arr.length == 0){
                        lisensesFeaturesModel.modelLicenseFeatures.RemoveData(rootKey);
                    }
                    else{
                        let resStr = arr.join(';');
                        lisensesFeaturesModel.modelLicenseFeatures.SetData(rootKey, resStr);
                    }
                }
            }
            else{
                if (arr.indexOf(value) == -1){
                    arr.push(value);
                    let resStr = arr.join(';');
                    lisensesFeaturesModel.modelLicenseFeatures.SetData(rootKey, resStr);
                }
            }
        }
        else{
            lisensesFeaturesModel.modelLicenseFeatures.SetData(rootKey, value);
        }

        if (state == 2){
            treeViewModel.selectFeature(value);
        }
    }
}
