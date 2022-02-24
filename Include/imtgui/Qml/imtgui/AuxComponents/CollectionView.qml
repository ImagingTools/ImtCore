import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionViewContainer;

    height: 100;
    width: 100;

    color: "transparent";

    property TreeItemModel collectionViewModel;

    property var table: tableInternal;

    property int selectedIndex: -1;

    property string gqlModelInfo;
    property string gqlModelItems;
    property string gqlModelRemove;
    property string gqlModelRename;
    property string gqlModelSetDescription;

    property string itemId;
    property string itemName;

    property bool autoRefresh: false;

    signal selectItem(string selectedId, string name);
    signal removedItem(string itemId);
    signal collectionViewRightButtonMouseClicked(Item item, int mouseX, int mouseY);

    onCollectionViewModelChanged: {
        console.log("CollectionView onModelChanged", collectionViewContainer.itemId, collectionViewContainer.itemName);
        collectionViewContainer.refresh();
    }

    function openMessageDialog(nameDialog, message) {

        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = collectionViewContainer;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "Ok";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function getDescriptionBySelectedItem(){
        console.log("CollectionView getDescriptionBySelectedItem");
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");
        console.log("dataModelLocal", dataModelLocal);

        var description;
        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            description = dataModelLocal.GetData("AccountDescription", tableInternal.selectedIndex);
        }
        else{
            description = dataModelLocal.GetData("Description", tableInternal.selectedIndex);
        }

        return description;
    }

    function getNameBySelectedItem(){
        console.log("CollectionView getNameBySelectedItem");
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        var name;
        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            name = dataModelLocal.GetData("AccountName", tableInternal.selectedIndex);
        }
        else{
            name = dataModelLocal.GetData("Name", tableInternal.selectedIndex);
        }
        return name;
    }

    function callSetDescriptionQuery(value){
        console.log("CollectionView callSetDescriptionQuery", value);
        setDescriptionQuery.setDescription(value);
    }

    function callRenameQuery(value){
        console.log("CollectionView callRenameQuery", value);
        renameQuery.rename(value);
    }

    function updateItemAfterRename(newId, newName) {
        console.log("CollectionView updateItemAfterRename", newId, newName);
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        dataModelLocal.SetData("Id", newId, tableInternal.selectedIndex);

        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            dataModelLocal.SetData("AccountName", newName, tableInternal.selectedIndex);
        }
        else{
            dataModelLocal.SetData("Name", newName, tableInternal.selectedIndex);
        }

        collectionViewContainer.collectionViewModel.SetData("data", dataModelLocal);
    }

    function updateItemAfterSetDescription(description) {
        console.log("CollectionView updateItemAfterSetDescription", description);
        var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

        if (collectionViewContainer.gqlModelInfo === "AccountInfo"){
            console.log("set AccountDescription", description);
            dataModelLocal.SetData("AccountDescription", description, tableInternal.selectedIndex);
        }
        else{
            console.log("set Description", description);
            dataModelLocal.SetData("Description", description, tableInternal.selectedIndex);
        }

        collectionViewContainer.collectionViewModel.SetData("data", dataModelLocal);
    }

    function dialogResult(parameters) {
         console.log("CollectionView dialogResult", parameters["status"]);

        if (parameters["status"] === "yes") {

            if (collectionViewContainer.gqlModelRemove !== "") {
                collectionViewContainer.removeSelectedItem();

            }
            collectionViewContainer.refresh();
            tableInternal.selectedIndex = -1;
        }
    }

    function refresh() {
        var isHeaderUpdated = false;
        console.log("CollectionView refresh", collectionViewContainer.gqlModelInfo)
        if (collectionViewContainer.collectionViewModel && collectionViewContainer.collectionViewModel.ContainsKey("headers")){
            var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("headers");
            tableInternal.headers = dataModelLocal;
        }
        else {
            headerInfoModel.updateModel()
            isHeaderUpdated = true
        }

        if (collectionViewContainer.collectionViewModel && collectionViewContainer.collectionViewModel.ContainsKey("data")){
//            console.log("CollectionView refresh ");
            var dataModelLocal = collectionViewContainer.collectionViewModel.GetData("data");

            tableInternal.elements = 0;
            tableInternal.elements = dataModelLocal;

            var selectedIndexLocal = collectionViewContainer.collectionViewModel.GetData("selectedIndex");

            if (selectedIndexLocal) {
                tableInternal.selectedIndex = selectedIndexLocal;
            }

            if (collectionViewContainer.autoRefresh) {
                itemsModel.updateModel();
            }
        }
        else if(!isHeaderUpdated){
            itemsModel.updateModel();
        }
    }

    function menuActivated(menuId) {
        console.log("CollectionView menuActivated", menuId);

        var itemId = tableInternal.getSelectedId();
        var name = tableInternal.getSelectedName();

        if (menuId  === "New"){
            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Edit") {
//            if (itemId !== "" && name !== "") {
//                collectionViewContainer.selectItem(itemId, name);
//            }

            collectionViewContainer.selectItem(itemId, name);
        }
        else if (menuId  === "Remove") {
            console.log("CollectionView try remove element with id", itemId);
            if (itemId !== "") {
                var source = "AuxComponents/MessageDialog.qml";
                var parameters = {};
                parameters["message"] = "Remove selected file from the database ?";
                parameters["nameDialog"] = "RemoveDialog";
                parameters["resultItem"] = collectionViewContainer;

                thubnailDecoratorContainer.openDialog(source, parameters);
            }
        }
    }

    function removeSelectedItem() {
        console.log("CollectionView removeSelectedItem", collectionViewContainer.itemId);
        collectionViewContainer.itemId = tableInternal.getSelectedId();
        removeModel.updateModel();
    }

    AuxTable {
        id: tableInternal;

        anchors.fill: parent;

        onSelectItem: {
            console.log("CollectionView AuxTable onSelectItem", selectedId, name);
            collectionViewContainer.selectItem(selectedId, name);
        }

        onRightButtonMouseClicked: {
            console.log("CollectionView AuxTable onRightButtonMouseClicked");
            collectionViewContainer.collectionViewRightButtonMouseClicked(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            console.log(" CollectionView AuxTable onSelectedIndexChanged", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
//            if (tableInternal.selectedIndex != -1) {
//                //collectionViewContainer.selectedIndex = tableInternal.selectedIndex;
//                collectionViewContainer.collectionViewModel.SetData("selectedIndex", tableInternal.selectedIndex);
//            }
            collectionViewContainer.collectionViewModel.SetData("selectedIndex", tableInternal.selectedIndex);
            collectionViewContainer.selectedIndex = tableInternal.selectedIndex;
            //console.log("collectionViewContainer.selectedIndex 2", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
        }
    }

    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "headerInfoModel update", collectionViewContainer.gqlModelInfo);

            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelInfo);
            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("headerInfoModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, headerInfoModel);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (headerInfoModel.ContainsKey("errors")){

                    dataModelLocal = headerInfoModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo);

                        if (dataModelLocal.ContainsKey("message")){

                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("ErrorDialog", message);
                        }

                    }

                    return;
                }

                if (headerInfoModel.ContainsKey("data")){
                    dataModelLocal = headerInfoModel.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo);

                        if(dataModelLocal.ContainsKey("headers")){
                            tableInternal.headers = dataModelLocal.GetData("headers")
                            collectionViewContainer.collectionViewModel.SetExternTreeModel('headers',tableInternal.headers)
                            itemsModel.updateModel();
                        }
                    }
                }

