import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

/*!
	\qmltype CollectionFieldFilterDelegate
	\inqmlmodule imtcolgui
	\brief Field filter chip whose options come from a collection list request.

	Unlike FieldFilterDelegate it does not need the option list up front: the options are
	requested page by page while the popup is open and the typed text is matched on the
	server. The selected option is kept as an id plus its display text, so a filter can be
	set from navigation parameters before any option was ever loaded.

	\sa FieldFilterDelegate, FilterableSelectPopup, FilterableSelectCollectionDataProvider
*/
FilterDelegateBase {
	id: filterDelegate

	isActive: filterDelegate.selectedId !== ""
	mainButtonText: filterDelegate.selectedId !== "" ? filterDelegate.selectedText : filterDelegate.name

	property CollectionFilter collectionFilter: null

	property FieldFilter defaultFieldFilter: FieldFilter {
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}

	//! GQL command-ID of the collection list request the options come from.
	property string commandId

	//! Collection fields to request; the id and the title field have to be among them.
	property var fields: []

	property string idField: "id"
	property string titleField: "name"

	//! Fields the typed text is matched against, as collection field ids.
	property var textFilterFieldIds: []

	//! GroupFilter objects scoping the offered options, re-applied to every request.
	property var groupFilters: []

	property string sortByField
	property string filterPlaceholder: qsTr("Search")
	property int popupItemWidth: Style.sizeHintXS

	property string selectedId
	property string selectedText

	signal selectionChanged(string itemId, string itemText)

	//! Override to add request headers.
	function getHeaders(){
		return {}
	}

	//! Override to add fields to the request input.
	function setCustomInputParams(inputParams){
	}

	function setSelectedId(itemId, itemText, beQuiet){
		filterDelegate.selectedId = itemId
		filterDelegate.selectedText = itemText ? itemText : itemId
		filterDelegate.applyFilter(beQuiet)
		filterDelegate.selectionChanged(filterDelegate.selectedId, filterDelegate.selectedText)
	}

	function applyFilter(beQuiet){
		if (!filterDelegate.collectionFilter){
			internal.applyPending = true
			return
		}

		internal.applyPending = false

		filterDelegate.collectionFilter.removeFilterByFieldId(filterDelegate.defaultFieldFilter.m_fieldId)

		if (filterDelegate.selectedId !== ""){
			let filter = filterDelegate.defaultFieldFilter.copyMe()
			filter.m_filterValue = filterDelegate.selectedId
			filterDelegate.collectionFilter.addFieldFilter(filter)
		}

		if (!beQuiet){
			filterDelegate.collectionFilter.filterChanged()
		}
	}

	onCollectionFilterChanged: {
		if (filterDelegate.collectionFilter && internal.applyPending){
			filterDelegate.applyFilter(true)
		}
	}

	onOpenFilter: {
		let point = filterDelegate.popupPoint()
		ModalDialogManager.openDialog(popupComp, {"x": point.x, "y": point.y})
	}

	onClearFilter: {
		if (filterDelegate.selectedId !== ""){
			filterDelegate.setSelectedId("", "", beQuiet)
		}
	}

	QtObject {
		id: internal

		property bool applyPending: false
	}

	Connections {
		target: filterDelegate.collectionFilter

		function onCleared(beQuiet){
			filterDelegate.clearFilter(beQuiet)
		}

		// A selection made under the previous value of the filter this one follows
		// may not be among the options any more.
		function onFieldFilterAdded(fieldId, fieldValue){
			if (filterDelegate.filterMenu && filterDelegate.filterMenu.hasDependsOn(filterDelegate.filterId, fieldId)){
				if (filterDelegate.selectedId !== ""){
					filterDelegate.clearFilter(true)
				}
			}
		}

		function onFieldFilterRemoved(fieldId){
			if (filterDelegate.filterMenu && filterDelegate.filterMenu.hasDependsOn(filterDelegate.filterId, fieldId)){
				if (filterDelegate.selectedId !== ""){
					filterDelegate.clearFilter(true)
				}
			}
		}
	}

	FilterableSelectCollectionDataProvider {
		id: optionsProvider

		multiSelect: false
		commandId: filterDelegate.commandId
		fields: filterDelegate.fields
		idField: filterDelegate.idField
		titleField: filterDelegate.titleField
		textFilterFieldIds: filterDelegate.textFilterFieldIds
		groupFilters: filterDelegate.groupFilters
		sortByField: filterDelegate.sortByField

		function getHeaders(){
			return filterDelegate.getHeaders()
		}

		function setCustomInputParams(inputParams){
			filterDelegate.setCustomInputParams(inputParams)
		}
	}

	Component {
		id: scopeHeaderComp

		// Names the filter the options follow, so a short or empty list is not a surprise.
		Item {
			height: scopeLabel.height

			Image {
				id: scopeIcon

				anchors.left: parent.left
				anchors.top: parent.top

				width: Style.iconSizeXS
				height: width
				opacity: filterDelegate.isScoped ? 1 : 0.5
				sourceSize.width: width
				sourceSize.height: height
				source: "qrc:/" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal)
			}

			BaseText {
				id: scopeLabel

				anchors.left: scopeIcon.right
				anchors.leftMargin: Style.spacingXS
				anchors.right: parent.right

				text: filterDelegate.scopeText
				color: Style.subtitleColor
				elide: Text.ElideNone
				wrapMode: Text.WordWrap
			}
		}
	}

	Component {
		id: popupComp

		FilterableSelectPopup {
			id: popup

			dataProvider: optionsProvider
			headerComponent: filterDelegate.scopeFilter !== null ? scopeHeaderComp : null
			itemWidth: filterDelegate.popupItemWidth
			filterPlaceholder: filterDelegate.filterPlaceholder
			preselectedIds: filterDelegate.selectedId !== "" ? [filterDelegate.selectedId] : []
			knownItems: filterDelegate.selectedId !== "" ? [{ "id": filterDelegate.selectedId, "title": filterDelegate.selectedText }] : []

			onItemSelected: {
				let item = popup.getItem(index)
				filterDelegate.setSelectedId(itemId, item ? item.title : itemId, false)
			}
		}
	}
}
