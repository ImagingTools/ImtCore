import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0 as ImtControls
import imtcontrols 1.0
import Qt.labs.platform 1.1

ViewCommandsDelegateBase {
    id: collectionViewCommandsDelegate;

    property Item collectionView: null;

    property string descriptionFieldId: "Description";

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    property TreeItemModel contextMenuModel: TreeItemModel {}

    property alias renameDialogComp: renameDialog;
    property alias removeDialogComp: removeDialog;
    property alias importFileDialog: importFileDialog_;
    property alias exportFileDialog: fileDialogSave;
    property alias setDescriptionDialogComp: setDescriptionDialog;

    property string importObjectTypeId;

    property bool canRename: false;
    property bool canSetDescription: false;

    property var importDialogMimeTypes: []
    property var exportDialogMimeTypes: []
    property var exportContextMenuItems: []

    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);
    signal selectionChanged(var selection)

    Component.onDestruction: {
        if (collectionViewCommandsDelegate.collectionView){
            Events.unSubscribeAllFromSlot(collectionViewCommandsDelegate.commandHandle);
        }
    }

    onCollectionViewChanged: {
        if (collectionView){
            commandsControllerConn.target = collectionView.commandsController
            collectionConnections.target = collectionView;
            elementsConnections.target = collectionView.table;
            controllerConnections.target = collectionView.dataController;
        }
    }

    ListModel {
        id: extensionModel;
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
            collectionViewCommandsDelegate.updateBaseCommandsAccent();
            collectionViewCommandsDelegate.updateItemSelection(selection);

            collectionViewCommandsDelegate.selectionChanged(selection);
        }
    }

    Connections {
        id: elementsConnections;

        function onElementsChanged(){
            let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
            collectionViewCommandsDelegate.updateItemSelection(indexes);
        }
    }

    Connections {
        id: controllerConnections;

        function onImported(){
            ModalDialogManager.showInfoDialog(qsTr("The object has been successfully imported"));
        }

        function onExported(name, data){
            exportFileIO.source = name;

            let encodedStr = Qt.atob(data);
            exportFileIO.write(encodedStr);
        }
    }

    function setupExtensionsModel(){
        extensionModel.clear();

        for (let i = 0; i < fileDialogSave.mimeTypes.length; i++){
            let mimeType = fileDialogSave.mimeTypes[i];
            let name = "";

            if (i < exportContextMenuItems.length){
               name = exportContextMenuItems[i];
            }

            if (name === ""){
                name = mimeType;
            }

            extensionModel.append({"Id": mimeType, "Name": name})
        }
    }

    function getContextMenuModel(){
        return collectionViewCommandsDelegate.contextMenuModel;
    }

    function updateBaseCommandsAccent(){
        let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
        let isEnabled = indexes.length > 0;

        let commandsView = collectionViewCommandsDelegate.collectionView.commandsView;
        if (commandsView){
            if (commandsView.setPositiveAccentCommandIds !== undefined){
                commandsView.setPositiveAccentCommandIds(["New"]);
            }

            if (isEnabled){
                if (commandsView.setNegativeAccentCommandIds !== undefined){
                    commandsView.setNegativeAccentCommandIds(["Remove"]);
                }

                if (commandsView.setPositiveAccentCommandIds !== undefined){
                    commandsView.setPositiveAccentCommandIds(["Edit"]);
                }
            }
        }

        collectionViewCommandsDelegate.updateCustomCommandsAccent();
    }

    function updateCustomCommandsAccent(){}

    function updateItemSelection(selectedItems){
        if (collectionViewCommandsDelegate.collectionView && collectionViewCommandsDelegate.collectionView.commandsController){
            let isEnabled = selectedItems.length > 0;
            let commandsController = collectionViewCommandsDelegate.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
                commandsController.setCommandIsEnabled("Export", isEnabled);
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

    Component {
        id: extensionsPopupMenuDialog;

        PopupMenuDialog {
            onFinished: {
                if (index >= 0){
                    fileDialogSave.fileExt = collectionViewCommandsDelegate.getExtensionFromNameFilter(fileDialogSave.nameFilters, index);
                    fileDialogSave.open();
                }
            }
        }
    }

    // for reimplemented
    function onEdit(){
    }

    function onImport(){
        importFileDialog_.open();
    }

    function onExport(){
        if (Qt.platform.os == "web"){
            setupExtensionsModel();

            if (extensionModel.count === 1){
                fileDialogSave.fileExt = collectionViewCommandsDelegate.getExtensionFromNameFilter(fileDialogSave.nameFilters, 0);

                fileDialogSave.open();
            }
            else{
                ModalDialogManager.openDialog(extensionsPopupMenuDialog, {"x": ModalDialogManager.activeView.width / 2, "y": ModalDialogManager.activeView.height / 2, "model": extensionModel});
            }
        }
        else{
            fileDialogSave.open();
        }
    }

    // importObject(typeId, name, description, b64encoded, ext, additionalParamsObj) - signature in dataController
    function onImportDialogResult(name, fileData, mimeType){
        if (collectionView && collectionView.dataController){
            collectionView.dataController.importObject(importObjectTypeId, name, "", fileData, mimeType);
        }
    }

    // exportObject(objectId, extension, additionalParamsObj) - signature in dataController
    function onExportDialogResult(objectId, mimeType){
        if (collectionView && collectionView.dataController){
            collectionView.dataController.exportObject(objectId, mimeType);
        }
    }

    function getDialogNameFilterIndex(nameFilters, fileExt){
        for (let i = 0; i < nameFilters.length; i++){
            let nameFilter = nameFilters[i];
            let filterExt = nameFilter.split("*.")[1].split(")")[0];
            if (filterExt === fileExt){
                return i;
            }
        }

        return -1;
    }

    function getExtensionFromNameFilter(nameFilters, index){
        let nameFilter = nameFilters[index];
        let filterExt = nameFilter.split("*.")[1].split(")")[0];
        return filterExt;
    }

    FileDialog {
        id: importFileDialog_
        title: qsTr("Import file")
        fileMode: FileDialog.OpenFile
        nameFilters: ["All files (*)"]

        property var mimeTypes: collectionViewCommandsDelegate.importDialogMimeTypes;

        onAccepted: {
            let filePath;
            if (Qt.platform.os == "web"){
                filePath = importFileDialog_.file.toString()
            }
            else{
                filePath = importFileDialog_.file.toString()
            }

            filePath = filePath.replace('file:///', '')

            console.log("filePath", filePath);
            if (Qt.platform.os == "web"){
                let reader = new FileReader()

                reader.readAsDataURL(filePath)

                reader.onload = function(){
                    let encodedContentWithHeader = reader.result
                    let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')

                    let fileName = filePath.name
                    let data = filePath.name.split('.')
                    let ext = "";
                    console.log("fileName", fileName);
                    if (data.length > 1){
                        fileName = data[0]
                        ext = data[1]

                        console.log("fileName", fileName);
                        console.log("ext", ext);
                    }

                    let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
                    if (index < 0){
                        console.error("Extension not found in namefilters");
                        return;
                    }

                    collectionViewCommandsDelegate.onImportDialogResult(fileName, encodedContent, mimeTypes[index]);
                }.bind(this)
            }
            else {
                fileIO.source = filePath
                let fileData = fileIO.read()
                let encodedData = Qt.btoa(fileData);

                let fileName = filePath

                let parts = filePath.split('/')
                if (parts.length > 0){
                    let data = parts[parts.length - 1].split('.')
                    let ext = "";
                    if (data.length > 0){
                        fileName = data[0]
                        ext = data[1]
                    }
                }

                let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
                if (index < 0){
                    console.error("Extension not found in namefilters");
                    return;
                }

                collectionViewCommandsDelegate.onImportDialogResult(fileName, encodedData, mimeTypes[index]);
            }
        }

        FileIO {
            id: fileIO
        }
    }

    function getFormatFromMimeType(mimeType) {
        return mimeType.split('/').pop();
    }

    FileDialog {
        id: fileDialogSave;
        title: qsTr("Save file");
        nameFilters: ["All files (*)"];
        fileMode: FileDialog.SaveFile;
        currentFile: "Default";
        property string fileExt;
        property var mimeTypes: collectionViewCommandsDelegate.exportDialogMimeTypes;

        onAccepted: {
            let filePath = fileDialogSave.file.toString();
            console.log("filePath", filePath);

            let mimeType = ""
            if (Qt.platform.os == "web"){
                exportFileIO.source = fileDialogSave.currentFile;

                let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
                if (index < 0){
                    console.error("Extension not found in name filters");
                    return;
                }

                mimeType = mimeTypes[index]
            }
            else{
                let extension = filePath.substring(filePath.lastIndexOf('.') + 1);
                fileDialogSave.fileExt = extension;

                filePath = filePath.replace('file:///', '')
                exportFileIO.source = filePath;

                let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
                if (index < 0){
                    console.error("Extension not found in name filters");
                    return;
                }

                mimeType = mimeTypes[index]
            }

            let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
            if (indexes.length > 0){
                let index = indexes[0];
                let selectedId = collectionViewCommandsDelegate.collectionView.table.elements.getData("Id", index);
                if (selectedId !== ""){
                    collectionViewCommandsDelegate.onExportDialogResult(selectedId, mimeType);
                }
            }
        }

        FileIO {
            id: exportFileIO
        }
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
        console.log("commandHandle", commandId);

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
            else if (commandId === "Import"){
                collectionViewCommandsDelegate.onImport();
            }
            else if (commandId === "Export"){
                collectionViewCommandsDelegate.onExport();
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
        ImtControls.ErrorDialog {
        }
    }

    Component {
        id: setDescriptionDialog;

        ImtControls.InputDialog {
            width: 300;
            title: qsTr("Set description");
            placeHolderText: qsTr("Enter the description")
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
        ImtControls.InputDialog {
            width: 300;
            title: qsTr("Rename document");
            placeHolderText: qsTr("Enter the name")
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
        ImtControls.MessageDialog {
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

