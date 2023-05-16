import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableTreeViewEditor: null;

    Component.onDestruction: {
        container.tableTreeViewEditor.selectedIndexChanged.disconnect(container.selectedIndexChanged);
    }

    onEntered: {
        container.documentBase.documentModel.SetData("Id", value);
        container.documentBase.documentModel.SetData("Name", value);
    }

    onTableTreeViewEditorChanged: {
        container.tableTreeViewEditor.selectedIndexChanged.connect(container.selectedIndexChanged);
    }

    onSaved: {
        featuresProvider.updateModel();
    }

//    property Component modelIndexComp: Component {
//        ModelIndex {}
//    }

//    function createNewModelIndex(parentIndex, ){
//        let count = model.GetItemsCount();
//        model.InsertNewItemWithParameters(count, {"Id":"", "Name":"Feature Name", "Description":"", "Optional":false})
//    }

    function createNewItem(model){
        let count = model.GetItemsCount();
        model.InsertNewItemWithParameters(count, {"Id":"", "Name":"Feature Name", "Description":"", "Optional":false})
    }

    onCommandActivated: {
        console.log("PackageViewCommands onCommandActivated", commandId);
        if (commandId === "New"){
            let index = container.tableTreeViewEditor.selectedIndex;
            if (index !== null){
                let parentModel = index.getParentModel();
                let childModel = index.getData("ChildModel");
                if (!childModel){
                    if (parentModel){
                        childModel = parentModel.AddTreeModel("ChildModel", index.getIndex());
                    }
                    else{
                        childModel = container.tableTreeViewEditor.rowModel.AddTreeModel("ChildModel", index.getIndex());
                    }
                }

                container.createNewItem(childModel);

                container.tableTreeViewEditor.rowModel = 0;
                container.tableTreeViewEditor.rowModel = container.documentBase.documentModel.GetData("Items");
            }
            else{
                container.createNewItem(tableTreeViewEditor.rowModel);
            }

            container.documentBase.modelChanged();
            container.documentBase.updateModel();
        }
        else if (commandId === "Remove"){
            let index = container.tableTreeViewEditor.selectedIndex;
            if (index !== null){
                let removedFeaturesIds = []
                container.getAllRemovedFeatures(container.tableTreeViewEditor.selectedIndex, removedFeaturesIds);

                let featureId = index.itemData.Id;
                removedFeaturesIds.push(featureId);
                console.log("removedFeaturesIds", removedFeaturesIds);

                for (let removedFeatureId of removedFeaturesIds){
                    container.removeDependencies(removedFeatureId);
                }

                let dependenciesModel = container.documentBase.documentModel.GetData("DependenciesModel");
                console.log("dependenciesModel", dependenciesModel.toJSON());

                container.tableTreeViewEditor.removeByIndex(index);
                container.documentBase.syncronise()
                container.documentBase.modelChanged()
                container.documentBase.updateModel();

                container.tableTreeViewEditor.resetSelection();

//                //Удаление всех зависимостей от этой фичи
//                let dependenciesModel = container.documentBase.documentModel.GetData("DependenciesModel");
//                if (dependenciesModel){
//                    for (let removedFeatureId of removedFeaturesIds){
//                        if (dependenciesModel.ContainsKey(removedFeatureId)){
//                            dependenciesModel.RemoveData(removedFeatureId);
//                        }

//                        let keys = dependenciesModel.GetKeys();

//                        for (let i = 0; i < keys.length; i++){
//                            let key = keys[i];
//                            let values = dependenciesModel.GetData(key);

//                            if (values !== ""){
//                                let dependenciesList = values.split(';');

//                                if (dependenciesList.includes(removedFeatureId)){
//                                    let pos = dependenciesList.indexOf(removedFeatureId);
//                                    dependenciesList.splice(pos, 1);

//                                    let newDependencies = dependenciesList.join(';');
//                                    dependenciesModel.SetData(key, newDependencies);
//                                }
//                            }
//                        }
//                    }
//                }

//                tableTreeViewEditor.removeByIndex(index);
//                container.documentBase.modelChanged()
//                container.documentBase.updateModel();
            }
        }
    }

    function removeDependencies(featureId){
        let dependenciesModel = container.documentBase.documentModel.GetData("DependenciesModel");
        if (dependenciesModel){
            if (dependenciesModel.ContainsKey(featureId)){
                dependenciesModel.RemoveData(featureId);
            }

            let keys = dependenciesModel.GetKeys();

            for (let i = 0; i < keys.length; i++){
                let key = keys[i];
                let values = dependenciesModel.GetData(key);

                if (values !== ""){
                    let dependenciesList = values.split(';');

                    if (dependenciesList.includes(featureId)){
                        let pos = dependenciesList.indexOf(featureId);
                        dependenciesList.splice(pos, 1);

                        let newDependencies = dependenciesList.join(';');
                        dependenciesModel.SetData(key, newDependencies);
                    }
                }
            }
        }
    }

    function selectedIndexChanged(){
        //        let mode = tableTreeViewEditor.selectedIndex != null ? "Normal" : "Disabled";
        let isEnabled = container.tableTreeViewEditor.selectedIndex != null;
        container.documentBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
    }

    property Component messageDialog: Component {
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){

                    let removedFeatureId = container.tableTreeViewEditor.selectedIndex.itemData.Id;
                    let indexes = container.tableTreeViewEditor.selectedIndex.getIndexes();

                    let removedFeaturesIds = [];
                    container.getAllRemovedFeatures(container.tableTreeViewEditor.selectedIndex, removedFeaturesIds);

                    console.log("Remove indexes", indexes);

                    container.tableTreeViewEditor.removeRow(indexes);

                    //Удаление всех зависимостей от этой фичи
                    let dependenciesModel = container.documentBase.documentModel.GetData("DependenciesModel");
                    if (dependenciesModel){

                        for (let removedFeatureId of removedFeaturesIds){
                            if (dependenciesModel.ContainsKey(removedFeatureId)){
                                dependenciesModel.RemoveData(removedFeatureId);
                            }

                            let keys = dependenciesModel.GetKeys();

                            for (let i = 0; i < keys.length; i++){
                                let key = keys[i];
                                let values = dependenciesModel.GetData(key);

                                if (values != ""){
                                    let dependenciesList = values.split(';')

                                    if (dependenciesList.includes(removedFeatureId)){
                                        let pos = dependenciesList.indexOf(removedFeatureId)
                                        dependenciesList.splice(pos, 1);

                                        let newDependencies = dependenciesList.join(';');
                                        dependenciesModel.SetData(key, newDependencies);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    function getAllRemovedFeatures(indexModel, retVal){
        console.log("getAllRemovedFeatures", indexModel, retVal);
        let childModel = indexModel.childModel;

        for (let child of childModel){
            let id = child.itemData.Id;
            retVal.push(id)

            container.getAllRemovedFeatures(child, retVal)
        }
    }
}

