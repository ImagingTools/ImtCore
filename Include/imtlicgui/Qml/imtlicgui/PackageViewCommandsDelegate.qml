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
        objectModel.SetData("Id", value);
        objectModel.SetData("Name", value);
    }

    onTableTreeViewEditorChanged: {
        container.tableTreeViewEditor.selectedIndexChanged.connect(container.selectedIndexChanged);
    }

    onSaved: {
        featuresProvider.updateModel();
    }

    onCommandActivated: {
        console.log("PackageViewCommands onCommandActivated", commandId);
        if (commandId === "New"){
            let insertIndexes = []
            if (container.tableTreeViewEditor.selectedIndex == null){
                insertIndexes.push(container.tableTreeViewEditor.rowCount);
            }
            else {
                let indexes = container.tableTreeViewEditor.selectedIndex.getIndexes();

                let childrenIndexes = container.tableTreeViewEditor.selectedIndex.childModel;

                indexes.push(childrenIndexes.length);

                insertIndexes = indexes;
            }

            container.tableTreeViewEditor.insertRow(insertIndexes, {"Id": "", "Name": "Feature Name", "Description": "", "Optional": false, "Selected": true});
        }
        else if (commandId === "Remove"){
//            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected feature from the package ?")});

            let removedFeatureId = container.tableTreeViewEditor.selectedIndex.itemData.Id;
            let indexes = container.tableTreeViewEditor.selectedIndex.getIndexes();

            console.log("tableTreeViewEditor.selectedIndex", container.tableTreeViewEditor.selectedIndex);

            let removedFeaturesIds = []
            getAllRemovedFeatures(container.tableTreeViewEditor.selectedIndex, removedFeaturesIds);

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
                            let dependenciesList = values.split(';');

                            if (dependenciesList.includes(removedFeatureId)){
                                let pos = dependenciesList.indexOf(removedFeatureId);
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

    function selectedIndexChanged(){
//        let mode = tableTreeViewEditor.selectedIndex != null ? "Normal" : "Disabled";
        let isEnabled = container.tableTreeViewEditor.selectedIndex != null;
        container.documentBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
    }

    Component {
        id: messageDialog;

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

