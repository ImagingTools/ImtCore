import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
	id: container;

	property TreeItemModel collectionModel: TreeItemModel {};
	property TreeItemModel notificationModel: TreeItemModel {};

	property string commandId;
	property string subscriptionCommandId;

	// Property indicating whether the model is already requested from server
	property bool completed: false;

	// Fields to get from server
	property var fields: [];
	// Fields to which text filter will be applied
	property var textFilteringInfoIds: []

	property string orderType: "ASC";
	property string sortByField;

	property alias state: container.itemsInfoModel.state;

	property int offset: 0;
	property int count: -1;

	property CollectionFilter filter: CollectionFilter {}

	signal modelUpdated(var data);
	signal failed(string message);

	Component.onCompleted: {
		filter.setSortingInfo(sortByField, orderType)
	}

	Component.onDestruction: {
		if (collectionModel){
			collectionModel.destroy();
		}

		if (filter){
			filter.destroy();
		}
	}

	onTextFilteringInfoIdsChanged: {
		filter.textFilteringInfoIds = textFilteringInfoIds
	}

	onOrderTypeChanged: {
		filter.setSortingInfo(sortByField, orderType)
	}

	onSortByFieldChanged: {
		filter.setSortingInfo(sortByField, orderType)
	}

	function updateModel(offset){
		if(offset !== undefined && offset !== null && offset >= 0){
			container.offset = offset;
		}

		container.itemsInfoModel.send();
	}

	function clearModel(){
		collectionModel.clear();
	}

	function getData(objectId, value){
		for (let i = 0; i < container.collectionModel.getItemsCount(); i++){
			let objectCollectionId = container.collectionModel.getData("id", i);
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
			viewParams.InsertField("offset", container.offset);
			viewParams.InsertField("count", container.count);
			viewParams.InsertField("filterModel", container.filter);

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
			container.notificationModel = data.getData("notification");

			container.modelUpdated(data);
		}

		function onError(message, type){
			console.warn(message);
			container.failed(message)
		}
	}

	property SubscriptionClient subscriptionClient: SubscriptionClient {
		gqlCommandId: container.subscriptionCommandId;

		onMessageReceived: {
			console.log("CollectionDataProvider.qml onMessageReceived", container.commandId);

			container.clearModel();
			container.itemsInfoModel.send();
		}

		function getHeaders(){
			return container.getHeaders()
		}
	}
}
