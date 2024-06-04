import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: container;

    property TreeItemModel collectionModel: TreeItemModel {};

    property string commandId;
    property string subscriptionCommandId;

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

    function updateModel(){
        if (collectionModel && collectionModel.GetItemsCount() === 0){
            container.itemsInfoModel.updateModel(container.fields);
            return;
        }

        if (!completed || hasRemoteChanges){
            if (container.commandId == ""){
                console.log( "ERROR: Collection data provider Command-ID is empty!");

                return;
            }

            container.itemsInfoModel.updateModel(container.fields);
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

    function getAdditionalInputParams(){
        return {};
    }

    property GqlModel itemsInfoModel: GqlModel {
        function updateModel(fields) {
            var query = Gql.GqlRequest("query", container.commandId);

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", container.offset);
            viewParams.InsertField("Count", container.count);

            var jsonString = container.filterModel.ToJson();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

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
                    if (dataModelLocal.ContainsKey(container.commandId)){
                        dataModelLocal = dataModelLocal.GetData(container.commandId);
                        if (dataModelLocal.ContainsKey("items")){
                            container.collectionModel = dataModelLocal.GetData("items");
                        }
                        else{
                            console.error("Unable to parsing data: tag 'items' not found!");
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");
                            }
                        }
                        else{
                            console.warn("Unable to get notification info for collection model.");
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
        console.log("updateSubscription", container.subscriptionCommandId);

        if (container.subscriptionCommandId === ""){
            console.error("Unable to update subscription, command-ID is empty!");
            return;
        }

        let subscriptionRequestId = container.subscriptionCommandId;
        var query = Gql.GqlRequest("subscription", subscriptionRequestId);
        var queryFields = Gql.GqlObject("notification");
        queryFields.InsertField("Id");
        query.AddField(queryFields);

        Events.sendEvent("RegisterSubscription", {"Query": query, "Client": subscriptionClient});
    }

    SubscriptionClient {
        id: subscriptionClient;

        property bool ok: container.subscriptionCommandId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (ok){
                container.updateSubscription();
            }
        }

        onStateChanged: {
            if (state === "Ready"){
                console.log("SubscriptionClient", subscriptionClient.ToJson());
                container.hasRemoteChanges = true;
            }
        }
    }
}
