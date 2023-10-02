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
        container.documentBase.itemId = value;
        container.documentBase.itemName = value;
//        container.documentBase.documentModel.SetData("Id", value);
//        container.documentBase.documentModel.SetData("Name", value);
    }

    onTableTreeViewEditorChanged: {
        container.tableTreeViewEditor.selectedIndexChanged.connect(container.selectedIndexChanged);
    }

    onSaved: {
        featuresProvider.updateModel();
    }

    function createNewItem(model){
        let count = model.GetItemsCount();
        model.InsertNewItemWithParameters(count, {"Id":"", "Name":"Feature Name", "Description":"", "Optional":false})
    }

    onCommandActivated: {
        console.log("PackageViewCommands onCommandActivated", commandId);
        if (commandId === "New"){
            let insertedIndexes = []
            let count = tableTreeViewEditor.rowModel.GetItemsCount();
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

                insertedIndexes = insertedIndexes.concat(index.getIndexes());

                container.createNewItem(childModel);

                let childCount = childModel.GetItemsCount();

                container.tableTreeViewEditor.rowModel = 0;
                container.tableTreeViewEditor.rowModel = container.documentBase.documentModel.GetData("Items");

                insertedIndexes.push(childCount - 1);
            }
            else{
                container.createNewItem(tableTreeViewEditor.rowModel);
                insertedIndexes.push(count);
            }

            container.documentBase.modelChanged();
            container.documentBase.updateModel();

            let delegateItems = container.tableTreeViewEditor.getItemsDataAsList();
            for (let delegateItem of delegateItems){
                let delegateIndex = delegateItem.modelIndex;
                if (delegateIndex.equalByArrayIndexes(insertedIndexes)){
                    container.tableTreeViewEditor.singleSelect(delegateIndex);
                    break;
                }
            }
        }
        else if (commandId === "Remove"){
            let index = container.tableTreeViewEditor.selectedIndex;
            if (index !== null){
                let removedFeaturesIds = []
                container.getAllRemovedFeatures(container.tableTreeViewEditor.selectedIndex, removedFeaturesIds);

                let featureId = index.itemData.Id;
                removedFeaturesIds.push(featureId);

                for (let removedFeatureId of removedFeaturesIds){
                    container.removeDependencies(removedFeatureId);
                }

                let delegateItems = container.tableTreeViewEditor.getItemsDataAsList();
                container.tableTreeViewEditor.removeByIndex(index);
                container.documentBase.syncronise()
                container.documentBase.modelChanged()
                container.documentBase.updateModel();

                container.tableTreeViewEditor.resetSelection();

                delegateItems = container.tableTreeViewEditor.getItemsDataAsList();
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

