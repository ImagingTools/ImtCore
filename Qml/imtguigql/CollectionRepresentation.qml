import QtQuick 2.12
import Qt.labs.platform

import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
	id: root;

	property string collectionId: "";

	property TreeItemModel elementsModel: TreeItemModel {}
	property TreeItemModel headersModel: TreeItemModel {}
	property TreeItemModel objectEditorInfoModel: TreeItemModel {}

	property TreeItemModel notificationModel: TreeItemModel {}

	property TreeItemModel filterableHeadersModel: TreeItemModel {}

	property var additionalFieldIds: []
	property string importTitle
	property string importFilter

	signal removed(string objectId);
	signal renamed(string objectId, string newName);
	signal imported(string objectId);
	signal descriptionSetted(string objectId, string description);

	property alias removeGqlModel: removeModel;
	property alias headersGqlModel: headersGqlModel;

	property var payloadModel: null

	signal beginUpdate();
	signal endUpdate();

	onVisibleChanged: {
		console.log("onVisibleChanged", visible)
	}

	function importDocument(){
		console.log("importDocument()", visible)
		fileDialog.open()
	}

	onCollectionIdChanged: {
		console.log("onCollectionIdChanged", collectionId)
		updateModel();
	}

	onEndUpdate: {
		internal.elementsUpdatingBlock = false;
	}

	QtObject {
		id: internal;

		property string removeGqlCommand: root.collectionId + "Remove";
		property string renameGqlCommand: root.collectionId + "Rename";
		property string setDescriptionGqlCommand: root.collectionId + "SetDescription";
		property string importGqlCommand: root.collectionId + "Import";

		property bool elementsUpdatingBlock: false;
		property bool headersUpdatingBlock: false;
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

		headersGqlModel.getCollectionHeaders();
	}

	function updateElements(count, offset, filterModel){
		if (internal.elementsUpdatingBlock){
			return;
		}

		if (root.collectionId === ""){
			console.error("Unable to update elements 'collectionId' is empty!");
			return;
		}

		elementsGqlModel.getElements(count, offset, filterModel)
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

		let elementId = root.elementsModel.getData("Id", elementIndex);

		removeModel.remove(elementId)
	}

	function setElementName(elementIndex, name){
		if (elementIndex < 0 || elementIndex >= root.elementsModel.getItemsCount()){
			console.error();

			return;
		}

		let elementId = root.elementsModel.getData("Id", elementIndex);

		renameQuery.rename(elementId, name)
	}

	function setElementDescription(elementIndex, description){
		if (elementIndex < 0 || elementIndex >= root.elementsModel.getItemsCount()){
			console.error();

			return;
		}

		let elementId = root.elementsModel.getData("Id", elementIndex);

		setDescriptionQuery.setDescription(elementId, description)
	}

	function getHeaders(){
		return {};
	}

	GqlModel {
		id: removeModel;

		function remove(id) {
			var query = Gql.GqlRequest("mutation", internal.removeGqlCommand);

			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("Id", id);

			query.AddParam(inputParams);

			var queryFields = Gql.GqlObject("removedNotification");
			queryFields.InsertField("Id");
			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = root.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (this.state === "Ready"){
				var dataModelLocal;
				if (removeModel.containsKey("errors")){
					dataModelLocal = removeModel.getData("errors");

					if (dataModelLocal.containsKey(internal.removeGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.removeGqlCommand);
					}

					let message = ""
					if (dataModelLocal.containsKey("message")){
						message = dataModelLocal.getData("message");
					}

					let type;
					if (dataModelLocal.containsKey("type")){
						type = dataModelLocal.getData("type");
					}

					ModalDialogManager.showWarningDialog(message)

					return;
				}

				if (removeModel.containsKey("data")){
					dataModelLocal = removeModel.getData("data");

					if (dataModelLocal.containsKey(internal.removeGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.removeGqlCommand);

						if (dataModelLocal.containsKey("removedNotification")){
							dataModelLocal = dataModelLocal.getData("removedNotification");

							if (dataModelLocal.containsKey("Id")){
								var itemId = dataModelLocal.getData("Id");

								root.removed(itemId);
							}
						}
					}
				}
			}
		}
	}

	GqlModel {
		id: renameQuery;

		function rename(id, name) {
			var query;
			var queryFields;
			var inputParams = Gql.GqlObject("input");

			query = Gql.GqlRequest("mutation", internal.renameGqlCommand);

			inputParams.InsertField("Id", id);
			inputParams.InsertField("NewName", name);

			query.AddParam(inputParams);

			queryFields = Gql.GqlObject("rename");
			queryFields.InsertField("NewName");
			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = root.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (this.state === "Ready"){
				var dataModelLocal;

				if (renameQuery.containsKey("errors")){
					dataModelLocal = renameQuery.getData("errors");

					if (dataModelLocal.containsKey(internal.renameGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.renameGqlCommand);

						let message = ""
						if (dataModelLocal.containsKey("message")){
							message = dataModelLocal.getData("message");
						}

						let type;
						if (dataModelLocal.containsKey("type")){
							type = dataModelLocal.getData("type");
						}

						ModalDialogManager.showWarningDialog(message)
					}

					return;
				}

				if (renameQuery.containsKey("data")){
					dataModelLocal = renameQuery.getData("data");

					if (dataModelLocal.containsKey(internal.renameGqlCommand)) {
						dataModelLocal = dataModelLocal.getData(internal.renameGqlCommand);

						let id = dataModelLocal.getData("Id");
						let newName = dataModelLocal.getData("Name");

						root.renamed(id, newName);
					}
				}
			}
		}
	}

	GqlModel {
		id: setDescriptionQuery;

		function setDescription(id, description){
			var query = Gql.GqlRequest("mutation", internal.setDescriptionGqlCommand);

			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("Id", id);
			inputParams.InsertField("Description", description);

			query.AddParam(inputParams);

			var queryFields = Gql.GqlObject("setDescription");
			queryFields.InsertField("Description");

			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = root.getHeaders()

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (this.state === "Ready"){
				var dataModelLocal;

				if (setDescriptionQuery.containsKey("errors")){
					dataModelLocal = setDescriptionQuery.getData("errors");

					if (dataModelLocal.containsKey(internal.setDescriptionGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.setDescriptionGqlCommand);

						let message = ""
						if (dataModelLocal.containsKey("message")){
							message = dataModelLocal.getData("message");
						}

						let type;
						if (dataModelLocal.containsKey("type")){
							type = dataModelLocal.getData("type");
						}

						ModalDialogManager.showWarningDialog(message)
					}

					return;
				}

				if (setDescriptionQuery.containsKey("data")){
					dataModelLocal = setDescriptionQuery.getData("data");

					if (dataModelLocal.containsKey(internal.setDescriptionGqlCommand)) {

						dataModelLocal = dataModelLocal.getData(internal.setDescriptionGqlCommand);

						var id = dataModelLocal.getData("Id");
						var description = dataModelLocal.getData("Description");

						root.descriptionSetted(id ,description);
					}
				}
			}
		}
	}

	GqlModel {
		id: headersGqlModel;

		function getCollectionHeaders() {
			var query = Gql.GqlRequest("query", root.collectionId + "Info");

			var inputParams = Gql.GqlObject("input");
			query.AddParam(inputParams);

			var queryHeaders = Gql.GqlObject("headers");
			queryHeaders.InsertField("Id");
			queryHeaders.InsertField("Name");
			query.AddField(queryHeaders);

			var gqlData = query.GetQuery();
			let headers = root.getHeaders()
			internal.headersUpdatingBlock = true;

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (this.state === "Ready"){
				var dataModelLocal;

				if (this.containsKey("errors")){
					dataModelLocal = this.getData("errors");

					if (dataModelLocal.containsKey(root.collectionId + "Info")){
						dataModelLocal = dataModelLocal.getData(root.collectionId + "Info");
					}

					let message = ""
					if (dataModelLocal.containsKey("message")){
						message = dataModelLocal.getData("message");
					}

					let type;
					if (dataModelLocal.containsKey("type")){
						type = dataModelLocal.getData("type");
					}

					ModalDialogManager.showWarningDialog(message)

					return;
				}

				if (this.containsKey("data")){
					dataModelLocal = this.getData("data");

					if (dataModelLocal.containsKey(root.collectionId + "Info")){
						dataModelLocal = dataModelLocal.getData(root.collectionId + "Info");
						if (!dataModelLocal){
							return;
						}

						if (dataModelLocal.containsKey("FilterSearch")){
							let filterSearchModel = dataModelLocal.getData("FilterSearch")

							root.filterableHeadersModel = filterSearchModel;
						}

						if(dataModelLocal.containsKey("Headers")){
							dataModelLocal = dataModelLocal.getData("Headers");

							root.headersModel = dataModelLocal;

							internal.headersUpdatingBlock = false;
						}
					}
				}
			}
		}
	}

	GqlModel {
		id: elementsGqlModel;

		function getElements(count, offset, filterModel) {
			var query = Gql.GqlRequest("query", root.collectionId + "List");

			var viewParams = Gql.GqlObject("viewParams");
			viewParams.InsertField("Count", count);
			viewParams.InsertField("Offset", offset);
			if (filterModel.toGraphQL){
				viewParams.InsertField("ComplexFilterModel", filterModel);
			}
			else{
				viewParams.InsertField("FilterModel", filterModel.toJson());
			}

			var inputParams = Gql.GqlObject("input");
			inputParams.InsertFieldObject(viewParams);

			query.AddParam(inputParams);

			var queryFields = Gql.GqlObject("items");

			queryFields.InsertField("Id");
			queryFields.InsertField("Name");

			for(var i = 0; i < root.headersModel.getItemsCount(); i++){
				let headerId = root.headersModel.getData("Id", i);
				queryFields.InsertField(headerId);
			}

			for (let i = 0; i < root.additionalFieldIds.length; i++){
				queryFields.InsertField(root.additionalFieldIds[i]);
			}

			query.AddField(queryFields);

			var gqlData = query.GetQuery();
			let headers = root.getHeaders()
			console.log("Get headers", headers)
			root.beginUpdate();

			this.setGqlQuery(gqlData, headers);
		}

		onStateChanged: {
			if (this.state === "Ready"){
				var dataModelLocal;
				if (this.containsKey("errors")){
					dataModelLocal = this.getData("errors");

					if (dataModelLocal.containsKey(root.collectionId + "List")){
						dataModelLocal = dataModelLocal.getData(root.collectionId + "List");
					}

					let message = ""
					if (dataModelLocal.containsKey("message")){
						message = dataModelLocal.getData("message");
					}

					let type;
					if (dataModelLocal.containsKey("type")){
						type = dataModelLocal.getData("type");
					}

					ModalDialogManager.showWarningDialog(message)

					return;
				}

				if (this.containsKey("data")){
					dataModelLocal = this.getData("data");
					if (dataModelLocal.containsKey(root.collectionId + "List")){
						if (root.payloadModel){
							let responseObj = JSON.parse(this.json)
							if (!responseObj){
								console.error("Unable convert json ", json, " to object")
								return;
							}
							if ("data" in responseObj){
								let dataObject = responseObj["data"];
								let dataModelLocal = dataObject[root.collectionId + "List"];
								console.log("Load payloadModel")
								root.payloadModel.fromObject(dataModelLocal)
								root.elementsModel = root.payloadModel.m_items
							}
						}
						else{
							dataModelLocal = dataModelLocal.getData(root.collectionId + "List");

							if (dataModelLocal.containsKey("notification")){
								root.notificationModel = dataModelLocal.getData("notification");
							}

							if (!dataModelLocal.containsKey("items")){
								dataModelLocal.addTreeModel("items")
							}

							if (dataModelLocal.containsKey("items")){
								root.elementsModel = dataModelLocal.getData("items");
							}
						}
					}
				}
			}
			if (this.state !== "Loading"){
				root.endUpdate();
			}
		}
	}


	FileDialog {
		id: fileDialog

		title: root.importTitle

		fileMode: FileDialog.OpenFile

		property string dialogFilter: root.importFilter

		nameFilters: [dialogFilter]

		onAccepted: {
			let filePath;
			if (Qt.platform.os == "web"){
				filePath = fileDialog.file.toString()
			}
			else{
				filePath = fileDialog.selectedFile.toString()
			}

			filePath = filePath.replace('file:///', '')

			if (Qt.platform.os == "web"){
				let reader = new FileReader()

				reader.readAsDataURL(filePath)

				reader.onload = function(){
					let encodedContentWithHeader = reader.result
					let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')
					gqlAddModel.importFile(filePath.name, encodedContent)
				}.bind(this)
			}
			else {
				fileIO.source = filePath
				let encodedContentWithHeader = reader.result
				let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')
				gqlAddModel.importFile(filePath.name, encodedContent)
			}
		}

		FileIO {
			id: fileIO
		}
	}

	GqlModel {
		id: gqlAddModel

		function importFile(fileName, b64encoded){
			let query = Gql.GqlRequest("mutation", internal.importGqlCommand)

			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("FileName", fileName);
			inputParams.InsertField("FileContent", b64encoded);

			query.AddParam(inputParams);

			var gqlData = query.GetQuery();

			this.setGqlQuery(gqlData);
		}

		onStateChanged: {
			console.log("onResult", gqlAddModel.state)
			let state = gqlAddModel.state
			if (state === "Error"){
				console.log("Network error")
				ModalDialogManager.showWarningDialog("Network error")
			}
			if (state === "Ready"){
				let dataModelLocal
				if (gqlAddModel.containsKey("errors")){
					dataModelLocal = gqlAddModel.getData("errors")

					if (dataModelLocal.containsKey(internal.importGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.importGqlCommand)
					}

					let message = ""
					if (dataModelLocal.containsKey("message")){
						message = dataModelLocal.getData("message")
					}

					let type
					if (dataModelLocal.containsKey("type")){
						type = dataModelLocal.getData("type")
					}
					console.log(message)
					ModalDialogManager.showWarningDialog(message)
				}
				else if (gqlAddModel.containsKey("data")){
					dataModelLocal = gqlAddModel.getData("data")
					console.log("onResult data", dataModelLocal.toJson())

					let documentId = ""

					if (dataModelLocal.containsKey(internal.importGqlCommand)){
						dataModelLocal = dataModelLocal.getData(internal.importGqlCommand)
						dataModelLocal = dataModelLocal.getData("addedNotification")

						if (dataModelLocal){
							documentId = dataModelLocal.getData("Id")
						}
					}

					root.imported(documentId)

					let message
					message = qsTr("File import successful")

					ModalDialogManager.showInfoDialog(message)
				}
			}
		}
	}

}
