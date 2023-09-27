import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    property Item tableData: null;

    property int selectedIndex: container.tableData.selectedIndex;

    signal edited(string itemId, string itemName);
    signal removed(string itemId);

    onTableDataChanged: {
        console.log("DocumentView onTableDataChanged");
        container.tableData.rightButtonMouseClicked.connect(container.openPopupMenu);
    }

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", container.selectedIndex, container);
        let isEnabled = container.selectedIndex > -1;

        commandsProvider.setCommandIsEnabled("Remove", isEnabled);
        commandsProvider.setCommandIsEnabled("Edit", isEnabled);
    }

    onCommandActivated: {
        console.log("DocumentCommands onCommandActivated", container.itemId, commandId);

        if (commandId === "New"){
            let elements = container.tableData.elements;
            let index = elements.InsertNewItem();
            elements.SetData("Name", "Item Name", index);
            elements.SetData("Id", "", index);
            elements.SetData("Description", "", index);

            documentBase.updateGui();
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the document ?")});
        }
        else if (commandId === "SetDescription"){
            let elements = container.tableData.elements;
            let selectedDescription = elements.GetData("Description", container.selectedIndex);

            if (!selectedDescription){
                selectedDescription = ""
            }

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the item:"),
                                                                 "inputValue": selectedDescription});
        }
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
                    let elementsModel = documentBase.documentModel.GetData("Items");

                    let selectedIndex = container.tableData.selectedIndex;
                    let removedId = elementsModel.GetData("Id", selectedIndex);

                    elementsModel.RemoveItem(selectedIndex);

                    if (selectedIndex === elementsModel.GetItemsCount()){
                        container.tableData.selectedIndex = -1;
                    }

                    container.removed(removedId);
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
                container.commandActivated(commandId);
            }
        }
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }
}

