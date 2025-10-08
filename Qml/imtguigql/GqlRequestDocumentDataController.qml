import QtQuick 2.12
import Qt.labs.platform

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

QtObject {
	id: container;

	property string documentId;
	property string documentName;
	property string documentDescription;
	property string typeId;
	property Component documentModelComp;
	property var documentModel;
	
	property string gqlGetCommandId: "";
	property string gqlAddCommandId: "";
	property string gqlUpdateCommandId: "";

	property string subscriptionCommandId;

	property var getRequestInputParam: Gql.GqlObject("input");
	property var addRequestInputParam: Gql.GqlObject("input");
	property var updateRequestInputParam: Gql.GqlObject("input");

	signal beforeSaveModel();
	signal beforeInsertModel();

	signal saved(string id, string name);
	signal error(string message, string type);
	signal modelChanged();

	Component.onDestruction: {
		if (documentModel){
			documentModel.destroy();
		}
	}

	onDocumentModelChanged: {
		modelChanged()
	}

	onSubscriptionCommandIdChanged: {
		if (subscriptionCommandId !== ""){
			subscriptionClient.gqlCommandId = subscriptionCommandId;
		}
	}

	function getDocumentId(){
		return documentId;
	}
	
	function getDocumentName(){
		return documentName;
	}
	
	function getDocumentTypeId(){
		return typeId;
	}
	
	function getDocumentModel(){
		return documentModel;
	}
	
	function getDocumentDescription(){
		return documentDescription;
	}

	onError: {
		ModalDialogManager.showWarningDialog(message)
	}
	
	function setupGetRequestInputData(){
		getRequestInputParam.InsertField ("id", getDocumentId());
		getRequestInputParam.InsertField("typeId", getDocumentTypeId());
	}
	
	function setupAddRequestInputData(){
		addRequestInputParam.InsertField ("id", getDocumentId());
		addRequestInputParam.InsertField ("item", getDocumentModel());
		addRequestInputParam.InsertField("typeId", getDocumentTypeId());
		addRequestInputParam.InsertField("name", getDocumentName());
		addRequestInputParam.InsertField("description", getDocumentDescription());
	}
	
	function setupUpdateRequestInputData(){
		updateRequestInputParam.InsertField ("id", getDocumentId());
		updateRequestInputParam.InsertField ("item", getDocumentModel());
		updateRequestInputParam.InsertField("typeId", getDocumentTypeId());
		updateRequestInputParam.InsertField("name", getDocumentName());
		updateRequestInputParam.InsertField("description", getDocumentDescription());
	}
	
	function updateDocumentModel(){
		setupGetRequestInputData();
		
		gqlGetModel.getData();
	}

	function insertDocument(){
		setupAddRequestInputData()
		
		beforeInsertModel();

		if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
			documentModel.m_id = documentId;
		}

		gqlAddModel.save();
	}

	function saveDocument(){
		setupUpdateRequestInputData()
		
		beforeSaveModel();

		if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
			documentModel.m_id = documentId;
		}

		gqlUpdateModel.save();
	}

	function createDocumentModel(){
		if (documentModelComp){
			let objectData = documentModelComp.createObject(container);
			if (objectData.connectProperties){
				objectData.connectProperties();
			}

			if (objectData.m_id !== undefined){
				objectData.m_id = container.documentId
			}

			documentModel = objectData;
		}
		else{
			console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
		}
	}

	// TODO: ???
	function getHeaders(){
		return {};
	}

	property SubscriptionClient subscriptionClient: SubscriptionClient {
		onMessageReceived: {
			container.hasRemoteChanges = true;
		}
	}

	property GqlRequest gqlUpdateModel: GqlRequest {
		function save(){
			var query = Gql.GqlRequest("mutation", container.gqlUpdateCommandId);
			query.AddParam(container.updateRequestInputParam);

			var queryFields = Gql.GqlObject("updatedNotification");
			queryFields.InsertField("id");
			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = container.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			let state = container.gqlUpdateModel.state;
			if (state === "Error"){
				container.error("Network error", "Critical");
			}
			if (state === "Ready"){
				let responseObj = JSON.parse(this.json)
				if (!responseObj){
					console.error("Unable convert json ", json, " to object")
					return;
				}

				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"];
					if (container.gqlUpdateCommandId in errorsObject){
						errorsObject = errorsObject[container.gqlUpdateCommandId]
					}

					let message = ""
					if ("message" in errorsObject){
						message = errorsObject["message"];
					}

					let type;
					if ("type" in errorsObject){
						type = errorsObject["type"];
					}

					container.error(message, type);

					return;
				}

				if ("data" in responseObj){
					let dataObject = responseObj["data"];

					if (container.gqlUpdateCommandId in dataObject){
						dataObject = dataObject[container.gqlUpdateCommandId];

						let documentId = "";

						if ("id" in dataObject){
							documentId = dataObject["id"]
						}

						container.saved(documentId, "");
					}
				}
			}
		}
	}

	function getBodyForGetModel(){
		return null
	}

	property GqlRequest gqlGetModel: GqlRequest {
		function getData(){
			var query = Gql.GqlRequest("query", container.gqlGetCommandId);

			var queryFields = container.getBodyForGetModel()
			if (!queryFields){
				queryFields = Gql.GqlObject("item");
			}
			query.AddField(queryFields);

			query.AddParam(container.getRequestInputParam);

			var gqlData = query.GetQuery();
			let headers = container.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (state === "Error"){
				container.error("Network error", "Critical");
			}
			else if (state === "Ready"){
				let responseObj = JSON.parse(this.json)
				if (!responseObj){
					console.error("Unable convert json ", json, " to object")
					return;
				}

				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"];
					if (container.gqlGetCommandId in errorsObject){
						errorsObject = errorsObject[container.gqlGetCommandId]
					}

					let message = ""
					if ("message" in errorsObject){
						message = errorsObject["message"];
					}

					let type;
					if ("type" in errorsObject){
						type = errorsObject["type"];
					}

					container.error(message, type);

					return;
				}

				if ("data" in responseObj){
					let dataObject = responseObj["data"];
					let itemObject = dataObject[container.gqlGetCommandId];
					
					let objectData = container.documentModelComp.createObject(container);
					if (objectData){
						objectData.fromObject(itemObject);
						
						container.documentModel = objectData;
					}
				}
			}
		}
	}//GqlRequest itemModel

	property GqlRequest gqlAddModel: GqlRequest {
		function save(){
			var query = Gql.GqlRequest("mutation", container.gqlAddCommandId);
			query.AddParam(container.addRequestInputParam);

			var queryFields = Gql.GqlObject("addedNotification");
			queryFields.InsertField("id");
			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = container.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (state === "Error"){
				container.error("Network error", "Critical");
			}
			if (state === "Ready"){
				let responseObj = JSON.parse(this.json)
				if (!responseObj){
					console.error("Unable convert json '", json, "' to object")
					return;
				}

				if ("errors" in responseObj){
					let errorsObject = responseObj["errors"];
					if (container.gqlAddCommandId in errorsObject){
						errorsObject = errorsObject[container.gqlAddCommandId]
					}

					let message = ""
					if ("message" in errorsObject){
						message = errorsObject["message"];
					}

					let type;
					if ("type" in errorsObject){
						type = errorsObject["type"];
					}

					container.error(message, type);

					return;
				}

				if ("data" in responseObj){
					let dataObject = responseObj["data"];

					if (container.gqlAddCommandId in dataObject){
						dataObject = dataObject[container.gqlAddCommandId]
					}

					let documentId = ""

					if ("id" in dataObject){
						documentId = dataObject["id"];
					}

					container.saved(documentId, "");
				}
			}
		}
	}
}

