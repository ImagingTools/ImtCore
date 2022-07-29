import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property Item treeViewItem: null;
    property Item tableItem: null;

    property int selectedIndex: tableItem ? tableItem.selectedIndex : -1;

    signal checkBoxStateChanged(int state, string parentId, string childId);

    onSelectedIndexChanged: {
    }

    onCheckBoxStateChanged: {
        console.log("ObjectView TreeView onItemTreeViewCheckBoxStateChanged", state, parentId, childId);

        let rootFeatureId = tableItem.getSelectedId();
        let rootkey = rootFeatureId;
        let value = childId;

        if (state == 0){
            treeViewModel.deselectFeature(value);
        }

        if (featureDependenciesModel.modelFeatureDependencies.ContainsKey(rootkey)){
            let str = featureDependenciesModel.modelFeatureDependencies.GetData(rootkey);
            let arr = str.split(";");
            if (state == 0){
                if (arr){
                    let index = arr.indexOf(value);
                    if (index > -1){
                        arr.splice(index, 1);
                    }

                    if (arr.length == 0){
                        featureDependenciesModel.modelFeatureDependencies.RemoveData(rootkey);
                    }
                    else{
                        let resStr = arr.join(';');
                        featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, resStr);
                    }
                }
            }
            else{
                if (arr.indexOf(value) == -1){
                    arr.push(value);
                    let resStr = arr.join(';');
                    featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, resStr);
                }
            }
        }
        else{
            featureDependenciesModel.modelFeatureDependencies.SetData(rootkey, value);
        }

        if (state == 2){
            treeViewModel.selectFeature(value);
        }
    }
}
