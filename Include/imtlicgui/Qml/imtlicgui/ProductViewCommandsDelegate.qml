import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableData: null;

    property ModelIndex selectedIndex: container.tableData != null ? container.tableData.selectedIndex: null;

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged");
        let isEnabled = false;

        if (container.selectedIndex != null){
            let level = container.selectedIndex.depth;
            if (level === 0){
                isEnabled = true;
            }
        }

        container.documentBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
        container.documentBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);

        if (commandId === "New"){
            let count = container.tableData.rowModel.GetItemsCount();

            let licensesModel = container.documentBase.documentModel.GetData("Items");

            let index = licensesModel.InsertNewItem();

            licensesModel.SetData("Id", "", index);
            licensesModel.SetData("Name", "License Name", index);
            licensesModel.SetData("Description", "", index);

            container.tableData.rowModel.InsertNewItemWithParameters(count, {"Id":"", "Name":"License Name", "Description":""})
            container.documentBase.updateModel();

            let item = container.tableData.getRootItemByIndex(count);
            item.select();

            container.tableData.flickableToEnd();
        }
        else if (commandId === "Remove"){
            let selectedIndex = container.tableData.selectedIndex;

            let licenseId = selectedIndex.itemData.Id;

            if (container.documentBase.documentModel.ContainsKey("Features")){
                let featuresModel = container.documentBase.documentModel.GetData("Features");
                if (featuresModel.ContainsKey(licenseId)){
                    featuresModel.RemoveData(licenseId);
                }
            }

            container.removeDependencies(licenseId);

            let licensesModel = container.documentBase.documentModel.GetData("Items");
            licensesModel.RemoveItem(selectedIndex.getIndex());

            container.tableData.rowModel.RemoveItem(selectedIndex.index);

            container.documentBase.modelChanged();
            container.documentBase.updateModel();
            container.tableData.resetSelection();
        }
    }

    function removeDependencies(licenseId){
        if (container.documentBase.documentModel.ContainsKey("Dependencies")){
            let dependenciesModel = container.documentBase.documentModel.GetData("Dependencies");
            if (dependenciesModel.ContainsKey(licenseId)){
                dependenciesModel.RemoveData(licenseId);
            }

            let keys = dependenciesModel.GetKeys();

            for (let i = 0; i < keys.length; i++){
                let key = keys[i];
                let values = dependenciesModel.GetData(key);
                if (values !== ""){
                    let dependenciesList = values.split(';');

                    if (dependenciesList.includes(licenseId)){
                        let pos = dependenciesList.indexOf(licenseId);
                        dependenciesList.splice(pos, 1);

                        if (dependenciesList.length === 0){
                            dependenciesModel.RemoveData(key);
                        }
                        else{
                            let newDependencies = dependenciesList.join(';');
                            dependenciesModel.SetData(key, newDependencies);
                        }
                    }
                }
            }
        }
    }
}

