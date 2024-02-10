import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

Item {
    id: container;

    property Item collectionView: null;

    property string descriptionFieldId: "Description";

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    signal commandActivated(string commandId);
    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    Component.onDestruction: {
        if (container.collectionView){
            Events.unSubscribeEvent(container.collectionView.uuid + "CommandActivated", container.commandHandle)
        }
    }

    Connections {
        target: container.collectionView;

        function onSelectionChanged(selection){
            console.log("CW selectionChanged", selection)

            container.updateItemSelection(selection);
        }

        function onViewIdChanged(){
            console.log("Delegate onUuidChanged");
            let uuid = container.collectionView.viewId;

            Events.subscribeEvent(uuid + "CommandActivated", container.commandHandle)
        }
    }

    function updateItemSelection(selectedItems){
        console.log("updateItemSelection", selectedItems);
        if (container.collectionView && container.collectionView.commandsController){
            let isEnabled = selectedItems.length > 0;
            let commandsController = container.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
            }
        }
    }

    function setupContextMenu(contextMenu){
        if (contextMenu){
            contextMenu.Clear();


        }
    }

    function onEdit(){
        console.log("onEdit");
    }

    function onRemove(){
        modalDialogManager.openDialog(removeDialog, {});
    }

    function onNew(){}

    function onRename(){
        let indexes = container.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = container.collectionView.table.elements.GetData("Name", indexes[0]);
            modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }

    function onSetDescription(){
        let elements = container.collectionView.table.elements;

        let indexes = container.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedDescription = "";

            if (elements.ContainsKey(container.descriptionFieldId, indexes[0])){
                selectedDescription = elements.GetData(container.descriptionFieldId, indexes[0]);
            }

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
        }
    }

    function commandHandle(commandId){
        console.log("commandHandle", commandId);
        console.log("container.collectionView", container.collectionView);

        if (!container.collectionView){
            return;
        }

        let commandsController = container.collectionView.commandsController;
        console.log("commandsController", commandsController);

        let commandIsEnabled = commandsController.commandIsEnabled(commandId);
        console.log("commandIsEnabled", commandIsEnabled);

        if (commandIsEnabled){
            if (commandId === "New"){
                container.onNew();
            }
            else if (commandId === "Remove"){
                container.onRemove();
            }
            else if (commandId === "Edit"){
                console.log("=== Edit");

                container.onEdit();
            }
        }

        let editIsEnabled = commandsController.commandIsEnabled("Edit");
        if (editIsEnabled){
            if (commandId === "Rename"){
                container.onRename();
            }
            else if (commandId === "SetDescription"){
                container.onSetDescription();
            }
        }

        container.commandActivated(commandId);
    }

    Component {
        id: errorDialog;
        ErrorDialog {
        }
    }

    Component {
        id: setDescriptionDialog;
        InputDialog {
            title: qsTr("Set description");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = container.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        container.collectionView.setElementDescription(index, inputValue)
                    }
                }
            }
        }
    }

    Component {
        id: renameDialog;
        InputDialog {
            title: qsTr("Rename document");
            onFinished: {
                if (buttonId == Enums.ok){
                    let indexes = container.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        container.collectionView.setElementName(index, inputValue)
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
                    let indexes = container.collectionView.table.getSelectedIndexes();
                    if (indexes.length >= 0){
                        let index = indexes[0];
                        container.collectionView.removeElement(index)
                    }
                }
            }
        }
    }
}

