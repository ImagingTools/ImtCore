import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0

/*!
	\qmltype CollectionItemSelectElementView
	\inqmlmodule imtcolgui
	\brief ItemSelectElementView backed by a collection list request.

	Same element as \c ItemSelectElementView, but the popup and the name resolution run
	over the collection controller (CObjectCollectionControllerCompBase) instead of
	GetSelectableItems - so the picker inherits the field mapping, the header-driven
	filtering and the text search the collection views already use.

	Usage:
	\qml
	CollectionItemSelectElementView {
		label: qsTr("Roles")
		commandId: ImtauthRolesSdlCommandIds.s_rolesList
		fields: [RoleItemDataTypeMetaInfo.s_id, RoleItemDataTypeMetaInfo.s_roleName]
		titleField: RoleItemDataTypeMetaInfo.s_roleName
		textFilterFieldIds: [RoleItemDataTypeMetaInfo.s_roleName]
	}
	\endqml

	\sa ItemSelectElementView, FilterableSelectCollectionDataProvider
*/
ItemSelectElementView {
	id: collectionItemSelectView

	//! GQL command-ID of the collection list request.
	property string commandId: ""

	//! Collection fields to request; the id and the title field have to be among them.
	property var fields: []

	property string idField: "id"
	property string titleField: "name"

	//! Fields the typed text is matched against, as collection field ids.
	property var textFilterFieldIds: []

	//! Ids the popup must never offer, beyond the already selected ones.
	property var excludeIds: []

	property string sortByField
	property string orderType: "ASC"

	dataProvider: __itemsProvider

	//! Override to add request headers, e.g. the product scope of a role list.
	function getHeaders(){
		return {}
	}

	//! Override to add fields to the request input.
	function setCustomInputParams(inputParams){
	}

	onItemsChanged: {
		if (!collectionItemSelectView.__resolvingNames && collectionItemSelectView.hasUnresolvedItems()){
			__nameResolver.fetch("")
		}
	}

	// Selection list of the popup.
	FilterableSelectCollectionDataProvider {
		id: __itemsProvider

		multiSelect: true
		commandId: collectionItemSelectView.commandId
		fields: collectionItemSelectView.fields
		idField: collectionItemSelectView.idField
		titleField: collectionItemSelectView.titleField
		textFilterFieldIds: collectionItemSelectView.textFilterFieldIds
		excludeIds: collectionItemSelectView.excludeIds
		sortByField: collectionItemSelectView.sortByField
		orderType: collectionItemSelectView.orderType

		function getHeaders(){
			return collectionItemSelectView.getHeaders()
		}

		function setCustomInputParams(inputParams){
			collectionItemSelectView.setCustomInputParams(inputParams)
		}
	}

	// Turns the ids assigned from outside into names, without disturbing the popup list.
	FilterableSelectCollectionDataProvider {
		id: __nameResolver

		multiSelect: true
		pageSize: 100
		commandId: collectionItemSelectView.commandId
		fields: collectionItemSelectView.fields
		idField: collectionItemSelectView.idField
		titleField: collectionItemSelectView.titleField
		sortByField: collectionItemSelectView.sortByField
		orderType: collectionItemSelectView.orderType

		function getHeaders(){
			return collectionItemSelectView.getHeaders()
		}

		function setCustomInputParams(inputParams){
			collectionItemSelectView.setCustomInputParams(inputParams)
		}

		onDataChanged: {
			collectionItemSelectView.resolveItemNames(__nameResolver.items)
		}
	}
}
