import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionViewContainer;
    height: 100;
    width: 100;
    color: "transparent";
    property TreeItemModel model;
    property var table: tableInternal;
    property int selectedIndex: -1;
    property string gqlModelInfo;
    property string gqlModelItems;
    property string gqlModelRemove;
    property string itemId;
    property string itemName;
    property bool autoRefresh: false;
   // property var parametrs: new {};
    signal selectItem(string selectedId, string name);

    signal removedItem(string itemId);

    signal collectionViewRightButtonMouseClicked(Item item, int mouseX, int mouseY);

    function dialogResult(parameters) {
         console.log("CollectionView dialogResult", parameters["status"]);

        if (parameters["status"] === "yes") {

            if (gqlModelRemove !== "") {
                collectionViewContainer.removeSelectedItem();
            }
            collectionViewContainer.refresh();
        }
    }

    function refresh() {
        var isHeaderUpdated = false;
        console.log("CollectionView refresh()", collectionViewContainer.gqlModelInfo)
        if (collectionViewContainer.model && collectionViewContainer.model.ContainsKey("headers")){
            var dataModelLocal = collectionViewContainer.model.GetData("headers");
            tableInternal.headers = dataModelLocal;
        }
        else {
            headerInfoModel.updateModel()
            isHeaderUpdated = true
        }

        if (collectionViewContainer.model && collectionViewContainer.model.ContainsKey("data")){
            var dataModelLocal = collectionViewContainer.model.GetData("data");
            tableInternal.elements = 0;
            tableInternal.elements = dataModelLocal;

            var selectedIndexLocal = collectionViewContainer.model.GetData("selectedIndex");

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

    onModelChanged: {
        console.log("CollectionView onModelChanged",collectionViewContainer.itemId, collectionViewContainer.itemName);
        collectionViewContainer.refresh();
    }

    function menuActivated(menuId) {
        console.log("CollectionView menuActivated", menuId);

        var itemId = tableInternal.getSelectedId();
        var name = tableInternal.getSelectedName();

        if (menuId  === "New"){
            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Edit") {
            if (itemId !== "" && name !== "") {
                collectionViewContainer.selectItem(itemId, name);
            }
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
            if (tableInternal.selectedIndex != -1) {
                //collectionViewContainer.selectedIndex = tableInternal.selectedIndex;
                collectionViewContainer.model.SetData("selectedIndex", tableInternal.selectedIndex);
            }
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
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal && dataModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelInfo)
                    if(dataModelLocal.ContainsKey("headers")){
                        tableInternal.headers = dataModelLocal.GetData("headers")
                        collectionViewContainer.model.SetExternTreeModel('headers',tableInternal.headers)

                        itemsModel.updateModel();
                    }
                    else if(packageInfoModel.ContainsKey("errors")){
                        var errorsModelLocal = packageInfoModel.GetData("errors");
                        if(errorsModelLocal !== null && errorsModelLocal.ContainsKey(collectionViewContainer.gqlModelInfo)){
                            console.log("message", errorsModelLocal.GetData(collectionViewContainer.gqlModelInfo).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    Timer {
        id: elementsTimer;
        property var model;
        interval: 10;
        onTriggered: {
            tableInternal.elements = elementsTimer.model;
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
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey(collectionViewContainer.gqlModelItems)){
                    dataModelLocal = dataModelLocal.GetData(collectionViewContainer.gqlModelItems);
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        tableInternal.elements = dataModelLocal.GetData("items");
//                        elementsTimer.model = dataModelLocal.GetData("items");
//                        elementsTimer.start();

//                        if (!collectionViewContainer.autoRefresh) {
//                            console.log("CollectionView itemsModel onStateChanged update data");
//                            collectionViewContainer.model.SetExternTreeModel('data', tableInternal.elements);
//                        }

                        collectionViewContainer.model.SetExternTreeModel('data', tableInternal.elements);
//                        tableInternal._context._processActions()

//                        collectionViewContainer.itemId = "";

                    }
                    else if(itemsModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(collectionViewContainer.gqlModelItems)){
                            console.log("message", errorsModel.GetData(collectionViewContainer.gqlModelItems).GetData("message"));
                        }
                    }
                }
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

                var dataModelLocal = this.GetData("data");
                console.log("dataModelLocal:", dataModelLocal);
                if(dataModelLocal.ContainsKey("removedNotification")){
                    dataModelLocal = dataModelLocal.GetData("removedNotification");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("Id")){
                        var itemId = dataModelLocal.GetData("Id");
                        console.log("Remove item by id = ", itemId);
                        collectionViewContainer.removedItem(itemId)
                    }
                    else if(removeModel.ContainsKey("errors")){
                        var errorsModel = removeModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(collectionViewContainer.gqlModelItems)){
                            console.log("message", errorsModel.GetData(collectionViewContainer.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
