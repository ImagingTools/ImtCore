import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: container;

    property TreeItemModel collectionModel: TreeItemModel {};

    property string commandId;

    // Property indicating whether the model is ready for use
    property bool completed: false;

    // Fields to get from server
    property var fields: [];

    property string orderType: "ASC";
    property string sortByField;

    property alias stateModel: container.itemsInfoModel.state;

    property int offset: 0;
    property int count: -1;

    property bool hasRemoteChanges: false;

    property TreeItemModel filterModel: TreeItemModel {};

    signal modelUpdated();
    signal failed();

    function updateModel(inputParams){
        if (!inputParams){
            inputParams = {}
        }

        if (collectionModel && collectionModel.GetItemsCount() === 0){
            container.itemsInfoModel.updateModel(inputParams, container.fields);
            return;
        }

        if (!completed || hasRemoteChanges){
            if (container.commandId == ""){
                console.log( "ERROR: Collection data provider Command-ID is empty!");

                return;
            }

            container.itemsInfoModel.updateModel(inputParams, container.fields);
        }

        container.updateSubscription();
    }

    function clearModel(){
        collectionModel.Clear();
    }

    Component.onCompleted: {
        let sortModel = filterModel.GetTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.AddTreeModel("Sort")
        }

        sortModel.SetData("SortOrder", orderType);
        sortModel.SetData("HeaderId", sortByField);
    }

    onOrderTypeChanged: {
        let sortModel = filterModel.GetTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.AddTreeModel("Sort")
        }

        sortModel.SetData("SortOrder", orderType);
    }

    onSortByFieldChanged: {
        let sortModel = filterModel.GetTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.AddTreeModel("Sort")
        }

        sortModel.SetData("HeaderId", sortByField);
    }

    onHasRemoteChangesChanged: {
        if (hasRemoteChanges){
            updateModel();
        }
    }

    function setSortingHeader(headerId){
        container.sortByField = headerId;
    }

    function setOrderType(orderType){
        container.orderType = orderType;
    }

    function onModelUpdated(){
        if (!container.completed){
            container.completed = true;
        }
    }

    function getData(objectId, value){
        for (let i = 0; i < container.collectionModel.GetItemsCount(); i++){
            let objectCollectionId = container.collectionModel.GetData("Id", i);
            if (objectCollectionId === objectId){
                if (container.collectionModel.ContainsKey(value, i)){
                    return container.collectionModel.GetData(value, i);
                }

                break;
            }
        }

        return null;
    }

    property GqlModel itemsInfoModel: GqlModel {
        function updateModel(externInputParams, fields) {
            var query = Gql.GqlRequest("query", container.commandId + "List");

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", container.offset);
            viewParams.InsertField("Count", container.count);

            var jsonString = container.filterModel.ToJson();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }
            inputParams.InsertFieldObject(viewParams);

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            for (let key of fields){
                queryFields.InsertField(key);
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            container.completed = false;
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;
                if (container.itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (container.itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = container.itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(container.commandId + "List")){
                        dataModelLocal = dataModelLocal.GetData(container.commandId + "List");
                        if (dataModelLocal.ContainsKey("items")){
                            container.collectionModel = dataModelLocal.GetData("items");
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");
                            }
                        }

                        container.modelUpdated();

                        container.completed = true;
                        container.hasRemoteChanges = false;
                    }
                }
            }
            else if (this.state === "Error"){
                container.failed();

                container.completed = true;
            }
        }
    }

    function updateSubscription(){
        if (container.commandId === ""){
            console.error("Unable to update subscription, command-ID is empty!");
            return;
        }

        let subscriptionRequestId = "On" + container.commandId + "CollectionChanged"
        var query = Gql.GqlRequest("subscription", subscriptionRequestId);
        var queryFields = Gql.GqlObject("notification");
        queryFields.InsertField("Id");
        query.AddField(queryFields);

        Events.sendEvent("RegisterSubscription", {"Query": query, "Client": subscriptionClient});
    }

    SubscriptionClient {
        id: subscriptionClient;

        property bool ok: container.commandId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (ok){
                container.updateSubscription();
            }
        }

        onStateChanged: {
            if (state === "Ready"){
                container.hasRemoteChanges = true;
            }
        }
    }
}
