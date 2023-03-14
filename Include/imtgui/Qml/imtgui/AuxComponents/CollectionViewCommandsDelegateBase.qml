import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: containerBase;

    /**
        The table inside collection view
    */
    property Item tableData: null;

    property Item collectionViewBase: null;

    property Item documentManager: null;

    property CommandsProvider commandsProvider: null;

    /**
        The property for tracking changes to the selected item
    */
    property int selectedIndex: containerBase.tableData ? containerBase.tableData.selectedIndex: -1;

    property string commandsId;

    property var contextMenuModel: null;

    signal commandActivated(string commandId);

    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    signal selectionChanged();

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    /**
        Requests IDs for GQL models
    */
    property string gqlModelRemove;
    property string gqlModelRename;
    property string gqlModelSetDescription;
    property string gqlModelItem;
    property string gqlModelMetaInfo;

    onSelectionChanged: {
        let indexes = containerBase.tableData.getSelectedIndexes();

        let isEnabled = indexes.length > 0;
        if(containerBase.commandsProvider){
            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        }
    }

//    function onSelectionChanged(){
//        let indexes = containerBase.tableData.getSelectedIndexes();

//        let isEnabled = indexes.length > 0;
//        if(containerBase.commandsProvider){
//            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
//            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
//        }
//    }

    onTableDataChanged: {
        if (containerBase.tableData){
            containerBase.tableData.rightButtonMouseClicked.connect(openPopupMenu);

            containerBase.tableData.selectionChanged.connect(containerBase.selectionChanged);
        }
    }

