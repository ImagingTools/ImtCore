import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: container;

    property TreeItemModel collectionModel: TreeItemModel {};

    property string commandId;
    property string subscriptionCommandId;

    // Property indicating whether the model is already requested from server
    property bool completed: false;

    // Fields to get from server
    property var fields: [];

    property string orderType: "ASC";
    property string sortByField;

    property alias stateModel: container.itemsInfoModel.state;

    property int offset: 0;
    property int count: -1;

    property var filter: CollectionFilter {}

    signal modelUpdated();
    signal failed();

    function updateModel(){
        if (completed){
            console.warn("Collection data already loaded")
            //return;
        }

        container.itemsInfoModel.send();
    }

    function clearModel(){
        collectionModel.clear();
        completed = false;
    }

    Component.onCompleted: {
        filter.setSortingOrder(orderType);
        filter.setSortingInfoId(sortByField);
    }

    onOrderTypeChanged: {
        filter.setSortingOrder(orderType);
    }

    onSortByFieldChanged: {
        filter.setSortingInfoId(sortByField);
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

    function getHeaders(){
        return {};
    }

    property GqlRequestSender itemsInfoModel: GqlRequestSender {
        requestType: 0; // Query
        gqlCommandId: container.commandId;

        function createQueryParams(query){
            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", container.offset);
            viewParams.InsertField("Count", container.count);

            if (container.filter.filterModel.toGraphQL){
                viewParams.InsertField("ComplexFilterModel", container.filter.filterModel);
            }
            else{
                viewParams.InsertField("FilterModel", container.filter.filterModel.toJson());
            }

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            for (let key of container.fields){
                queryFields.InsertField(key);
            }
            query.AddField(queryFields);
        }

        function getHeaders(){
            return container.getHeaders();
        }

        function onResult(data){
            container.collectionModel = data.getData("items");
            container.completed = true;
            container.modelUpdated();
        }

        function onError(message, type){
            console.warn(message);
        }
    }

    function updateSubscription(){
        if (container.subscriptionCommandId === ""){
            console.error("Unable to update subscription, command-ID is empty!");
            return;
        }

        var query = Gql.GqlRequest("subscription", container.subscriptionCommandId);
        var queryFields = Gql.GqlObject("notification");
        queryFields.InsertField("Id");
        query.AddField(queryFields);

        Events.sendEvent("RegisterSubscription", {"Query": query, "Client": subscriptionClient, "Headers": container.getHeaders()});
    }

    property SubscriptionClient subscriptionClient: SubscriptionClient {
        property bool ok: container.subscriptionCommandId !== "" && subscriptionId !== "";
        onOkChanged: {
            if (ok){
                container.updateSubscription();
            }
        }

        onStateChanged: {
            if (state === "Ready"){
                container.itemsInfoModel.send();
            }
        }

        function getHeaders(){
            return container.getHeaders()
        }
    }
}
