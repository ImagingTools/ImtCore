import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableData: null;

    property ModelIndex selectedIndex: container.tableData != null ? container.tableData.selectedIndex: null;

    signal edited(string itemId, string itemName);
    signal removed(string itemId);

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged");
        let isEnabled = false;

        if (container.selectedIndex != null && container.selectedIndex.itemData){
            let level = container.selectedIndex.itemData.Level;
            if (level === 0){
                isEnabled = true;
            }
        }

        container.documentBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
        container.documentBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        container.documentBase.commandsProvider.setCommandIsEnabled("Duplicate", isEnabled);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);

        if (commandId === "New"){
            container.tableData.insertRow([container.tableData.rowCount], {"Id": "", "Name": "License Name", "Description": "", "Selected": true});
        }
        else if (commandId === "Remove"){
            let selectedIndex = container.tableData.selectedIndex;

            let indexes = selectedIndex.getIndexes();
            if (indexes.length > 1){
                return;
            }

            container.tableData.removeRow([selectedIndex.index]);
        }
    }

    onEntered: {
        container.documentBase.documentModel.SetData("Id", value);
        container.documentBase.documentModel.SetData("Name", value);
    }

    property Component messageDialog: Component {
//        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let selectedIndex = container.tableData.selectedIndex;

                    container.tableData.removeRow([selectedIndex.index]);
                }
            }
        }
    }

    property Component setDescriptionDialog: Component {
//        id: setDescriptionDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elements = container.tableData.elements;
                    elements.SetData("Description", inputValue, container.selectedIndex);

                    container.documentBase.updateGui();
                }
            }
        }
    }

    property Component popupMenu: Component {
//        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("DocumentView PopupMenuDialog", commandId);
                container.commandActivated(commandId);
            }
        }
    }

    property Component editDialog: Component {
//        id: editDialog;
        EditLicenseDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elementsModel = container.documentBase.documentModel.GetData("Items");

                    let oldId = elementsModel.GetData("Id", container.selectedIndex);
                    let oldName = elementsModel.GetData("Name", container.selectedIndex);

                    elementsModel.SetData("Name", valueName, container.selectedIndex);
                    elementsModel.SetData("Id", valueId, container.selectedIndex);

                    container.edited(oldId, oldName);

                    container.documentBase.updateGui();
                }
            }
        }
    }
}

