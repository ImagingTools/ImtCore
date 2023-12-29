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
    property TreeViewGql treeView

    property Item collectionStructureBase: null;

    property Item documentManagerPtr: null;

    property CommandsProvider commandsProvider: null;

    /**
        The property for tracking changes to the selected item
    */
    // property int selectedIndex: containerBase.tableData ? containerBase.tableData.selectedIndex: -1;

    property string commandId;

    property string descriptionFieldId: "Description";

    property var contextMenuModel: null;

    property bool autoUpdateAfterChanges: false;

    property alias removeGqlModel: removeModel;
    property alias renameGqlModel: renameQuery;
    property alias setDescriptionGqlModel: setDescriptionQuery;

    property alias setDescriptionDialogComp : setDescriptionDialog;

    signal selectionChanged();
    function test(){
        console.log("test")
    }

    property string removeDialogTitle: qsTr("Deleting a selected element");
    property string removeMessage: qsTr("Remove selected item from the collection ?");

    /**
        Requests IDs for GQL models
    */
    property string removeNodeGqlCommand;
    property string removeObjectGqlCommand;
    property string renameGqlCommand;
    property string newNodeGqlCommand;
    property string setDescriptionGqlCommand;

    signal commandActivated(string commandId);
    signal renamed(string id, string newName);
    signal descriptionSetted(string id, string description);
    signal removed(string id);

    onDescriptionSetted: {
        if (autoUpdateAfterChanges){
            containerBase.collectionStructureBase.updateGui();
        }
    }

    onSelectionChanged: {
        console.log("CollectionStructureCommandsDelegateBase onSelectionChanged", containerBase.commandsProvider);
        let index = containerBase.treeView.selectedIndex;

        let isEnabled = (index >= 0)
        if(containerBase.commandsProvider){
            containerBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
            containerBase.commandsProvider.setCommandIsEnabled("Edit", isEnabled);
        }
    }

    onTreeViewChanged: {
        if (containerBase.treeView){
            if (containerBase.treeView.rightButtonMouseClicked){
                containerBase.treeView.rightButtonMouseClicked.connect(openPopupMenu);
            }
            containerBase.treeView.selectionChanged.connect(containerBase.selectionChanged);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(containerBase.commandId + "CommandActivated", containerBase.commandHandle);
    }

    onCommandIdChanged: {
        console.log("CollectionStructureCommands oncommandIdChanged", containerBase.commandId);
        Events.subscribeEvent(containerBase.commandId + "CommandActivated", containerBase.commandHandle);

        if (containerBase.commandId !== ""){
            removeNodeGqlCommand = containerBase.commandId + "RemoveNode";
            removeObjectGqlCommand = containerBase.commandId + "RemoveObject";
            renameGqlCommand = containerBase.commandId + "Rename";
            newNodeGqlCommand = containerBase.commandId + "InsertNewNode";
            setDescriptionGqlCommand = containerBase.commandId + "SetDescription";
        }
    }

    onRenamed: {
//        containerBase.documentManagerPtr.setDocumentTitle({"Id": id, "Title": newName});
        if (autoUpdateAfterChanges){
            containerBase.collectionStructureBase.updateGui();
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
            containerBase.collectionStructureBase.updateGui();
        }
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
        console.log("CollectionStructure onEdit");
        let elementsModel = containerBase.treeView;
        if (!elementsModel){
            return;
        }

        let index = containerBase.treeView.selectedIndex
        if (index >= 0){
            let itemId = elementsModel.getData("Id", index);
            let itemName = elementsModel.getData("Name", index);
            containerBase.collectionStructureBase.selectItem(itemId, itemName);
        }
    }

    function onRemove(){
        if (containerBase.commandId === ""){
            console.error("Remove item was failed!")
            return;
        }

        modalDialogManager.openDialog(removeDialog, {});
    }

    function onNewDocument(){
        containerBase.collectionStructureBase.selectItem("", "");
    }

    function onNewNode(){
        if (containerBase.commandId === "" || newNodeGqlCommand === ""){
            console.error("Insert new node was failed!")
            return;
        }

        let index = -1
        if (containerBase.treeView.selectedIndex){
            index = containerBase.treeView.findParentIndex(containerBase.treeView.selectedIndex)
        }
        // let selectedName = containerBase.treeView.GetData("Name", index);
        modalDialogManager.openDialog(newNodeDialog, {"message": qsTr("Please enter the name of the new node"), "inputValue": ""});
    }

    function onRename(){
        if (containerBase.commandId === "" || renameGqlCommand === ""){
            console.error("Rename item was failed!")
            return;
        }

        let index = containerBase.treeView.selectedIndex
        if (index >= 0){            let selectedName = containerBase.tableData.elements.GetData("Name", indexes[0]);
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
        console.log("CollectionStructure commandHandle", commandId);
        if (containerBase.commandsProvider == null){
            return;
        }

        let commandIsEnabled = containerBase.commandsProvider.commandIsEnabled(commandId);
        commandIsEnabled = true
        if (commandIsEnabled){
            if (commandId === "New"){
                containerBase.onNewDocument();
            }
            else if (commandId === "Remove"){
                containerBase.onRemove();
            }
            else if (commandId === "Edit"){
                containerBase.onEdit();
            }
            else if (commandId === "NewNode"){
                containerBase.onNewNode();
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
                let uuid = containerBase.collectionStructureBase.uuid;
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
                    let index = containerBase.treeView.selectedIndex;
                    let id = containerBase.treeView.getData("Id", index);
                    let typeId = containerBase.treeView.getData("TypeId", index);
                    if (id !== ""){
                        removeModel.updateModel(id, typeId);
                    }
                }

                containerBase.treeView.forceActiveFocus();
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

    property Component newNodeDialog: Component {
        InputDialog {
            title: qsTr("New node");
            onFinished: {
                if (buttonId == "Ok"){
                    newNodeQuery.newNode(inputValue);
                }

                containerBase.treeView.forceActiveFocus();
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
                console.log("CollectionStructure PopupMenuDialog", commandId);
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

        function updateModel(itemId, typeId) {
            console.log( "updateModel removeModel", itemId, typeId);

            let query;
            let queryFields;
            let inputParams = Gql.GqlObject("input");

            if(itemId !== ""){
                if (typeId == "Node"){
                    query = Gql.GqlRequest("mutation", containerBase.removeNodeGqlCommand);
                }
                else{
                    query = Gql.GqlRequest("mutation", containerBase.removeObjectGqlCommand);
                }

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

    UuidGenerator {
        id: uuidGenerator;
    }

    GqlModel {
        id: newNodeQuery;

        function newNode(newName) {
            console.log( "CollectionStructure newNodeQuery");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("mutation", containerBase.newNodeGqlCommand);

            let uuid = uuidGenerator.generateUUID();
            inputParams.InsertField("Id", uuid);

            inputParams.InsertField("NewName", newName);

            let parentIndex = -1
            let selectedIndex = containerBase.treeView.selectedIndex
            if (containerBase.treeView.selectedIndex > -1){
                if (containerBase.treeView.getData("TypeId", selectedIndex) === "Node"){
                    parentIndex = selectedIndex
                }
                else{
                    parentIndex = containerBase.treeView.findParentIndex(containerBase.treeView.selectedIndex)
                }
            }

            let parentNodeId = containerBase.treeView.getData("Id", parentIndex)
            if (parentNodeId === undefined){
                parentNodeId = ""
            }
            inputParams.InsertField("ParentNodeId", parentNodeId);

            query.AddParam(inputParams);

            queryFields = Gql.GqlObject("successful");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("CollectionStructure newNodeQuery", gqlData);

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
        id: renameQuery;

        function rename(newName) {
            console.log( "CollectionStructure renameQuery rename");

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
            console.log("CollectionStructure renameQuery rename", gqlData);

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
            console.log( "CollectionStructure setDescriptionQuery");

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
            console.log("CollectionStructure setDescription query ", gqlData);
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

