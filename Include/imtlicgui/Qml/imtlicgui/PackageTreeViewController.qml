import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property int selectedIndex: collectionView.table.selectedIndex;

    signal checkBoxChanged(int state, string parentId, string childId);

    onSelectedIndexChanged: {
        console.log("PackageTreeView onSelectedIndexChanged", container.selectedIndex);

        let rootkey = collectionView.table.getSelectedId();

        treeView.visible = itemId != "" && rootkey != null && rootkey != "";

        treeViewModel.resetProperties();
        container.updateVisibleElements();

        if (featureDependenciesModel.modelFeatureDependencies){
            let strValues = featureDependenciesModel.modelFeatureDependencies.GetData(rootkey);
            if (strValues){
                let values = strValues.split(';');
                for (let value of values){
                    treeViewModel.selectFeature(value);
                }
            }

            let upFeatures = [];
            featureDependenciesModel.getFeaturesDependsByFeatureUp(rootkey, upFeatures);

            if (upFeatures.length > 0){
                treeViewModel.updateDataFeatureList(upFeatures, 0, 0);
            }
        }
    }

    onCheckBoxChanged: {
        console.log("ObjectView TreeView onItemTreeViewCheckBoxStateChanged", state, parentId, childId);
        console.log("ItemID", itemId);
        if (!visible){
            return;
        }

        let rootkey = collectionView.table.getSelectedId();
        let value = childId;

        /**
            Запрещаем зависимость Фичи от самой себя
        */
        if (rootkey === childId){
            return;
        }

        if (state === 0){
            treeViewModel.deselectFeature(value);
        }

        if (featureDependenciesModel.modelFeatureDependencies.ContainsKey(rootkey)){
            let str = featureDependenciesModel.modelFeatureDependencies.GetData(rootkey);
            let arr = str.split(";");
            if (state === 0){
                if (arr){
                    let index = arr.indexOf(value);
                    if (index > -1){
                        arr.splice(index, 1);
                    }

                    if (arr.length === 0){
                        featureDependenciesModel.modelFeatureDependencies.RemoveData(rootkey);
                    }
                    else{
                        let resStr = arr.join(';');
                        featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, resStr);
                    }
                }
            }
            else{
                if (arr.indexOf(value) === -1){
                    arr.push(value);
                    let resStr = arr.join(';');
                    featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, resStr);
                }
            }
        }
        else{
            featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, value);
        }

        if (state === 2){
            treeViewModel.selectFeature(value);
        }
    }

    function updateVisibleElements(){
        console.log("PackageTreeView updateVisibleElements");
        let packageId = packageViewContainer.itemId;
        let featureId = collectionView.table.getSelectedId();

        for (let i = 0; i < treeViewModel.modelTreeView.GetItemsCount(); i++){
            let m_packageId = treeViewModel.modelTreeView.GetData("Id", i);
            if (m_packageId === packageId){
                let m_features = treeViewModel.modelTreeView.GetData("childItemModel", i);

                for (let j = 0; j < m_features.GetItemsCount(); j++){
                    let m_visible = m_features.GetData("visible", j)
                    if (!m_visible){
                        console.log("visible -> 1 index", j);
                        m_features.SetData("visible", 1, j);
                    }

                    let m_featureId = m_features.GetData("Id", j);
                    if (m_featureId === featureId){
                        console.log("visible -> 0 index", j);
                        m_features.SetData("visible", 0, j);
                    }
                }
            }
        }
    }
}
