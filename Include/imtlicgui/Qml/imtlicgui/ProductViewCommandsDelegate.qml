import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: false;

    property Item tableData;

    property int selectedIndex: container.tableData.selectedIndex;

    signal edited(string itemId, string itemName);
    signal removed(string itemId);

    onTableDataChanged: {
        console.log("DocumentView onTableDataChanged");
        tableData.rightButtonMouseClicked.connect(openPopupMenu);
    }

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", container.selectedIndex, container);
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";

        commandsProvider.changeCommandMode("Remove", mode);
        commandsProvider.changeCommandMode("Edit", mode);
        commandsProvider.changeCommandMode("Duplicate", mode);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);

        if (commandId === "New"){
            let elements = container.tableData.elements;
            let index = elements.InsertNewItem();
            elements.SetData("Name", "License Name", index);
            elements.SetData("Id", "", index);
            elements.SetData("Description", "", index);

            documentBase.updateGui();
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the document ?")});
        }
        else if (commandId === "SetDescription"){
            let elements = tableData.elements;
            let selectedDescription = elements.GetData("Description", selectedIndex);

            if (!selectedDescription){
                selectedDescription = ""
            }

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the item:"),
                                                                 "inputValue": selectedDescription});
        }
        else if (commandId === "Duplicate"){
            let elements = container.tableData.elements;

            let selectedId = elements.GetData("Id", container.selectedIndex);
            let selectedName = elements.GetData("Name", container.selectedIndex);
            let selectedDescription = elements.GetData("Description", container.selectedIndex);

            let index = elements.InsertNewItem();
            elements.SetData("Name", "Copy of " + selectedName, index);
            elements.SetData("Id", selectedId, index);
            elements.SetData("Description", selectedDescription, index);
        }
        else if (commandId === "Edit"){
            let id = tableData.getSelectedId();
            let name = tableData.getSelectedName();

            modalDialogManager.openDialog(editDialog, {"valueId":      id,
                                                       "valueName":    name,
                                                       "licensesModel":tableData.elements,
                                                       "index":        tableData.selectedIndex});
        }

        updateGui();
    }

    function openPopupMenu(x, y){
        modalDialogManager.closeDialog();
        modalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
    }

    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let elementsModel = documentModel.GetData("Items");

                    let selectedIndex = container.tableData.selectedIndex;
                    let removedId = elementsModel.GetData("Id", selectedIndex);

                    elementsModel.RemoveItem(selectedIndex);

                    if (selectedIndex === elementsModel.GetItemsCount()){
                        container.tableData.selectedIndex = -1;
                    }

                    removed(removedId);
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
                    elements.SetData("Description", inputValue, selectedIndex);

                    documentBase.updateGui();
                }
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("DocumentView PopupMenuDialog", commandId);
                commandActivated(commandId);
            }
        }
    }

    Component {
        id: editDialog;
        EditLicenseDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elementsModel = documentModel.GetData("Items");

                    let oldId = elementsModel.GetData("Id", selectedIndex);
                    let oldName = elementsModel.GetData("Name", selectedIndex);

                    elementsModel.SetData("Name", valueName, container.selectedIndex);
                    elementsModel.SetData("Id", valueId, container.selectedIndex);

                    edited(oldId, oldName);

                    documentBase.updateGui();
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

