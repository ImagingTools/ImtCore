import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0

Item {
    id: containerBase;

    /**
        The table inside collection view
    */
    property Item tableData: null;

    property Item collectionViewBase: null;

    property Item documentManagerPtr: null;

    property CommandsProvider commandsProvider: null;

    /**
        The property for tracking changes to the selected item
    */
    property int selectedIndex: containerBase.tableData ? containerBase.tableData.selectedIndex: -1;

    property string commandId;

    property string descriptionFieldId: "Description";

    property var contextMenuModel: null;

    property bool autoUpdateAfterChanges: false;

    property alias removeGqlModel: removeModel;
    property alias renameGqlModel: renameQuery;
    property alias setDescriptionGqlModel: setDescriptionQuery;

    property alias setDescriptionDialogComp : setDescriptionDialog;

    signal selectionChanged();

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    /**
        Requests IDs for GQL models
    */
    property string removeGqlCommand;
    property string renameGqlCommand;
    property string setDescriptionGqlCommand;

    signal commandActivated(string commandId);
    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    onDescriptionSetted: {
        if (autoUpdateAfterChanges){
            containerBase.collectionViewBase.updateGui();
        }
    }

    onSelectionChanged: {
        console.log("CollectionViewCommandsDelegateBase onSelectionChanged", containerBase.commandsProvider);
        let indexes = containerBase.tableData.getSelectedIndexes();

        let isEnabled = indexes.length > 0;
        if(containerBase.commandsProvider){
            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        }
    }

    onTableDataChanged: {
        if (containerBase.tableData){
            if (containerBase.tableData.rightButtonMouseClicked){
                containerBase.tableData.rightButtonMouseClicked.connect(openPopupMenu);
            }
            containerBase.selectionChanged.connect(containerBase.selectionChanged);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(containerBase.commandId + "CommandActivated", containerBase.commandHandle);
    }

    onCommandIdChanged: {
        console.log("CollectionCommands oncommandIdChanged", containerBase.commandId);
        Events.subscribeEvent(containerBase.commandId + "CommandActivated", containerBase.commandHandle);

        if (containerBase.commandId !== ""){
            removeGqlCommand = containerBase.commandId + "Remove";
            renameGqlCommand = containerBase.commandId + "Rename";
            setDescriptionGqlCommand = containerBase.commandId + "SetDescription";
        }
    }

    onRenamed: {
//        containerBase.documentManagerPtr.setDocumentTitle({"Id": id, "Title": newName});
        if (autoUpdateAfterChanges){
            containerBase.collectionViewBase.updateGui();
        }
    }

    onRemoved: {
        console.log('onRemoved',id)
        console.log('documentManagerPtr',containerBase.documentManagerPtr)
        if (containerBase.documentManagerPtr){
            let documentIndex = containerBase.documentManagerPtr.getDocumentIndexByDocumentId(id);
            console.log("documentIndex", documentIndex)

            if (documentIndex >= 0){
                containerBase.documentManagerPtr.closeDocumentByIndex(documentIndex, true);
            }
        }

        if (autoUpdateAfterChanges){
            containerBase.collectionViewBase.updateGui();
        }
    }

    CollectionViewCommandsObserver {
        commandsProvider: containerBase.commandsProvider;
    }

    function openPopupMenu(x, y){
        console.log("openPopupMenu")
        modalDialogManager.closeDialog();

        if (containerBase.contextMenuModel != null){
            console.log("openPopupMenu", x, y)

            let offset = 26 * contextMenuModel.count;
            modalDialogManager.openDialog(popupMenu, {"x": x, "y": y - offset, "model": contextMenuModel});
        }
    }

    function onEdit(){
        console.log("CollectionView onEdit");
        let elementsModel = containerBase.tableData.elements;
        if (!elementsModel){
            return;
        }

        let indexes = containerBase.tableData.getSelectedIndexes();
        if (indexes.length > 0){
            let index = indexes[0];

            let itemId = elementsModel.GetData("Id", index);
            let itemName = elementsModel.GetData("Name", index);
            containerBase.collectionViewBase.selectItem(itemId, itemName);
        }
    }

    function onRemove(){
        if (containerBase.commandId === "" || removeGqlCommand === ""){
            console.error("Remove item was failed!")
            return;
        }

        modalDialogManager.openDialog(removeDialog, {});
    }

    function onNew(){
        containerBase.collectionViewBase.selectItem("", "");
    }

    function onRename(){
        if (containerBase.commandId === "" || renameGqlCommand === ""){
            console.error("Rename item was failed!")
            return;
        }

        let indexes = containerBase.tableData.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = containerBase.tableData.elements.GetData("Name", indexes[0]);
            modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }

    function onSetDescription(){
        if (containerBase.commandId === "" || setDescriptionGqlCommand === ""){
            console.error("Set description to the item was failed!")
            return;
        }

        let elements = containerBase.tableData.elements;

        let indexes = containerBase.tableData.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedDescription = "";

            if (elements.ContainsKey(containerBase.descriptionFieldId, indexes[0])){
                selectedDescription = elements.GetData(containerBase.descriptionFieldId, indexes[0]);
            }

            modalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
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
                containerBase.onNew();
            }
            else if (commandId === "Remove"){
                containerBase.onRemove();
            }
            else if (commandId === "Edit"){
                containerBase.onEdit();
            }
        }

        let editIsEnabled = containerBase.commandsProvider.commandIsEnabled("Edit");
        if (editIsEnabled){
            if (commandId === "Rename"){
                containerBase.onRename();
            }
            else if (commandId === "SetDescription"){
                containerBase.onSetDescription();
            }
        }

        if (commandId === "Close"){
            if (containerBase.documentManagerPtr){
                let uuid = containerBase.collectionViewBase.uuid;
                containerBase.documentManagerPtr.closeDocument(uuid);
            }
        }

        commandActivated(commandId);
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
                    let indexes = containerBase.tableData.getSelectedIndexes();
                    if (indexes.length > 0){
                        let index = indexes[0];
                        let id = containerBase.tableData.elements.GetData("Id", index);

                        console.log("index", index);
                        removeModel.updateModel(id);
                    }
                }

                containerBase.tableData.forceActiveFocus();
            }
        }
    }

    property Component renameDialog: Component {
        InputDialog {
            title: qsTr("Rename document");
            onFinished: {
                if (buttonId == "Ok"){
                    renameQuery.rename(inputValue);
                }

                containerBase.tableData.forceActiveFocus();
            }
        }
    }

    Component {
        id: setDescriptionDialog;
        InputDialog {
            title: qsTr("Set description");
            onFinished: {
                let elements = containerBase.tableData.elements;
                let indexes = containerBase.tableData.getSelectedIndexes();
                if (indexes.length === 1){
                    elements.SetData(containerBase.descriptionFieldId, inputValue, indexes[0]);
                }

                if (buttonId == "Ok"){
                    setDescriptionQuery.setDescription(inputValue);
                }

                containerBase.tableData.forceActiveFocus();
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popupMenuDialog;

            onFinished: {
                console.log("CollectionView PopupMenuDialog", commandId);
                popupMenuDialog.root.closeDialog();
                containerBase.commandHandle(commandId);
            }

            function onBackgroundClicked(){
                popupMenuDialog.root.closeDialog();
            }
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
                query = Gql.GqlRequest("mutation", containerBase.removeGqlCommand);
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

                    if (dataModelLocal.ContainsKey(containerBase.removeGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.removeGqlCommand);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})
                    //                    if (dataModelLocal.ContainsKey(containerBase.removeGqlCommand)){
                    //                        dataModelLocal = dataModelLocal.GetData(containerBase.removeGqlCommand);
                    //                        var message = dataModelLocal.GetData("message");
                    //                        modalDialogManager.openDialog(errorDialog, {"message": message});
                    //                    }

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.removeGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.removeGqlCommand);

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

            query = Gql.GqlRequest("mutation", containerBase.renameGqlCommand);

            let itemIds = containerBase.tableData.getSelectedIds();
            inputParams.InsertField("Id", itemIds[0]);

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

                    if (dataModelLocal.ContainsKey(containerBase.renameGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.renameGqlCommand);

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                        //                        if (dataModelLocal.ContainsKey("message")){
                        //                            var message = dataModelLocal.GetData("message");
                        //                            modalDialogManager.openDialog(errorDialog, {"message": message});
                        //                        }
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.renameGqlCommand)) {
                        dataModelLocal = dataModelLocal.GetData(containerBase.renameGqlCommand);

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

            query = Gql.GqlRequest("mutation", containerBase.setDescriptionGqlCommand);

            let itemIds = containerBase.tableData.getSelectedIds();
            inputParams.InsertField("Id", itemIds[0]);

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

                    if (dataModelLocal.ContainsKey(containerBase.setDescriptionGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(containerBase.setDescriptionGqlCommand);

                        //                        if (dataModelLocal.ContainsKey("message")){
                        //                            var message = dataModelLocal.GetData("message");
                        //                            modalDialogManager.openDialog(errorDialog, {"message": message});
                        //                        }

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        Events.sendEvent("SendError", {"Message": message, "ErrorType": type})
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(containerBase.setDescriptionGqlCommand)) {

                        dataModelLocal = dataModelLocal.GetData(containerBase.setDescriptionGqlCommand);

                        var id = dataModelLocal.GetData("Id");
                        var description = dataModelLocal.GetData("Description");

                        containerBase.descriptionSetted(id, description);
                    }
                }
            }
        }
    }
}