//                if(dataModelLocal && dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
//                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo)
//                    if(dataModelLocal.ContainsKey("headers")){
//                        tableInternal.headers = dataModelLocal.GetData("headers")
//                        collectionViewContainer.collectionViewModel.SetExternTreeModel('headers',tableInternal.headers)

//                        itemsModel.updateModel();
//                    }
//                    else if(packageInfoModel.ContainsKey("errors")){
//                        var errorsModelLocal = packageInfoModel.GetData("errors");
//                        if(errorsModelLocal !== null && errorsModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
//                            console.log("message", errorsModelLocal.GetData(collectionViewContainer.gqlModelInfo).GetData("message"));
//                        }
//                    }
//                }
            }
        }
    }

    Timer {
        id: elementsTimer;
        property var elementsmodel;
        interval: 10;
        onTriggered: {
            tableInternal.elements = elementsTimer.elementsmodel;
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "collectionViewContainer updateModel", collectionViewContainer.gqlModelItems, collectionViewContainer.itemId);
            var query = Gql.GqlRequest("query", collectionViewContainer.gqlModelItems);

            if(collectionViewContainer.itemId != ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", collectionViewContainer.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("items");            

            queryFields.InsertField("Id");
            for(var i = 0; i < tableInternal.headers.GetItemsCount(); i++){
                queryFields.InsertField(tableInternal.headers.GetData("Id",i));
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("collectionViewContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (itemsModel.ContainsKey("errors")){
                    dataModelLocal = itemsModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){

                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("ErrorDialog", message);
                        }
                    }

                    return;
                }

                if (itemsModel.ContainsKey("data")){
                    dataModelLocal = itemsModel.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){

                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);

                        if (dataModelLocal.ContainsKey("items")){
                            tableInternal.elements = dataModelLocal.GetData("items");
                            collectionViewContainer.collectionViewModel.SetExternTreeModel('data', tableInternal.elements);
                        }
                    }
                }

//                var dataModelLocal = this.GetData("data");
//                if(dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){
//                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);
//                    if(dataModelLocal.ContainsKey("items")){


