import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0


Item {
	id: representation

	property string collectionId: ""
	property string gqlGetListCommandId: collectionId + "List"
	property string permissionPath: ""

	property var additionalFieldIds: ["id", "name"]
	property var requestedFields: []

	property alias elementsModel: elementsList
	property alias headersModel: headersList
	property var notificationModel: null

	property bool elementsUpdatingBlock: false

	signal beginUpdate()
	signal endUpdate()
	signal headersReceived(var headers)
	signal elementsReceived(var elements)
	signal removed()

	function getHeaders(){
		return {}
	}

	function addAdditionalInputParams(inputParams){
	}

	function updateModel(){
		representation.getCollectionHeaders()
	}

	onCollectionIdChanged: {
		if (representation.collectionId !== ""){
			representation.updateModel()
		}
	}

	function getCollectionHeaders(){
		if (representation.collectionId === ""){
			console.error("Unable to load headers. 'collectionId' is empty")
			return
		}

		representation.beginUpdate()
		headersRequest.send()
	}

	function updateElements(count, offset, paramsSet){
		if (representation.elementsUpdatingBlock){
			return
		}

		if (representation.collectionId === ""){
			console.error("Unable to update elements. 'collectionId' is empty")
			return
		}

		representation.beginUpdate()
		representation.elementsUpdatingBlock = true
		elementsRequest.gqlCommandId = representation.gqlGetListCommandId
		collectionParams.m_collectionId = representation.collectionId
		collectionParams.m_count = count
		collectionParams.m_offset = offset
		collectionParams.m_selectionParams = paramsSet
		elementsRequest.send()
	}

	function fieldList(){
		if (representation.requestedFields && representation.requestedFields.length > 0){
			return representation.requestedFields
		}

		let fields = ["id", "name", "typeId"]
		if (representation.headersModel && representation.headersModel.getItemsCount){
			let count = representation.headersModel.getItemsCount()
			for (let i = 0; i < count; i++){
				let header = representation.headersModel.get(i)
				if (header && header.item && header.item.m_id){
					fields.push(header.item.m_id)
				}
			}
		}

		if (representation.additionalFieldIds){
			for (let i = 0; i < representation.additionalFieldIds.length; i++){
				fields.push(representation.additionalFieldIds[i])
			}
		}

		return fields
	}

	function makeHeaderItem(header){
		let item = {
			id: header && header.id ? header.id : "",
			m_id: header && header.id ? header.id : "",
			name: header && header.name ? header.name : "",
			m_name: header && header.name ? header.name : "",
			m_filterable: header && header.filterable === true,
			m_sortable: header && header.sortable === true
		}
		item.connectProperties = function(){
		}
		item.copyMe = function(){
			return representation.makeHeaderItem({
				id: item.m_id,
				name: item.m_name,
				filterable: item.m_filterable,
				sortable: item.m_sortable
			})
		}
		return item
	}

	function fillHeaders(items){
		headersList.clear()
		let rows = items ? items : []
		for (let i = 0; i < rows.length; i++){
			let header = rows[i]
			if (!header){
				continue
			}
			headersList.append({
				item: representation.makeHeaderItem(header)
			})
		}
	}

	function fillElements(items){
		elementsList.clear()
		let rows = items ? items : []
		if (rows.length > 0){
			elementsList.append(rows)
		}
	}

	function wrapNotification(payload){
		let fields = payload ? payload : {}
		let model = {
			count: 1,
			data: [fields]
		}
		model.containsKey = function(key){
			return fields[key] !== undefined && fields[key] !== null
		}
		model.getData = function(key){
			return fields[key]
		}
		return model
	}

	BaseModel {
		id: headersList
	}

	TreeItemModel {
		id: elementsList
		dynamicRoles: true
	}

	CreateSubCollectionInput {
		id: collectionParams
	}

	GqlPureRequestSender {
		id: headersRequest
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getCollectionHeaders
		context: representation.context

		function getHeaders(){
			return representation.getHeaders()
		}

		function createQueryParams(query){
			let inputObject = Gql.GqlObject("input")
			inputObject.InsertField("collectionId", representation.collectionId)
			query.AddParam(inputObject)
		}

		function getRequestedFields(){
			let headers = Gql.GqlObject("headers")
			headers.InsertField("id")
			headers.InsertField("name")
			headers.InsertField("filterable")
			headers.InsertField("sortable")

			let fields = Gql.GqlObject("")
			fields.InsertFieldObject(headers)
			return fields
		}

		onFinished: {
			representation.elementsUpdatingBlock = false
			if (status === 1 && result){
				let headers = result.headers ? result.headers : []
				representation.fillHeaders(headers)
				representation.headersReceived(headersList)
				return
			}
			representation.endUpdate()
		}
	}

	GqlPureRequestSender {
		id: elementsRequest
		context: representation.context

		function getHeaders(){
			return representation.getHeaders()
		}

		function createQueryParams(query){
			let inputObject = Gql.GqlObject("input")
			inputObject.fromObject(collectionParams)
			representation.addAdditionalInputParams(inputObject)
			query.AddParam(inputObject)
		}

		function getRequestedFields(){
			let items = Gql.GqlObject("items")
			let fields = representation.fieldList()
			for (let i = 0; i < fields.length; i++){
				items.InsertField(fields[i])
			}

			let notification = Gql.GqlObject("notification")
			notification.InsertField("pagesCount")
			notification.InsertField("totalCount")

			let resultFields = Gql.GqlObject("")
			resultFields.InsertFieldObject(items)
			resultFields.InsertFieldObject(notification)
			return resultFields
		}

		onFinished: {
			representation.elementsUpdatingBlock = false
			if (status === 1 && result){
				let items = result.items ? result.items : []
				representation.fillElements(items)
				if (result.notification){
					representation.notificationModel = representation.wrapNotification(result.notification)
				}
				else {
					representation.notificationModel = null
				}
				representation.elementsReceived(elementsList)
			}
			representation.endUpdate()
		}
	}
}
