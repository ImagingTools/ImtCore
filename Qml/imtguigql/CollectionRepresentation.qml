import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
	id: root;
	
	property string collectionId: "";
	
	property string gqlGetListCommandId: collectionId + "List";
	property string gqlGetHeadersCommandId: collectionId + "Info";
	property string gqlRemoveCommandId: collectionId + "Remove";
	property string gqlRenameCommandId: collectionId + "Rename";
	property string gqlSetDescriptionCommandId: collectionId + "SetDescription";
	property string gqlImportCommandId: collectionId + "Import";
	property string gqlExportCommandId: collectionId + "Export";
	
	property TreeItemModel elementsModel: TreeItemModel {}
	property TreeItemModel headersModel: TreeItemModel {}
	property TreeItemModel objectEditorInfoModel: TreeItemModel {}
	property TreeItemModel notificationModel: TreeItemModel {}
	property TreeItemModel filterableHeadersModel: TreeItemModel {}
	
	property var additionalFieldIds: []

	signal removed(string objectId);
	signal renamed(string objectId, string newName);
	signal imported(string objectId);
	signal exported(string fileName, string data);
	signal descriptionSetted(string objectId, string description);
	
	property alias removeGqlModel: removeModel;
	property alias headersGqlModel: headersGqlModel;

	signal beginUpdate();
	signal endUpdate();

	function importObject(typeId, name, description, b64encoded, mimeType, additionalParamsObj){
		let params = {}
		params["typeId"] = typeId;
		params["name"] = name;
		params["description"] = description;
		params["fileData"] = b64encoded;
		params["mimeType"] = mimeType;
		
		if (additionalParamsObj){
			for (let key in additionalParamsObj){
				params[key] = additionalParamsObj[key]
			}
		}
		
		importObjectRequestSender.send(params)
	}
	
	function exportObject(objectId, mimeType, additionalParamsObj){
		let params = {}
		params["id"] = objectId
		params["mimeType"] = mimeType
		
		if (additionalParamsObj){
			for (let key in additionalParamsObj){
				params[key] = additionalParamsObj[key]
			}
		}
		
		exportObjectRequestSender.send(params);
	}
	
	onCollectionIdChanged: {
		console.log("onCollectionIdChanged", collectionId);
		
		root.gqlGetHeadersCommandId = collectionId + "Info";
		
		updateModel();
	}
	
	onEndUpdate: {
		internal.elementsUpdatingBlock = false;
	}
	
	QtObject {
		id: internal;

		property bool elementsUpdatingBlock: false;
		property bool headersUpdatingBlock: false;
	}

	function addAdditionalInputParams(inputParams){
		// override for derived implementations
	}
	
	function updateModel(){
		updateHeaders();
	}
	
	function updateHeaders(){
		if (internal.headersUpdatingBlock){
			return;
		}
		
		if (root.collectionId === ""){
			console.error("Unable to update headers 'collectionId' is empty!");
			return;
		}
		
		headersGqlModel.gqlCommandId = root.gqlGetHeadersCommandId;
		headersGqlModel.send();
	}
	
	function updateElements(count, offset, filterModel){
		if (internal.elementsUpdatingBlock){
			return;
		}
		
		if (root.collectionId === ""){
			console.error("Unable to update elements 'collectionId' is empty!");
			return;
		}
		
		root.beginUpdate();
		elementsGqlModel.gqlCommandId = gqlGetListCommandId;
		elementsGqlModel.send({"count":count, "offset":offset,"filterModel":filterModel})
	}
	
	function clearElements(){
		if (elementsModel){
			elementsModel.clear()
			elementsModel = null
		}
		
		if (notificationModel){
			notificationModel.clear()
			notificationModel = null
		}
	}
	
	function getElementsRepresentation(){
		return root.elementsModel;
	}
	
	function getHeadersRepresentation(){
		return root.headersModel;
	}
	
	function insertNewObject(typeId){
	}
	
	function getObjectData(objectId, callback){
	}
	
	function setObjectData(objectId, objectData, callback){
	}
	
	function removeElement(elementIndex){
		if (elementIndex < 0 || elementIndex >= root.elementsModel.getItemsCount()){
			console.error();
			
			return;
		}
		
		let elementId = root.elementsModel.getData("id", elementIndex);
		
		removeModel.send({"id":elementId})
	}
	
	function setElementName(elementIndex, name){
		if (elementIndex < 0 || elementIndex >= root.elementsModel.getItemsCount()){
			console.error();
			
			return;
		}
		
		let elementId = root.elementsModel.getData("id", elementIndex);
		
		renameQuery.send({"id": elementId, "newName": name})
	}
	
	function setElementDescription(elementIndex, description){
		if (elementIndex < 0 || elementIndex >= root.elementsModel.getItemsCount()){
			console.error();
			
			return;
		}
		
		let elementId = root.elementsModel.getData("id", elementIndex);
		
		setDescriptionQuery.send({"id":elementId,"description":description});
	}
	
	function getHeaders(){
		return {};
	}
	
	GqlRequestSender {
		id: removeModel;
		requestType: 1;
		gqlCommandId: root.gqlRemoveCommandId;
		
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("id", params["id"]);
			
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("removedNotification");
			queryFields.InsertField("id");
			query.AddField(queryFields);
		}
		
		function onResult(data){
			if (data.containsKey("id")){
				var itemId = data.getData("id");
				
				root.removed(itemId);
			}
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlRequestSender {
		id: renameQuery;
		requestType: 1;
		gqlCommandId: root.gqlRenameCommandId;
		
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			
			inputParams.InsertField("id", params["id"]);
			inputParams.InsertField("newName", params["newName"]);
			
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("rename");
			query.AddField(queryFields);
		}
		
		function onResult(data){
			let id = data.getData("id");
			let newName = data.getData("name");
			
			root.renamed(id, newName);
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlRequestSender {
		id: setDescriptionQuery
		gqlCommandId: root.gqlSetDescriptionCommandId;
		
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("id", params["id"]);
			inputParams.InsertField("description", params["description"]);
			
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("setDescription");
			queryFields.InsertField("description");
			
			query.AddField(queryFields);
		}
		
		function onResult(data){
			var id = data.getData("id");
			var description = data.getData("description");
			
			root.descriptionSetted(id ,description);
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlRequestSender {
		id: headersGqlModel;
		gqlCommandId: root.gqlGetHeadersCommandId;
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			query.AddParam(inputParams);
			
			var queryHeaders = Gql.GqlObject("headers");
			queryHeaders.InsertField("id");
			queryHeaders.InsertField("name");
			query.AddField(queryHeaders);
			
			internal.headersUpdatingBlock = true;
		}
		
		function onResult(data){
			if (data.containsKey("filterSearch")){
				let filterSearchModel = data.getData("filterSearch")
				
				root.filterableHeadersModel = filterSearchModel;
			}
			
			if(data.containsKey("headers")){
				data = data.getData("headers");
				
				if (root.headersModel){
					root.headersModel.destroy();
				}
				
				root.headersModel = data;
			}
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
		
		onStateChanged: {
			if (state !== "Loading"){
				internal.headersUpdatingBlock = false;
			}
		}
	}
	
	GqlRequestSender {
		id: elementsGqlModel;
		gqlCommandId: root.gqlGetListCommandId;
		
		function createQueryParams(query, params){
			var viewParams = Gql.GqlObject("viewParams");
			viewParams.InsertField("count", params["count"]);
			viewParams.InsertField("offset", params["offset"]);
			viewParams.InsertField("filterModel", params["filterModel"]);

			var inputParams = Gql.GqlObject("input");
			inputParams.InsertFieldObject(viewParams);

			root.addAdditionalInputParams(inputParams)
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("items");
			
			queryFields.InsertField("id");
			queryFields.InsertField("name");
			
			for(var i = 0; i < root.headersModel.getItemsCount(); i++){
				let headerId = root.headersModel.getData("id", i);
				queryFields.InsertField(headerId);
			}
			
			for (let i = 0; i < root.additionalFieldIds.length; i++){
				queryFields.InsertField(root.additionalFieldIds[i]);
			}
			
			query.AddField(queryFields);
		}
		
		function onResult(data){
			if (data.containsKey("notification")){
				root.notificationModel = data.getData("notification");
			}
			
			if (data.containsKey("items")){
				root.elementsModel = data.getData("items");
			}
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
		
		onStateChanged: {
			if (state !== "Loading"){
				root.endUpdate();
			}
		}
	}
	
	GqlRequestSender {
		id: importObjectRequestSender;
		gqlCommandId: root.gqlImportCommandId;
		requestType: 1;
		
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			if (params){
				for (let key in params){
					inputParams.InsertField(key, params[key]);
				}
			}
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("import");
			queryFields.InsertField("status");
			query.AddField(queryFields);
		}
		
		function onResult(data){
			root.imported("");
		}
	}
	
	GqlRequestSender {
		id: exportObjectRequestSender;
		gqlCommandId: root.gqlExportCommandId;
		
		function createQueryParams(query, params){
			var inputParams = Gql.GqlObject("input");
			if (params){
				for (let key in params){
					inputParams.InsertField(key, params[key]);
				}
			}
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("export");
			queryFields.InsertField("fileData");
			query.AddField(queryFields);
		}
		
		function onResult(data){
			if (data.containsKey("fileData")){
				let contentData = data.getData("fileData");
				let fileName = data.getData("fileName");
				root.exported(fileName, contentData);
			}
		}
	}
}
