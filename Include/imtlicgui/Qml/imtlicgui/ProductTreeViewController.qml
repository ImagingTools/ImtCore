import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

TreeViewControllerBase {
    id: container;

    property int selectedIndex: collectionView.table.selectedIndex;

    signal checkBoxChanged(int state, string childId);

    onSelectedIndexChanged: {
        console.log("ProductTreeView onSelectedIndexChanged", selectedIndex);
        if (selectedIndex > -1){
            let rootKey = collectionView.table.getSelectedId();

            treeView.visible = rootKey != null && rootKey != "";
            let featuresModel = documentModel.GetData("Features");

            treeView.itemStateChanged.disconnect(itemStateChanged);
            resetProperties();
            treeView.itemStateChanged.connect(itemStateChanged);

            if (featuresModel){
                if (featuresModel.ContainsKey(rootKey)){
                    let licenseFeaturesModel = featuresModel.GetData(rootKey);

                    let parentFeatures = []
                    getParentFeatures(licenseFeaturesModel, parentFeatures);

                    let selectedFeatures = []

                    for (let parentId of parentFeatures){
                        getFeaturesLeaves(licenseFeaturesModel, parentId, selectedFeatures);
                    }

                    console.log('selectedFeatures', selectedFeatures);

                    for (let featureId of selectedFeatures){
                        selectFeature(featureId);

                        let childrenDepends = []
                        featureDependenciesModel.getAllChildrenDependsFeatures(featureId, childrenDepends);
                        setValueToProperty("Active", false, childrenDepends);
                    }
                }
            }
        }
    }

    function itemStateChanged(itemData){
        console.log("ProductTreeView itemStateChanged", itemData);
        let featuresModel = documentModel.GetData("Features");

        if (!featuresModel){
            featuresModel = documentModel.AddTreeModel("Features");
        }

        let itemId = itemData.Id;
        let itemName = itemData.Name;
        let itemState = itemData.State;
        let itemOptional = itemData.Optional;

        let itemParentId = null;

        if (itemData.Parent && itemData.Parent.Level > 0){
            itemParentId = itemData.Parent.Id;
        }

        let licenseId = collectionView.table.getSelectedId();

        let childrenDepends = []
        featureDependenciesModel.getAllChildrenDependsFeatures(itemId, childrenDepends);

        if (itemState == Qt.Checked || itemState == Qt.PartiallyChecked){

            let licenseFeaturesModel;
            if (featuresModel.ContainsKey(licenseId)){
                licenseFeaturesModel = featuresModel.GetData(licenseId);
            }
            else{
                licenseFeaturesModel = featuresModel.AddTreeModel(licenseId);
            }

            let itemIndex = -1;
            for (let i = 0; i < licenseFeaturesModel.GetItemsCount(); i++){
                let curItemId = licenseFeaturesModel.GetData("Id", i);

                if (curItemId == itemId){
                    itemIndex = i;
                    break;
                }
            }

            if (itemIndex < 0){
                itemIndex = licenseFeaturesModel.InsertNewItem();
            }

            if (licenseFeaturesModel.ContainsKey("State", itemIndex)){
                let curState = licenseFeaturesModel.GetData("State", itemIndex);
                if (curState == itemState){
                    return;
                }
            }

            licenseFeaturesModel.SetData("Id", itemId, itemIndex);
            licenseFeaturesModel.SetData("Name", itemName, itemIndex);
            licenseFeaturesModel.SetData("State", itemState, itemIndex);
            licenseFeaturesModel.SetData("Optional", itemOptional, itemIndex);
            licenseFeaturesModel.SetData("ParentId", itemParentId, itemIndex);

            setValueToProperty("Active", false, childrenDepends);
        }
        else if (itemState == Qt.Unchecked){
            if (featuresModel.ContainsKey(licenseId)){
                let licenseFeaturesModel = featuresModel.GetData(licenseId);

                let itemIndex = -1;
                for (let i = 0; i < licenseFeaturesModel.GetItemsCount(); i++){
                    let curItemId = licenseFeaturesModel.GetData("Id", i);

                    if (curItemId == itemId){
                        itemIndex = i;
                        break;
                    }
                }

                if (itemIndex >= 0){
                    licenseFeaturesModel.RemoveItem(itemIndex);
                }

                if (licenseFeaturesModel.GetItemsCount() == 0){
                    featuresModel.RemoveData(licenseId);
                }

                setValueToProperty("Active", true, childrenDepends);
            }
        }
        console.log("featuresModel", featuresModel.toJSON());
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

//     function getParentFeatures(model, retVal){
//         for (let i = 0; i < model.GetItemsCount(); i++){
//             let currentFeatureId = model.GetData("Id", i);
//             let currentParentFeatureId = model.GetData("ParentId", i);

//             if (!currentParentFeatureId){
//                 retVal.push(currentFeatureId);
//             }
//         }
//     }

//     function getFeaturesLeaves(model, parentId, retVal){
//         let result = []
//         for (let i = 0; i < model.GetItemsCount(); i++){
//             let currentFeatureId = model.GetData("Id", i);
//             let currentParentFeatureId = model.GetData("ParentId", i);

//             if (currentParentFeatureId == parentId){
//                 result.push(currentFeatureId)
//                 getFeaturesLeaves(model, currentFeatureId, retVal);
//             }
//         }

//         if (result.length == 0){
//             retVal.push(parentId)
//         }
//     }
}
