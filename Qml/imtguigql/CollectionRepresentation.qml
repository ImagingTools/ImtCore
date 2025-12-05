import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

Item {
	id: root;
	
	property string collectionId: "";
	
	property string gqlGetListCommandId: collectionId + "List";
	property string gqlImportCommandId: collectionId + "Import";
	property string gqlExportCommandId: collectionId + "Export";

	property TreeItemModel elementsModel: TreeItemModel {}
	property var headersModel: null
	property TreeItemModel notificationModel: TreeItemModel {}
	
	property var additionalFieldIds: []
	property var requestedFields: []

	signal removed()
	signal elementsRemoved(var elementIds)
	signal elementSetRemoved()
	
	signal renamed(string objectId, string newName);
	signal imported(string objectId);
	signal exported(string fileName, string data);
	signal descriptionSetted(string objectId, string description);
	signal headersReceived(var headers);
	signal elementsReceived(var elements);
	signal objectMetaInfoReceived(var metaInfoData);
	signal objectMetaInfoReceiveFailed(string message);
	signal visualStatusReceived(string objectId, string icon, string text, string description)
	signal visualStatusReceiveFailed(string objectId, string errorMessage)

	signal beginUpdate();
	signal endUpdate();

	Component {
		id: selectionParamsComp
		ParamsSet {
		}
	}

	Component {
		id: parameterComp
		Parameter {
		}
	}

	ParamsSetController {
		id: paramsSetController
	}

	function createSelectionParams(collectionFilter, documentFilter){
		let selectionParams = selectionParamsComp.createObject(root)
		paramsSetController.paramsSet = selectionParams

		if (collectionFilter){
			paramsSetController.addParam("ComplexFilter", "ComplexFilter", "", "",  collectionFilter.toJson())
		}

		if (documentFilter){
			paramsSetController.addParam("DocumentFilter", "DocumentFilter", "", "", documentFilter.toJson())
		}

		return selectionParams
	}

	function importObject(typeId, name, description, b64encoded, mimeType){
		importObjectInput.m_typeId = typeId
		importObjectInput.m_name = name
		importObjectInput.m_description = description
		importObjectInput.m_fileData = b64encoded
		importObjectInput.m_mimeType = mimeType
		importObjectRequest.send(importObjectInput)
	}
	
	function exportObject(objectId, mimeType, additionalParamsObj){
		exportObjectInput.m_objectId = objectId
		exportObjectInput.m_mimeType = mimeType
		exportObjectRequest.send(exportObjectInput)
	}

	function getObjectMetaInfo(objectId){
		getElementMetaInfoInput.m_elementId = objectId
		getElementMetaInfoRequest.send(getElementMetaInfoInput)
	}

	function getObjectVisualStatus(objectId, typeId){
		objectVisualStatusInput.m_objectId = id
		objectVisualStatusInput.m_typeId = typeId
		getObjectVisualStatusRequest.send(objectVisualStatusInput)
	}

	onCollectionIdChanged: {
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
		getCollectionHeaders()
	}
	
	function getCollectionHeaders(){
		getCollectionHeadersInput.m_collectionId = collectionId
		getCollectionHeadersRequest.send(getCollectionHeadersInput)
	}

	function updateElements(count, offset, paramsSet){
		if (internal.elementsUpdatingBlock){
			return;
		}

		if (root.collectionId === ""){
			console.error("Unable to update elements 'collectionId' is empty!");
			return;
		}

		root.beginUpdate();
		elementsGqlModel.gqlCommandId = gqlGetListCommandId;
		collectionParams.m_collectionId = root.collectionId
		collectionParams.m_count = count
		collectionParams.m_offset = offset
		collectionParams.m_selectionParams = paramsSet

		elementsGqlModel.send(collectionParams)
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

		elementsReceived(elementsModel)
	}
	
	function getElementsRepresentation(){
		return root.elementsModel;
	}

	function removeElements(elementIds){
		removeElementsInput.m_elementIds = elementIds
		removeGqlSender.elementIds = elementIds
		removeGqlSender.send(removeElementsInput)
	}

	function removeElementSet(collectionFilter){
		let selectionParams = createSelectionParams(collectionFilter, null)
		removeElementSetInput.m_selectionParams = selectionParams
		removeElementSetSender.send(removeElementSetInput)
	}
	
	function setElementName(objectId, name){
		setObjectNameInput.m_objectId = objectId
		setObjectNameInput.m_name = name
		setObjectNameRequest.send(setObjectNameInput)
	}
	
	function setElementDescription(objectId, description){
		setObjectDescriptionInput.m_objectId = objectId
		setObjectDescriptionInput.m_description = description
		setObjectDescriptionRequest.send(setObjectDescriptionInput)
	}

	function restoreObjectSet(collectionFilter){
		let selectionParams = createSelectionParams(collectionFilter, null)
		restoreObjectSetInput.m_selectionParams = filterModel
		restoreObjectSetRequest.send(restoreObjectSetInput)
	}
	
	function restoreObjects(objectIds){
		restoreObjectsInput.m_objectIds = objectIds
		restoreObjectsRequest.send(restoreObjectsInput)
	}

	function getHeaders(){
		return {}
	}
	
	RemoveElementsInput {
		id: removeElementsInput
		m_collectionId: root.collectionId
	}
	
	RemoveElementSetInput {
		id: removeElementSetInput
		m_collectionId: root.collectionId
	}
	
	RestoreObjectsInput {
		id: restoreObjectsInput
		m_collectionId: root.collectionId
	}
	
	RestoreObjectSetInput {
		id: restoreObjectSetInput
		m_collectionId: root.collectionId
	}

	GetElementMetaInfoInput {
		id: getElementMetaInfoInput
		m_collectionId: root.collectionId
	}

	GqlSdlRequestSender {
		id: getElementMetaInfoRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getElementMetaInfo
		sdlObjectComp: Component {
			GetElementMetaInfoPayload {
				onFinished: {
					root.objectMetaInfoReceived(m_elementMetaInfo)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlSdlRequestSender {
		id: restoreObjectsRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_restoreObjects
		requestType: 1
		sdlObjectComp: Component {
			RestoreObjectsPayload {}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlSdlRequestSender {
		id: restoreObjectSetRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_restoreObjectSet
		requestType: 1
		sdlObjectComp: Component {
			RestoreObjectSetPayload {}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlSdlRequestSender {
		id: removeElementSetSender
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElementSet
		requestType: 1
		sdlObjectComp: Component {
			RemoveElementSetPayload {
				onFinished: {
					if (m_success){
						root.elementSetRemoved()
					}
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	GqlSdlRequestSender {
		id: removeGqlSender
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements
		requestType: 1
		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: {
					root.elementsRemoved(removeGqlSender.elementIds)
				}
			}
		}

		property var elementIds: []
		function getHeaders(){
			return root.getHeaders();
		}
	}
	
	SetObjectNameInput {
		id: setObjectNameInput
		m_collectionId: root.collectionId
	}
	
	GqlSdlRequestSender {
		id: setObjectNameRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectName
		requestType: 1
		sdlObjectComp: Component {
			SetObjectNamePayload {
				onFinished: {
					root.renamed(m_objectId, m_name)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	SetObjectDescriptionInput {
		id: setObjectDescriptionInput
		m_collectionId: root.collectionId
	}
	
	ObjectVisualStatusInput {
		id: objectVisualStatusInput
		m_collectionId: root.collectionId
	}

	GqlSdlRequestSender {
		id: getObjectVisualStatusRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectVisualStatus
		sdlObjectComp: Component {
			VisualStatus {
				onFinished: {
					root.visualStatusReceived(m_objectId, m_icon, m_text, m_description)
				}
			}
		}
		
		function onError(message, type){
			root.visualStatusReceiveFailed(message)
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}

	GqlSdlRequestSender {
		id: setObjectDescriptionRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectDescription
		requestType: 1
		sdlObjectComp: Component {
			SetObjectDescriptionPayload {
				onFinished: {
					root.descriptionSetted(m_objectId, m_description)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	GetCollectionHeadersInput {
		id: getCollectionHeadersInput
	}

	GqlSdlRequestSender {
		id: getCollectionHeadersRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getCollectionHeaders
		sdlObjectComp: Component {
			GetCollectionHeadersPayload {
				onFinished: {
					root.headersModel = m_headers
					root.headersReceived(m_headers)
				}
			}
		}

		// HTTP headers
		function getHeaders(){
			return root.getHeaders();
		}
	}

	CreateSubCollectionInput {
		id: collectionParams
	}

	function getRequestedFields(){
		if (requestedFields.length > 0){
			return requestedFields
		}

		let retVal = ["id", "name", "typeId"]
		
		if (root.headersModel){
			for(let i = 0; i < root.headersModel.getItemsCount(); i++){
				let headerInfo = root.headersModel.get(i).item
				retVal.push(headerInfo.m_id)
			}
		}
		
		for (let i = 0; i < root.additionalFieldIds.length; i++){
			retVal.push(root.additionalFieldIds[i])
		}

		return retVal
	}

	GqlRequestSender {
		id: elementsGqlModel;
		gqlCommandId: root.gqlGetListCommandId;
		
		function createQueryParams(query, params){
			var viewParams = Gql.GqlObject("viewParams");

			var inputParams = Gql.GqlObject("input");
			inputParams.fromObject(params);

			root.addAdditionalInputParams(inputParams)
			query.AddParam(inputParams);
			
			var queryFields = Gql.GqlObject("items");
			let requestedFields = root.getRequestedFields()

			for (let i = 0; i < requestedFields.length; ++i){
				queryFields.InsertField(requestedFields[i])
			}

			query.AddField(queryFields);
		}
		
		function onResult(data){
			if (data.containsKey("notification")){
				root.notificationModel = data.getData("notification");
			}
			
			if (data.containsKey("items")){
				root.elementsModel = data.getData("items");
				root.elementsReceived(root.elementsModel)
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

	ImportObjectInput {
		id: importObjectInput
		m_collectionId: root.collectionId
	}

	GqlSdlRequestSender {
		id: importObjectRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_importObject
		requestType: 1
		sdlObjectComp: Component {
			ImportObjectPayload {
				onFinished: {
					root.imported("")
				}
			}
		}
	}

	ExportObjectInput {
		id: exportObjectInput
		m_collectionId: root.collectionId
	}

	GqlSdlRequestSender {
		id: exportObjectRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_exportObject
		sdlObjectComp: Component {
			ExportObjectPayload {
				onFinished: {
					root.exported(m_fileName, m_fileData)
				}
			}
		}
	}
}
