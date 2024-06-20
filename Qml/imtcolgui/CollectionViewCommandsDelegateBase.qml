import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

ViewCommandsDelegateBase {
    id: collectionViewCommandsDelegate;

    property Item collectionView: null;

    property string descriptionFieldId: "Description";

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    property TreeItemModel contextMenuModel: TreeItemModel {}

    property alias renameDialogComp: renameDialog;
    property alias removeDialogComp: removeDialog;
    property alias setDescriptionDialogComp: setDescriptionDialog;

    property bool canRename: false;
    property bool canSetDescription: false;

    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    Component.onDestruction: {
        if (collectionViewCommandsDelegate.collectionView){
            Events.unSubscribeAllFromSlot(collectionViewCommandsDelegate.commandHandle);
        }
    }

    onCollectionViewChanged: {
        if (collectionView){
            collectionConnections.target = collectionView;
            elementsConnections.target = collectionView.table;
            commandsControllerConn.target = collectionView.commandsController
        }
    }

    Connections {
        id: commandsControllerConn;

        function onCommandsModelChanged(){
            collectionViewCommandsDelegate.setupContextMenu();
        }
    }

    Connections {
        id: collectionConnections;

        function onSelectionChanged(selection){
            collectionViewCommandsDelegate.updateItemSelection(selection);
        }
    }

    QtObject {
        id: internal;

        function onSelectionChanged(selection){
            collectionViewCommandsDelegate.updateItemSelection(selection);
        }

        function onCommandsModelChanged(){
            collectionViewCommandsDelegate.setupContextMenu();
        }
    }

    Connections {
        id: elementsConnections;

        function onElementsChanged(){
            let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
            collectionViewCommandsDelegate.updateItemSelection(indexes);
        }
    }

    function getContextMenuModel(){
        return collectionViewCommandsDelegate.contextMenuModel;
    }

    function updateItemSelection(selectedItems){
        if (collectionViewCommandsDelegate.collectionView && collectionViewCommandsDelegate.collectionView.commandsController){
            let isEnabled = selectedItems.length > 0;
            let commandsController = collectionViewCommandsDelegate.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
            }
        }
    }

    function setupContextMenu(){
        let commandsController = collectionView.commandsController;
        if (commandsController){
            collectionViewCommandsDelegate.contextMenuModel.clear();

            let canEdit = commandsController.commandExists("Edit");
            let canRemove = commandsController.commandExists("Remove");

            if (canEdit){
                let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.setData("Id", "Edit", index);
                collectionViewCommandsDelegate.contextMenuModel.setData("Name", qsTr("Edit"), index);
                collectionViewCommandsDelegate.contextMenuModel.setData("Icon", "Icons/Edit", index);
            }

            if (canRemove){
                let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.setData("Id", "Remove", index);
                collectionViewCommandsDelegate.contextMenuModel.setData("Name", qsTr("Remove"), index);
                collectionViewCommandsDelegate.contextMenuModel.setData("Icon", "Icons/Delete", index);
            }

            if (canEdit){
                if (collectionViewCommandsDelegate.canRename){
                    let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

                    collectionViewCommandsDelegate.contextMenuModel.setData("Id", "Rename", index);
                    collectionViewCommandsDelegate.contextMenuModel.setData("Name", qsTr("Rename"), index);
                    collectionViewCommandsDelegate.contextMenuModel.setData("Icon", "", index);
                }

                if (collectionViewCommandsDelegate.canSetDescription){
                    let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

                    collectionViewCommandsDelegate.contextMenuModel.setData("Id", "SetDescription", index);
                    collectionViewCommandsDelegate.contextMenuModel.setData("Name", qsTr("Set Description"), index);
                    collectionViewCommandsDelegate.contextMenuModel.setData("Icon", "", index);
                }
            }

            collectionViewCommandsDelegate.contextMenuModel.refresh();
        }
    }

    function onEdit(){
    }

    function onRemove(){
        ModalDialogManager.openDialog(removeDialog, {});
    }

    function onNew(){}

    function onRename(){
        let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = collectionViewCommandsDelegate.collectionView.table.elements.getData("Name", indexes[0]);
            ModalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }

    function onSetDescription(){
        let elements = collectionViewCommandsDelegate.collectionView.table.elements;

        let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedDescription = "";

            if (elements.containsKey(collectionViewCommandsDelegate.descriptionFieldId, indexes[0])){
                selectedDescription = elements.getData(collectionViewCommandsDelegate.descriptionFieldId, indexes[0]);
            }

            ModalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
        }
    }

    function commandHandle(commandId){
        if (!collectionViewCommandsDelegate.collectionView){
            return;
        }

        let commandsController = collectionViewCommandsDelegate.collectionView.commandsController;
        if (!commandsController){
            return;
        }

        let commandIsEnabled = commandsController.commandIsEnabled(commandId);
        if (commandIsEnabled){
            if (commandId === "New"){
                collectionViewCommandsDelegate.onNew();
            }
            else if (commandId === "Remove"){
                collectionViewCommandsDelegate.onRemove();
            }
            else if (commandId === "Edit"){
                collectionViewCommandsDelegate.onEdit();
            }
        }

        let editIsEnabled = commandsController.commandIsEnabled("Edit");
        if (editIsEnabled){
            if (commandId === "Rename"){
                collectionViewCommandsDelegate.onRename();
            }
            else if (commandId === "SetDescription"){
                collectionViewCommandsDelegate.onSetDescription();
            }
        }

        collectionViewCommandsDelegate.commandActivated(commandId);
    }

    Component {
        id: errorDialog;
        ErrorDialog {
        }
    }

    Component {
        id: setDescriptionDialog;

        InputDialog {
            width: 300;
            title: qsTr("Set description");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.setElementDescription(index, inputValue)
                    }
                }

                if (collectionViewCommandsDelegate.collectionView){
                    collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
                }
            }
        }
    }

    Component {
        id: renameDialog;
        InputDialog {
            width: 300;
            title: qsTr("Rename document");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.setElementName(index, inputValue)
                    }
                }

                if (collectionViewCommandsDelegate.collectionView){
                    collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
                }
            }
        }
    }

    Component {
        id: removeDialog;
        MessageDialog {
            width: 400;
            title: collectionViewCommandsDelegate.removeDialogTitle;
            message: collectionViewCommandsDelegate.removeMessage;
            onFinished: {
                if (buttonId == Enums.yes){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.removeElement(index)
                    }
                }

                if (collectionViewCommandsDelegate.collectionView){
                    collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
                }
            }
        }
    }
}

