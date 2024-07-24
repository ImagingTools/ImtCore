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
        if (collectionModel && collectionModel.getItemsCount() === 0){
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
        collectionModel.clear();
    }

    Component.onCompleted: {
        let sortModel = filterModel.getTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.addTreeModel("Sort")
        }

        sortModel.setData("SortOrder", orderType);
        sortModel.setData("HeaderId", sortByField);
    }

    onOrderTypeChanged: {
        let sortModel = filterModel.getTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.addTreeModel("Sort")
        }

        sortModel.setData("SortOrder", orderType);
    }

    onSortByFieldChanged: {
        let sortModel = filterModel.getTreeItemModel("Sort");
        if (!sortModel){
            sortModel = filterModel.addTreeModel("Sort")
        }

        sortModel.setData("HeaderId", sortByField);
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
        for (let i = 0; i < container.collectionModel.getItemsCount(); i++){
            let objectCollectionId = container.collectionModel.getData("Id", i);
            if (objectCollectionId === objectId){
                if (container.collectionModel.containsKey(value, i)){
                    return container.collectionModel.getData(value, i);
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

            var jsonString = container.filterModel.toJson();
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

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
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;
                if (container.itemsInfoModel.containsKey("errors")){
                    return;
                }

                if (container.itemsInfoModel.containsKey("data")){
                    dataModelLocal = container.itemsInfoModel.getData("data");
                    if (dataModelLocal.containsKey(container.commandId)){
                        dataModelLocal = dataModelLocal.getData(container.commandId);
                        if (dataModelLocal.containsKey("items")){
                            container.collectionModel = dataModelLocal.getData("items");
                        }
                        else{
                            console.error("Unable to parsing data: tag 'items' not found!");
                        }

                        if (dataModelLocal.containsKey("notification")){
                            dataModelLocal = dataModelLocal.getData("notification");
                            if (dataModelLocal.containsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.getData("PagesCount");
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
                console.log("SubscriptionClient", subscriptionClient.toJson());
                container.hasRemoteChanges = true;
            }
        }
    }
}
