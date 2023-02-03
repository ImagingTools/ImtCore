import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

BasicTreeView {
    id: treeView;

    clip: true;

    headerVisible: false;

    tristate: true;

    property BasicTreeView featuresTableView: null;
    property TreeItemModel documentModel: null;

    UuidGenerator {
        id: generator;
    }

    Component.onCompleted: {
        let uuid = generator.generateUUID();

        treeView.commandId = uuid;
    }

    Component.onDestruction: {
        treeView.featuresTableView.selectedIndexChanged.disconnect(treeView.onSelectedIndexChanged);
    }

    onFeaturesTableViewChanged: {
        if (treeView.featuresTableView != null){
            treeView.featuresTableView.selectedIndexChanged.connect(treeView.onSelectedIndexChanged);
        }
    }

    function onSelectedIndexChanged(){
        if (featuresTableView.selectedIndex != null){
            updateGui();
        }
    }

    function updateGui(){
        Events.sendEvent(treeView.commandId + "ClearAllTreeView");

        let featureId = featuresTableView.selectedIndex.itemData.Id;

        Events.sendEvent(treeView.commandId + featureId + "SetVisible", false);

        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");

        if (dependenciesModel.ContainsKey(featureId)){
            let values = dependenciesModel.GetData(featureId);
            let dependencyFeatures = values.split(';')

            for (let i = 0; i < dependencyFeatures.length; i++){
                let dependencyId = dependencyFeatures[i];

                Events.sendEvent(dependencyId + "SetState", Qt.Checked);

                let retVal = [];
                treeView.findChildrenFeatureDependencies(dependencyId, retVal);

                for (let j = 0; j < retVal.length; j++){
                    let childrenDependencyFeatureId = retVal[j];

                    Events.sendEvent(treeView.commandId + childrenDependencyFeatureId + "SetState", Qt.Checked);
                    Events.sendEvent(treeView.commandId + childrenDependencyFeatureId + "SetActive", false);
                }
            }

            let inactiveElements = [];
            treeView.findParentFeatureDependencies(featureId, inactiveElements);

            console.log("inactiveElements", inactiveElements);

            for (let i = 0; i < inactiveElements.length; i++){
                let inactiveElement = inactiveElements[i];

                Events.sendEvent(treeView.commandId + inactiveElement + "SetState", Qt.Unchecked);
                Events.sendEvent(treeView.commandId + inactiveElement + "SetActive", false);
            }
        }
    }

    function updateModel(featureId, checkState){
        console.log("updateDependenciesModel");
        let dependenciesModel = treeView.documentModel.GetData("DependenciesModel");

        if (!dependenciesModel){
            dependenciesModel = treeView.documentModel.AddTreeModel("DependenciesModel");
        }

        let selectedId = tableView.selectedIndex.itemData.Id;
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

//    TreeItemModelConverter {
//        id: converter;
//    }

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
