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

    //    property var templateContextMenuModel: [
    //        {"Id": "Edit"},
    //        {"Id": ""},
    //        {"Id": "Rename"},
    //        {"Id": "SetDescription"}
    //    ]

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

            if (collectionView.commandsController){
//                collectionView.commandsController.commandsModelChanged.connect(internal.onCommandsModelChanged);

//                collectionViewCommandsDelegate.setupContextMenu();
            }
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
        console.log("setupContextMenu", collectionView);
        let commandsController = collectionView.commandsController;
        if (commandsController){
            collectionViewCommandsDelegate.contextMenuModel.Clear();

            let canEdit = commandsController.commandExists("Edit");
            let canRemove = commandsController.commandExists("Remove");

            if (canEdit){
                let index = collectionViewCommandsDelegate.contextMenuModel.InsertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.SetData("Id", "Edit", index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Name", qsTr("Edit"), index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Icon", "Icons/Edit", index);
            }

            if (canRemove){
                let index = collectionViewCommandsDelegate.contextMenuModel.InsertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.SetData("Id", "Remove", index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Name", qsTr("Remove"), index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Icon", "Icons/Delete", index);
            }

            if (canEdit){
                let index = collectionViewCommandsDelegate.contextMenuModel.InsertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.SetData("Id", "Rename", index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Name", qsTr("Rename"), index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Icon", "", index);

                index = collectionViewCommandsDelegate.contextMenuModel.InsertNewItem();

                collectionViewCommandsDelegate.contextMenuModel.SetData("Id", "SetDescription", index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Name", qsTr("Set Description"), index);
                collectionViewCommandsDelegate.contextMenuModel.SetData("Icon", "", index);
            }

            collectionViewCommandsDelegate.contextMenuModel.Refresh();
        }
    }

    function onEdit(){
    }

    function onRemove(){
        modalDialogManager.openDialog(removeDialog, {});
    }

    function onNew(){}

    function onRename(){
        let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = collectionViewCommandsDelegate.collectionView.table.elements.GetData("Name", indexes[0]);
            modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }

    function onSetDescription(){
        let elements = collectionViewCommandsDelegate.collectionView.table.elements;

        let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedDescription = "";

            if (elements.ContainsKey(collectionViewCommandsDelegate.descriptionFieldId, indexes[0])){
                selectedDescription = elements.GetData(collectionViewCommandsDelegate.descriptionFieldId, indexes[0]);
            }

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
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
            title: qsTr("Set Description");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.setElementDescription(index, inputValue)
                    }
                }
            }
        }
    }

    Component {
        id: renameDialog;
        InputDialog {
            title: qsTr("Rename Document");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.setElementName(index, inputValue)
                    }
                }
            }
        }
    }

    Component {
        id: removeDialog;
        MessageDialog {
            message: qsTr("Deleting a selected element");
            title: qsTr("Remove selected item from the collection ?");
            onFinished: {
                console.log("removeDialog onFinished", buttonId);
                if (buttonId == Enums.yes){
                    let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        collectionViewCommandsDelegate.collectionView.removeElement(index)
                    }
                }
            }
        }
    }
}

