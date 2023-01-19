import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property Item tableData: null;

    property ModelIndex selectedIndex: tableData != null ? container.tableData.selectedIndex: null;

    signal edited(string itemId, string itemName);
    signal removed(string itemId);

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged");
        let isEnabled = false;

        if (container.selectedIndex != null){
            let level = container.selectedIndex.itemData.Level;
            if (level == 0){
                isEnabled = true;
            }
        }

        commandsProvider.setCommandIsEnabled("Remove", isEnabled);
        commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        commandsProvider.setCommandIsEnabled("Duplicate", isEnabled);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);

        if (commandId === "New"){
            container.tableData.insertRow([container.tableData.rowCount], {"Id": "", "Name": "License Name", "Description": "", "Selected": true});
        }
        else if (commandId === "Remove"){
//            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the document ?")});

            let selectedIndex = container.tableData.selectedIndex;

            container.tableData.removeRow([selectedIndex.index]);
        }
    }

    onEntered: {
        container.objectModel.SetData("Id", value);
        container.objectModel.SetData("Name", value);
    }

    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let selectedIndex = container.tableData.selectedIndex;

                    container.tableData.removeRow([selectedIndex.index]);
                }
            }
        }
    }

    Component {
        id: setDescriptionDialog;
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

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("DocumentView PopupMenuDialog", commandId);
                container.commandActivated(commandId);
            }
        }
    }

    Component {
        id: editDialog;
        EditLicenseDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elementsModel = documentModel.GetData("Items");

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

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }

//    onClosed: {
//        Events.sendEvent("LicenseFeaturesUpdate");
//    }

//    onEdited: {
//        let elementsModel = tableData.elements;
//        let oldLicenseId = itemId;
//        let newLicenseId = elementsModel.GetData("Id", selectedIndex);

//        if (oldLicenseId != newLicenseId){
//            lisensesFeaturesModel.updateLicensesDependenciesAfterLicenseEditing(oldLicenseId, newLicenseId);
//        }
//    }

//    RegExpValidator{
//        id: validatorId;
//        regExp: "[0-9][0-9]\.[0-9999]";
//    }
}

