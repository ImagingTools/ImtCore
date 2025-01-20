import QtQuick 2.12
import Qt.labs.platform

import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentDataController {
	id: container;

	property string gqlGetCommandId;
	property string gqlAddCommandId;
	property string gqlUpdateCommandId;

	property var payloadModel

	property string subscriptionCommandId;

	property bool ok: subscriptionCommandId != "" && documentId != "";

	property var getRequestInputParam: Gql.GqlObject("input");
	property var addRequestInputParam: Gql.GqlObject("input");
	property var updateRequestInputParam: Gql.GqlObject("input");

	signal beforeSaveModel();
	signal beforeInsertModel();

	Component.onCompleted: {
		let additionInputParams = container.getHeaders();
		if (additionInputParams && Object.keys(additionInputParams).length > 0){
			let additionParams = Gql.GqlObject("addition");
			for (let key in additionInputParams){
				additionParams.InsertField(key, additionInputParams[key]);
			}
			getRequestInputParam.InsertFieldObject(additionParams);
			addRequestInputParam.InsertFieldObject(additionParams);
			updateRequestInputParam.InsertFieldObject(additionParams);
		}

		setupObjectTypeId();
	}

	Component.onDestruction: {
		Events.sendEvent("UnRegisterSubscription", container.subscriptionClient);
	}

	onDocumentModelChanged: {
		updateRequestInputParam.InsertField ("Item", container.documentModel);
		addRequestInputParam.InsertField ("Item", container.documentModel);
	}

	onDocumentIdChanged: {
		getRequestInputParam.InsertField("Id", container.documentId);
		addRequestInputParam.InsertField("Id", container.documentId);
		updateRequestInputParam.InsertField("Id", container.documentId);
	}

	onTypeIdChanged: {
		setupObjectTypeId();
	}

	function setupObjectTypeId(){
		getRequestInputParam.InsertField("typeId", container.typeId);
		addRequestInputParam.InsertField("typeId", container.typeId);
		updateRequestInputParam.InsertField("typeId", container.typeId);
	}

	onError: {
		ModalDialogManager.showWarningDialog(message)
	}

	onOkChanged: {
		if (documentId !== ""){
			container.subscriptionClient.register();
		}
	}

	function updateDocumentModel(){
		gqlGetModel.getData();
	}

	function insertDocument(){
		beforeInsertModel();

		if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
			documentModel.m_id = documentId;
		}

		gqlAddModel.save();
	}

	function saveDocument(){
		beforeSaveModel();

		if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
			documentModel.m_id = documentId;
		}

		gqlUpdateModel.save();
	}

	function createDocumentModel(){
		if (container.documentModelComp){
			let objectData = container.documentModelComp.createObject(container);
			objectData.connectProperties();
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
		function register(){
			if (container.subscriptionCommandId === ""){
				console.warn("Unable to register subscription for document because command-ID is empty");
				return;
			}

			let subscriptionRequestId = container.subscriptionCommandId;
			var query = Gql.GqlRequest("subscription", subscriptionRequestId);
			var queryFields = Gql.GqlObject("notification");
			queryFields.InsertField("Id");
			query.AddField(queryFields);

			Events.sendEvent("RegisterSubscription", {"Query": query, "Client": container.subscriptionClient});
		}

		onStateChanged: {
			if (state === "Ready"){
				container.hasRemoteChanges = true;
			}
		}
	}

	property GqlRequest gqlUpdateModel: GqlRequest {
		function save(){
			var query = Gql.GqlRequest("mutation", container.gqlUpdateCommandId);
			query.AddParam(container.updateRequestInputParam);

			var queryFields = Gql.GqlObject("updatedNotification");
			queryFields.InsertField("Id");
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

				console.log("Updated", this.json);

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

						if ("Id" in dataObject){
							documentId = dataObject["Id"]
						}

						container.saved(documentId, "");
					}
				}
			}
		}
	}

	property GqlRequest gqlGetModel: GqlRequest {
		function getData(){
			var query = Gql.GqlRequest("query", container.gqlGetCommandId);

			var queryFields = Gql.GqlObject("item");
			queryFields.InsertField("Id");
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

					container.payloadModel.fromObject(itemObject);
				}
			}
		}
	}//GqlRequest itemModel

	property GqlRequest gqlAddModel: GqlRequest {
		function save(){
			var query = Gql.GqlRequest("mutation", container.gqlAddCommandId);
			query.AddParam(container.addRequestInputParam);

			var queryFields = Gql.GqlObject("addedNotification");
			queryFields.InsertField("Id");
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

					if ("Id" in dataObject){
						documentId = dataObject["Id"];
					}

					container.saved(documentId, "");
				}
			}
		}
	}
}

