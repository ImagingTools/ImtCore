import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

BasicTreeView2 {
    id: treeView;

    clip: true;

    headerVisible: false;

    checkable: true;

    property BasicTreeView2 featuresTableView: null;
    property TreeItemModel documentModel: null;

    property var selectedFeatureData: [];

    property bool blockUpdatingModel: false;

    rowDelegate: TreeViewItemDelegateBase2 {
        width: treeView.width;

        root: treeView;

        onCheckedStateChanged: {
            console.log("onCheckedStateChanged");
            let itemData = treeView.getItemDataByUuid(uuid);
            if (itemData){
//                treeView.updateGui();

                let featureId = itemData.getData("Id");

                treeView.updateModel(featureId, state);
            }
        }
    }

    Connections{
        target: treeView.featuresTableView;

        onSelectionChanged: {
            treeView.onSelectedIndexChanged();
        }
    }

    function onSelectedIndexChanged(){
        console.log("treeView onSelectedIndexChanged", treeView.documentModel);

        let indexes = treeView.featuresTableView.getSelectedIndexes();
        if (indexes.length !== 1){
            treeView.visible = false;
        }
        else{
            treeView.visible = true;

            treeView.selectedFeatureData = treeView.featuresTableView.getItemDataByIndex(indexes[0]);

            let featureId = treeView.selectedFeatureData.getData("Id");
            if (featureId === ""){
                treeView.visible = false;
            }
            else{
                treeView.updateGui();
            }
        }

//        let selectedItems = featuresTableView.getSelectedItems();

//        if (selectedItems.length !== 1){
//            treeView.visible = false;
//        }
//        else{
//            treeView.visible = true;
//            treeView.selectedFeatureData = selectedItems[0];

//            let featureId = treeView.selectedFeatureData.getData("Id");
//            if (featureId === ""){
//                treeView.visible = false;
//            }
//            else{
//                treeView.updateGui();
//            }
//        }
    }

    function clear(){
        let items = treeView.getAllItemsAsList();

        for (let i = 0; i < items.length; i++){
            let item = items[i];

            item.resetData();
        }
    }

    function updateGui(){
        console.log("treeView updateGui");

        treeView.blockUpdatingModel = true;

        treeView.clear();

        let featureId = treeView.selectedFeatureData.getData("Id");

        if (!featureId || featureId === ""){
            return;
        }

        if (!treeView.documentModel.ContainsKey("DependenciesModel")){
            return;
        }

        console.log("featureId", featureId);

        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");
        if (dependenciesModel.ContainsKey(featureId)){
            let values = dependenciesModel.GetData(featureId);
            let dependencyFeatures = values.split(';');

            let inactiveElements = [];
            treeView.findParentFeatureDependencies(featureId, inactiveElements);

            let items = treeView.getAllItemsAsList();

            let retVal = [];
            for (let i = 0; i < items.length; i++){
                let item = items[i];

                if (item.level === 0){
                    item.isCheckable = false;
                }

                let itemId = item.getData("Id");

                if (itemId === featureId){
                    item.isVisible = false;
                }

                if (dependencyFeatures.includes(itemId)){
                    item.checkedState = Qt.Checked;

                    let dependencyId = dependencyFeatures.indexOf(itemId);

                    treeView.findChildrenFeatureDependencies(dependencyId, retVal);
                }

                if (retVal.includes(itemId)){
                    item.checkedState = Qt.Checked;
                    item.isActive = false;
                }

                if (inactiveElements.includes(itemId)){
                    item.checkedState = Qt.Unchecked;
                    item.isActive = false;
                }
            }
        }

        treeView.blockUpdatingModel = false;
    }

    function updateModel(featureId, checkState){
        if (treeView.blockUpdatingModel){
            return;
        }

        console.log("updateDependenciesModel");
        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = treeView.documentModel.AddTreeModel("DependenciesModel");
        }

        let selectedId = treeView.selectedFeatureData.getData("Id");
        let dependencyId = featureId;

        if (dependenciesModel.ContainsKey(selectedId)){
            let dependencies = dependenciesModel.GetData(selectedId);

            if (dependencies != ""){
                let dependenciesList = dependencies.split(';')

                let pos = dependenciesList.indexOf(dependencyId);
                if (checkState == Qt.Checked){
                    if (pos < 0){
                        dependenciesList.push(dependencyId)
                    }
                }
                else{
                    if (pos >= 0){
                        dependenciesList.splice(pos, 1)
                    }
                }

                let newDependencies = dependenciesList.join(';')

                if (newDependencies.length == 0){
                    dependenciesModel.RemoveData(selectedId)
                }
                else{
                    dependenciesModel.SetData(selectedId, newDependencies);
                }
            }
        }
        else{
            if (checkState == Qt.Checked){
                dependenciesModel.SetData(selectedId, dependencyId);
            }
        }
    }

    Component.onCompleted: {
        treeView.addColumn({"Id": "Name", "Name": "Name"});

//        if (featuresProvider.model){
//            let listModel = converter.convertToListModel(featuresProvider.model);

//            treeView.rowModel = listModel;
//        }
    }

    onRowModelDataChanged: {
        if (prop == "CheckState"){
            treeView.updateModel(delegate.itemData.Id, delegate.itemData.CheckState);

            treeView.updateGui();
        }
    }

    function findParentFeatureDependencies(featureId, retVal){
        console.log("findParentFeatureDependencies");
        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");
        if (!dependenciesModel){
            return;
        }

        let keys = dependenciesModel.GetKeys();

        for (let i = 0; i < keys.length; i++){
            let key = keys[i];

            let values = dependenciesModel.GetData(key);
            if (values != ""){
                let dependenciesList = values.split(';')
                if (dependenciesList.includes(featureId)){
                    retVal.push(key)
                    treeView.findParentFeatureDependencies(key, retVal)
                }
            }
        }
    }

    function findChildrenFeatureDependencies(featureId, retVal){
        console.log("findChildrenFeatureDependencies");
        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");
        if (!dependenciesModel){
            return;
        }

        let keys = dependenciesModel.GetKeys();

        if (keys.includes(featureId)){
            let values = dependenciesModel.GetData(featureId);

            if (values != ""){
                let dependenciesList = values.split(';')

                for (let i = 0; i < dependenciesList.length; i++){
                    let value = dependenciesList[i];

                    retVal.push(value)

                    treeView.findChildrenFeatureDependencies(value, retVal);
                }
            }
        }
    }
}