//                        if (tableInternal.elements){
//                            console.log("Old elements:");
//                            for (var i = 0; i < tableInternal.elements.GetItemsCount(); i++){
//                                var id = tableInternal.elements.GetData("Id", i);

//                                console.log("\tElement Id", id);
//                            }
//                        }

//                        tableInternal.elements = dataModelLocal.GetData("items");

//                        console.log("New elements:");
//                        for (var i = 0; i < tableInternal.elements.GetItemsCount(); i++){
//                            var id = tableInternal.elements.GetData("Id", i);

//                            console.log("\tElement Id", id);
//                        }

//                        collectionViewContainer.collectionViewModel.SetExternTreeModel('data', tableInternal.elements);
//                    }
//                    else if(itemsModel.ContainsKey("errors")){
//                        var errorsModel = itemsModel.GetData("errors");
//                        if(errorsModel.ContainsKey(collectionViewContainer.gqlModelItems)){
//                            console.log("message", errorsModel.GetData(collectionViewContainer.gqlModelItems).GetData("message"));
//                        }
//                    }
//                }
            }
        }
    }

    GqlModel {
        id: removeModel;

        function updateModel() {
            console.log( "updateModel removeModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(collectionViewContainer.itemId != ""){
                query = Gql.GqlRequest("query", collectionViewContainer.gqlModelRemove);
                inputParams.InsertField("Id");
                //inputParams.InsertFieldArgument("Id", collectionViewContainer.itemId);
                inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());
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

                    if (dataModelLocal.ContainsKey("removedNotification")){

                        dataModelLocal = dataModelLocal.GetData("removedNotification");

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("ErrorDialog", message);
                        }
                    }

                    return;
                }

                if (removeModel.ContainsKey("data")){
                    dataModelLocal = removeModel.GetData("data");

                    if (dataModelLocal.ContainsKey("removedNotification")){

                        dataModelLocal = dataModelLocal.GetData("removedNotification");

                        if (dataModelLocal.ContainsKey("Id")){
                            var itemId = dataModelLocal.GetData("Id");
                            collectionViewContainer.removedItem(itemId)
                        }
                    }
                }

//                var dataModelLocal = this.GetData("data");
//                console.log("dataModelLocal:", dataModelLocal);
//                if(dataModelLocal.ContainsKey("removedNotification")){
//                    dataModelLocal = dataModelLocal.GetData("removedNotification");

//                    if(dataModelLocal.ContainsKey("Id")){
//                        var itemId = dataModelLocal.GetData("Id");
//                        console.log("Remove item by id = ", itemId);
//                        collectionViewContainer.removedItem(itemId)
//                    }
//                    else if(removeModel.ContainsKey("errors")){
//                        var errorsModel = removeModel.GetData("errors");
//                        if(errorsModel.ContainsKey(collectionViewContainer.gqlModelItems)){
//                            console.log("message", errorsModel.GetData(collectionViewContainer.gqlModelItems).GetData("message"));
//                        }
//                    }
//                }
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

            query = Gql.GqlRequest("query", collectionViewContainer.gqlModelRename);
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());

            inputParams.InsertField("NewName");
            inputParams.InsertFieldArgument("NewName", newName);

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

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRename)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("ErrorDialog", message);
                        }
                    }

                    return;
                }

                if (renameQuery.ContainsKey("data")){
                    dataModelLocal = renameQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelRename)) {

                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelRename);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var newId = dataModelLocal.GetData("NewId");

                            if (collectionViewContainer.gqlModelInfo === "ProductCollectionInfo"){
                                newId = tableInternal.getSelectedId();
                            }

                            var newName = dataModelLocal.GetData("NewName");

                            //collectionViewContainer.updateItemAfterRename(newId, newName);
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: setDescriptionQuery;

        function setDescription(description){
            console.log( "CollectionView renameQuery rename");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            query = Gql.GqlRequest("query", collectionViewContainer.gqlModelSetDescription);
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", tableInternal.getSelectedId());

            inputParams.InsertField("Description");
            inputParams.InsertFieldArgument("Description", description);

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

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelSetDescription)){
                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("message")){
                            var message = dataModelLocal.GetData("message");
                            collectionViewContainer.openMessageDialog("ErrorDialog", message);
                        }
                    }

                    return;
                }

                if (setDescriptionQuery.ContainsKey("data")){
                    dataModelLocal = setDescriptionQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(collectionViewContainer.gqlModelSetDescription)) {

                        dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelSetDescription);

                        if (dataModelLocal.ContainsKey("item")) {
                            dataModelLocal = dataModelLocal.GetData("item");

                            var description = dataModelLocal.GetData("Description");

                            collectionViewContainer.updateItemAfterSetDescription(description);
                        }
                    }
                }
            }
        }
    }

}
