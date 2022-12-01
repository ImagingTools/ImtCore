import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: containerBase;

    /**
        The table inside collection view
    */
    property Item tableData;

    /**
        The property for tracking changes to the selected item
    */
    property int selectedIndex: containerBase.tableData ? containerBase.tableData.selectedIndex: -1;

    property string commandsId;

    signal commandActivated(string commandId);

    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    /**
        Requests IDs for GQL models
    */
    property string gqlModelRemove;
    property string gqlModelRename;
    property string gqlModelSetDescription;
    property string gqlModelItem;
    property string gqlModelMetaInfo;

    onTableDataChanged: {
        tableData.rightButtonMouseClicked.connect(openPopupMenu);
    }

    //TODO -> onItemSelectionChanged
    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", containerBase.selectedIndex, containerBase);
        let mode = containerBase.selectedIndex > -1 ? "Normal" : "Disabled";

        commandsProvider.changeCommandMode("Remove", mode);
        commandsProvider.changeCommandMode("Edit", mode);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(containerBase.commandsId + "CommandActivated", containerBase.commandHandle);
        tableData.rightButtonMouseClicked.disconnect(openPopupMenu);
    }

    onCommandsIdChanged: {
        console.log("CollectionCommands onCommandsIdChanged", containerBase.commandsId);
        Events.subscribeEvent(containerBase.commandsId + "CommandActivated", containerBase.commandHandle);
    }

    function openPopupMenu(x, y){
        modalDialogManager.closeDialog();
        modalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
    }

    function commandHandle(commandId){
        console.log("CollectionView commandHandle", commandId);
        if (commandId === "New"){
            baseCollectionView.selectedItem("", "<new item>");
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected item from the collection ?")});
        }
        else if (commandId === "Edit"){
            let itemId = tableData.getSelectedId();
            let itemName = tableData.getSelectedName();
            baseCollectionView.selectedItem(itemId, itemName);
        }
        else if (commandId === "Rename"){
            let selectedName = tableData.getSelectedName();
            modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
        else if (commandId === "SetDescription"){
            console.log("tableData", tableData);
            let elements = tableData.elements;
            let selectedDescription = elements.GetData("Description", selectedIndex);
            console.log("selectedIndex", selectedIndex);
            console.log("selectedDescription", selectedDescription);

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
        }

        else if (commandId === "Close"){
            documentManager.closeDocument(itemId);
        }

        commandActivated(commandId);
    }

    onRenamed: {
        documentManager.setDocumentTitle({"Id": id, "Title": newName});

        updateGui();
    }

    onRemoved: {
        documentManager.closeDocument(id);

        updateGui();
    }

    onDescriptionSetted: {
        updateGui();
    }

    Component {
        id: errorDialog;
        MessageDialog {
        }
    }

    Component {
        id: removeDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let itemId = tableData.getSelectedId();
                    removeModel.updateModel(itemId);
                }
            }
        }
    }

    Component {
        id: renameDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    renameQuery.rename(inputValue);
                }
            }
        }
    }

    Component {
        id: setDescriptionDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    setDescriptionQuery.setDescription(inputValue);
                }
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("CollectionView PopupMenuDialog", commandId);
                commandHandle(commandId);
            }
        }
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
            contextMenuModel.append({"Id": "Rename", "Name": qsTr("Rename"), "IconSource": ""});
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }

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

                            tableData.selectedIndex = -1;

                            if (dataModelLocal.ContainsKey("Id")){
                                var itemId = dataModelLocal.GetData("Id");

                                removed(itemId);
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
            inputParams.InsertField("Id", tableData.getSelectedId());

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

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            let oldId = dataModelLocal.GetData("OldId");
                            let newName = dataModelLocal.GetData("NewName");

                            renamed(oldId, newName);
                        }
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
            inputParams.InsertField("Id", tableData.getSelectedId());

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

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var id = dataModelLocal.GetData("Id");
                            var description = dataModelLocal.GetData("Description");

                            descriptionSetted(id, description);
                        }
                    }
                }
            }
        }
    }
}