//    //TODO -> onItemSelectionChanged
//    onSelectedIndexChanged: {
//        console.log("CollectionViewCommands onSelectedIndexChanged", containerBase.selectedIndex, containerBase);
//        //        let mode = containerBase.selectedIndex > -1 ? "Normal" : "Disabled";
//        let isEnabled = containerBase.selectedIndex > -1;
//        if(containerBase.commandsProvider){
//            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
//            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
//        }
//    }
    onCommandsProviderChanged: {
        if(commandsProvider){
            let isEnabled = containerBase.selectedIndex > -1;
            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(containerBase.commandsId + "CommandActivated", containerBase.commandHandle);
        if (containerBase.tableData){
            containerBase.tableData.rightButtonMouseClicked.disconnect(containerBase.openPopupMenu);
            containerBase.tableData.selectionChanged.disconnect(containerBase.selectionChanged);
        }
    }

    onCommandsIdChanged: {
        console.log("CollectionCommands onCommandsIdChanged", containerBase.commandsId);
        Events.subscribeEvent(containerBase.commandsId + "CommandActivated", containerBase.commandHandle);
    }

    CollectionViewCommandsObserver {
        commandsProvider: containerBase.commandsProvider;
    }

    function openPopupMenu(x, y){
        modalDialogManager.closeDialog();

        if (containerBase.contextMenuModel != null){
            modalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
        }
    }

    function commandHandle(commandId){
        console.log("CollectionView commandHandle", commandId);
        if (containerBase.commandsProvider == null){
            return;
        }

        let commandIsEnabled = containerBase.commandsProvider.commandIsEnabled(commandId);
        if (commandIsEnabled){
            if (commandId === "New"){
                containerBase.collectionViewBase.selectItem("", "<new item>");
            }
            else if (commandId === "Remove"){
                modalDialogManager.openDialog(removeDialog, {});
            }
            else if (commandId === "Edit"){
                let itemIds = containerBase.tableData.getSelectedIds();
                let itemNames = containerBase.tableData.getSelectedNames();

                if (itemIds.length > 0){
                    let itemId = itemIds[0];
                    let itemName = itemNames[0];

                    containerBase.collectionViewBase.selectItem(itemId, itemName);
                }

//                for (let i = 0; i < itemIds.length; i++){
//                    let itemId = itemIds[i];
//                    let itemName = itemNames[i];

//                    containerBase.collectionViewBase.selectItem(itemId, itemName);
//                }

//                let itemId = containerBase.tableData.getSelectedId();
//                let itemName = containerBase.tableData.getSelectedName();
//                containerBase.collectionViewBase.selectItem(itemId, itemName);
            }
        }

        let editIsEnabled = containerBase.commandsProvider.commandIsEnabled("Edit");
        if (editIsEnabled){
            if (commandId === "Rename"){
                let selectedName = containerBase.tableData.getSelectedName();
                modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
            }
            else if (commandId === "SetDescription"){
                let elements = containerBase.tableData.elements;
                let selectedDescription = "";

                if (elements.ContainsKey("Description", selectedIndex)){
                    selectedDescription = elements.GetData("Description", selectedIndex);
                }

                modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
            }
        }

        if (commandId === "Close"){
            containerBase.documentManager.closeDocument(itemId);
        }

        commandActivated(commandId);
    }

    onRenamed: {
        containerBase.documentManager.setDocumentTitle({"Id": id, "Title": newName});

        containerBase.collectionViewBase.updateGui();
    }

    onRemoved: {
        console.log('onRemoved',id)
        if (containerBase.documentManager){
            containerBase.documentManager.closeDocument(id);
        }

        containerBase.collectionViewBase.updateGui();
    }

    onDescriptionSetted: {
        containerBase.collectionViewBase.updateGui();
    }

    Component {
        id: errorDialog;
        ErrorDialog {
        }
    }

    Component {
        id: removeDialog;
        MessageDialog {
            message: containerBase.removeMessage;
            title: containerBase.removeDialogTitle;
            onFinished: {
                if (buttonId == "Yes"){
//                    let itemId = containerBase.tableData.getSelectedId();
//                    removeModel.updateModel(itemId);

                    let itemIds = containerBase.tableData.getSelectedIds();
                    let itemNames = containerBase.tableData.getSelectedNames();

                    for (let i = 0; i < itemIds.length; i++){
                        let itemId = itemIds[i];
                        let itemName = itemNames[i];

                        removeModel.updateModel(itemId);
                    }
                }

                tableData.forceActiveFocus();
            }
        }
    }

    Component {
        id: renameDialog;
        InputDialog {
            title: qsTr("Rename document");
            onFinished: {
                if (buttonId == "Ok"){
                    renameQuery.rename(inputValue);
                }

                tableData.forceActiveFocus();
            }
        }
    }

    Component {
        id: setDescriptionDialog;
        InputDialog {
            title: qsTr("Set description");
            onFinished: {
                if (buttonId == "Ok"){
                    setDescriptionQuery.setDescription(inputValue);
                }

                tableData.forceActiveFocus();
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("CollectionView PopupMenuDialog", commandId);
                containerBase.commandHandle(commandId);
            }
        }
    }

//    ListModel {
//        id: contextMenuModel;

//        Component.onCompleted: {
//            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
//            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
//            contextMenuModel.append({"Id": "Rename", "Name": qsTr("Rename"), "IconSource": ""});
//            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
//        }
//    }

    /**
        GQL Request for removing element from the collection by object id
    */
    GqlModel {
        id: removeModel;

        function updateModel(itemId) {
            console.log( "updateModel removeModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(itemId !== ""){
                query = Gql.GqlRequest("query", containerBase.gqlModelRemove);
                inputParams.InsertField("Id", itemId);
                queryFields = Gql.GqlObject("removedNotification");
                query.AddParam(inputParams);

                queryFields.InsertField("Id");
                queryFields.InsertField("Successed");

                query.AddField(queryFields);

                var gqlData = query.GetQuery();
                console.log("removeModel query ", gqlData);
                this.SetGqlQuery(gqlData);
            }
        }

        onStateChanged: {
            console.log("State:", this.state, removeModel);
            if (this.state === "Ready"){

                var dataModelLocal;
                if (removeModel.ContainsKey("errors")){
                    dataModelLocal = removeModel.GetData("errors");
                    if (dataModelLocal.ContainsKey(containerBase.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelRemove);
                        var message = dataModelLocal.GetData("message");
                        modalDialogManager.openDialog(errorDialog, {"message": message});
                    }

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.gqlModelRemove)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelRemove);

                        if (dataModelLocal.ContainsKey("removedNotification")){
                            dataModelLocal = dataModelLocal.GetData("removedNotification");

                            containerBase.tableData.selectedIndex = -1;

                            if (dataModelLocal.ContainsKey("Id")){
                                var itemId = dataModelLocal.GetData("Id");

                                containerBase.removed(itemId);
                            }
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: renameQuery;

        function rename(newName) {
            console.log( "CollectionView renameQuery rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", containerBase.gqlModelRename);
            inputParams.InsertField("Id", containerBase.tableData.getSelectedId());

            inputParams.InsertField("NewName", newName);

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("rename");
            queryFields.InsertField("NewName");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView renameQuery rename", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, renameQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (renameQuery.ContainsKey("errors")){
                    dataModelLocal = renameQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(containerBase.gqlModelRename)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelRename);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            modalDialogManager.openDialog(errorDialog, {"message": message});
                        }
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.gqlModelRename)) {
                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelRename);

                        let oldId = dataModelLocal.GetData("Id");
                        let newName = dataModelLocal.GetData("Name");

                        containerBase.renamed(oldId, newName);
                    }
                }
            }
        }
    }

    GqlModel {
        id: setDescriptionQuery;

        function setDescription(description){
            console.log( "CollectionView setDescriptionQuery");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", containerBase.gqlModelSetDescription);
            inputParams.InsertField("Id", containerBase.tableData.getSelectedId());

            inputParams.InsertField("Description", description);

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("setDescription");
            queryFields.InsertField("Description");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionView setDescription query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, setDescriptionQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (setDescriptionQuery.ContainsKey("errors")){
                    dataModelLocal = setDescriptionQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(containerBase.gqlModelSetDescription)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            modalDialogManager.openDialog(errorDialog, {"message": message});
                        }
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.gqlModelSetDescription)) {

                        dataModelLocal = dataModelLocal.GetData(containerBase.gqlModelSetDescription);

                        var id = dataModelLocal.GetData("Id");
                        var description = dataModelLocal.GetData("Description");

                        containerBase.descriptionSetted(id, description);
                    }
                }
            }
        }
    }
}

