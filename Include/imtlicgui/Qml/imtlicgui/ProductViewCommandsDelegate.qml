import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableData: null;

    property ModelIndex selectedIndex: container.tableData != null ? container.tableData.selectedIndex: null;

//    signal edited(string itemId, string itemName);
//    signal removed(string itemId);

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
//            licensesModel.InsertNewItemWithParameters(count, {"Id":"", "Name":"License Name", "Description":""})

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

            let licensesModel = container.documentBase.documentModel.GetData("Items");
            licensesModel.RemoveItem(selectedIndex.getIndex());

            container.tableData.rowModel.RemoveItem(selectedIndex.index);

            container.documentBase.modelChanged();
            container.documentBase.updateModel();
            container.tableData.resetSelection();
        }
    }

//    onEntered: {
//        container.documentBase.documentModel.SetData("Id", value);
//        container.documentBase.documentModel.SetData("Name", value);
//    }

//    property Component messageDialog: Component {
//        MessageDialog {
//            onFinished: {
//                if (buttonId == "Yes"){
//                    let selectedIndex = container.tableData.selectedIndex;

//                    container.tableData.removeRow([selectedIndex.index]);
//                }
//            }
//        }
//    }

//    property Component setDescriptionDialog: Component {
//        InputDialog {
//            onFinished: {
//                if (buttonId == "Ok"){
//                    let elements = container.tableData.elements;
//                    elements.SetData("Description", inputValue, container.selectedIndex);

//                    container.documentBase.updateGui();
//                }
//            }
//        }
//    }

//    property Component popupMenu: Component {
//        PopupMenuDialog {
//            onFinished: {
//                console.log("DocumentView PopupMenuDialog", commandId);
//                container.commandActivated(commandId);
//            }
//        }
//    }

//    property Component editDialog: Component {
//        EditLicenseDialog {
//            onFinished: {
//                if (buttonId == "Ok"){
//                    let elementsModel = container.documentBase.documentModel.GetData("Items");

//                    let oldId = elementsModel.GetData("Id", container.selectedIndex);
//                    let oldName = elementsModel.GetData("Name", container.selectedIndex);

//                    elementsModel.SetData("Name", valueName, container.selectedIndex);
//                    elementsModel.SetData("Id", valueId, container.selectedIndex);

//                    container.edited(oldId, oldName);

//                    container.documentBase.updateGui();
//                }
//            }
//        }
//    }
}

