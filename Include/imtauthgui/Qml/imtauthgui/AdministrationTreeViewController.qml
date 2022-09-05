import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property int selectedIndex: collectionView.table.selectedIndex;

    signal checkBoxChanged(int state, string parentId, string childId);

    Component.onCompleted: {
        commandsDelegate.removed.connect(removed);
        commandsDelegate.edited.connect(edited);
    }

    Component.onDestruction: {
        commandsDelegate.removed.disconnect(removed);
        commandsDelegate.edited.disconnect(edited);
    }

    function synchronise(){
        let dependenciesModel = documentModel.GetData("Dependencies");

        let keys = dependenciesModel.GetKeys();

        for (let key of keys){
            let value = dependenciesModel.GetData(key);
            featureDependenciesModel.modelFeatureDependencies.SetData(key, value);
        }
    }

    function removed(itemId){
        console.log("AdministrationTreeView removed");

        let dependenciesModel = documentModel.GetData("Dependencies");

        featureDependenciesModel.removeDependsFeature(dependenciesModel, itemId);

        treeViewModel.removeFeatureInTreeViewModel(administrationViewContainer.itemId, itemId);
    }

    function edited(itemId, itemName){
        console.log("AdministrationTreeView edited");

        let oldId = itemId;
        let elementsModel = collectionView.table.elements;

        let newId = elementsModel.GetData("Id", selectedIndex);
        let newName = elementsModel.GetData("Name", selectedIndex);

        if (oldId == ""){
            treeViewModel.addFeatureInTreeViewModel(administrationViewContainer.itemId, newId, newName);
        }
        else{
            treeViewModel.updateTreeViewAfterFeatureEditing(administrationViewContainer.itemId, oldId, newId, newName);
        }
    }

    onSelectedIndexChanged: {
        console.log("AdministrationTreeView onSelectedIndexChanged", container.selectedIndex);

        let rootkey = collectionView.table.getSelectedId();

        treeView.visible = itemId != "" && rootkey != null && rootkey != "";

        treeViewModel.resetProperties();
        container.updateVisibleElements();

        let dependenciesModel = documentModel.GetData("Dependencies");
        if (dependenciesModel){
            let strValues = dependenciesModel.GetData(rootkey);
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

        let dependenciesModel = documentModel.GetData("Dependencies");

        if (!dependenciesModel){
            dependenciesModel = documentModel.AddTreeModel("Dependencies");
        }

        if (dependenciesModel.ContainsKey(rootkey)){
            let str = dependenciesModel.GetData(rootkey);
            let arr = str.split(";");
            if (state === 0){
                if (arr){
                    let index = arr.indexOf(value);
                    if (index > -1){
                        arr.splice(index, 1);
                    }

                    if (arr.length === 0){
                        dependenciesModel.RemoveData(rootkey);
                    }
                    else{
                        let resStr = arr.join(';');
                        dependenciesModel.SetData(rootkey, resStr);
                    }
                }
            }
            else{
                if (arr.indexOf(value) === -1){
                    arr.push(value);
                    let resStr = arr.join(';');
                    dependenciesModel.SetData(rootkey, resStr);
                }
            }
        }
        else{
            dependenciesModel.SetData(rootkey, value);
        }

        if (state === 2){
            treeViewModel.selectFeature(value);
        }

        synchronise();
    }

    function updateVisibleElements(){
        console.log("AdministrationTreeView updateVisibleElements");
        let packageId = administrationViewContainer.itemId;
        let featureId = collectionView.table.getSelectedId();

        for (let i = 0; i < treeViewModel.modelTreeView.GetItemsCount(); i++){
            let m_packageId = treeViewModel.modelTreeView.GetData("Id", i);
            if (m_packageId === packageId){
                let m_features = treeViewModel.modelTreeView.GetData("childItemModel", i);

                for (let j = 0; j < m_features.GetItemsCount(); j++){
                    let m_visible = m_features.GetData("visible", j)
                    if (!m_visible){
                        m_features.SetData("visible", 1, j);
                    }

                    let m_featureId = m_features.GetData("Id", j);
                    if (m_featureId === featureId){
                        m_features.SetData("visible", 0, j);
                    }
                }
            }
        }
    }
}
