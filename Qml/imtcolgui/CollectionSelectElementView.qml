import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0

/*!
	\qmltype CollectionSelectElementView
	\inqmlmodule imtcolgui
	\brief Single-select ElementView over a collection list request.

	The rows are requested page by page only while the popup is open and the typed text
	is matched on the server, so the host never loads the whole collection. The current
	value is shown from \c selectedText: the host takes it from its document, which carries
	the name next to the id.

	Usage:
	\qml
	CollectionSelectElementView {
		name: qsTr("Customer")
		commandId: "AccountsList"
		fields: ["id", "name"]
		textFilterFieldIds: ["name"]
		selectedId: orderData.m_customerId
		selectedText: orderData.m_customerName
		onItemSelected: {
			orderData.m_customerId = itemId
			orderData.m_customerName = item ? item.title : ""
		}
	}
	\endqml

	\sa FilterableSelectPopup, FilterableSelectCollectionDataProvider, CollectionItemSelectElementView
*/
ElementView {
	id: collectionSelectView

	//! GQL command-ID of the collection list request.
	property string commandId

	//! Collection fields to request; each one lands in \c item.values under its id.
	property var fields: []

	property string idField: "id"
	property string titleField: "name"

	//! Field shown on the second line of a popup row; empty for single-line rows.
	property string descriptionField

	//! Fields the typed text is matched against, as collection field ids.
	property var textFilterFieldIds: []

	//! GroupFilter objects scoping the offered rows, re-applied to every request.
	property var groupFilters: []

	property var excludeIds: []
	property string sortByField
	property string orderType: "ASC"

	property string selectedId
	property string selectedText

	property string placeHolderText: qsTr("Select an item")
	property string filterPlaceholder: qsTr("Search")
	property bool changeable: true
	property bool clearable: false
	property bool isSelectionRequired: false
	property string errorText: qsTr("Please select an item")
	property int popupItemHeight: collectionSelectView.descriptionField !== "" ? Style.controlHeightL + Style.marginL : Style.controlHeightL
	property int popupMaxVisibleItems: 6

	property bool isOpen: false

	//! A row was picked (\c item is { id, title, values }) or the value was cleared (\c item is null).
	signal itemSelected(string itemId, var item)

	bottomComp: collectionSelectView.isSelectionRequired && collectionSelectView.selectedId === "" ? errorComp : undefined

	//! Override to add request headers.
	function getHeaders(){
		return {}
	}

	//! Override to add fields to the request input.
	function setCustomInputParams(inputParams){
	}

	function itemValue(item, fieldId){
		if (!item || !item.values){
			return ""
		}

		let value = item.values[fieldId]

		return value === undefined || value === null ? "" : String(value)
	}

	function clearSelection(){
		collectionSelectView.selectedId = ""
		collectionSelectView.selectedText = ""
		collectionSelectView.itemSelected("", null)
	}

	function openPopup(){
		if (!collectionSelectView.changeable || !collectionSelectView.controlItem){
			return
		}

		let point = collectionSelectView.controlItem.mapToItem(null, 0, 0)
		let popupWidth = collectionSelectView.controlItem.width + 2 * Style.marginL
		let maxX = ModalDialogManager.activeView.width - popupWidth - Style.marginL

		internal.anchorTop = point.y
		internal.anchorBottom = point.y + collectionSelectView.controlItem.height

		collectionSelectView.isOpen = true
		ModalDialogManager.openDialog(popupComp, {
			"x": Math.max(Style.marginL, Math.min(point.x, maxX)),
			"y": internal.anchorBottom + Style.marginXS
		})
	}

	// Drops below the control, or above it when the window has no room left underneath.
	function placePopup(popup){
		let viewHeight = ModalDialogManager.activeView.height
		if (internal.anchorBottom + Style.marginXS + popup.height + Style.marginL > viewHeight){
			popup.y = Math.max(Style.marginL, internal.anchorTop - popup.height - Style.marginXS)
		}
		else{
			popup.y = internal.anchorBottom + Style.marginXS
		}
	}

	QtObject {
		id: internal

		property real anchorTop: 0
		property real anchorBottom: 0
	}

	FilterableSelectCollectionDataProvider {
		id: itemsProvider

		multiSelect: false
		commandId: collectionSelectView.commandId
		fields: collectionSelectView.fields
		idField: collectionSelectView.idField
		titleField: collectionSelectView.titleField
		textFilterFieldIds: collectionSelectView.textFilterFieldIds
		groupFilters: collectionSelectView.groupFilters
		excludeIds: collectionSelectView.excludeIds
		sortByField: collectionSelectView.sortByField
		orderType: collectionSelectView.orderType

		function getHeaders(){
			return collectionSelectView.getHeaders()
		}

		function setCustomInputParams(inputParams){
			collectionSelectView.setCustomInputParams(inputParams)
		}
	}

	controlComp: Component {
		Rectangle {
			id: control

			objectName: "CollectionSelect"

			width: collectionSelectView.controlWidth
			height: collectionSelectView.controlHeight

			radius: Style.comboBoxRadius
			color: collectionSelectView.changeable ? Style.baseColor : Style.alternateBaseColor
			border.width: 1
			border.color: control.activeFocus || collectionSelectView.isOpen ? Style.iconColorOnSelected : Style.borderColor

			Keys.onSpacePressed: {
				collectionSelectView.openPopup()
			}

			Keys.onReturnPressed: {
				collectionSelectView.openPopup()
			}

			MouseArea {
				id: controlMouseArea

				objectName: "MouseArea"

				anchors.fill: parent

				hoverEnabled: true
				enabled: collectionSelectView.changeable
				cursorShape: Qt.PointingHandCursor

				onClicked: {
					control.forceActiveFocus()
					collectionSelectView.openPopup()
				}
			}

			Text {
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: clearButton.visible ? clearButton.left : arrowIcon.left
				anchors.rightMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				text: collectionSelectView.selectedId !== "" ? collectionSelectView.selectedText : collectionSelectView.placeHolderText
				color: collectionSelectView.selectedId !== "" ? Style.textColor : Style.placeHolderTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}

			Button {
				id: clearButton

				objectName: "ClearButton"

				anchors.right: arrowIcon.left
				anchors.rightMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				width: Style.iconSizeXS
				height: Style.iconSizeXS

				visible: collectionSelectView.clearable && collectionSelectView.changeable && collectionSelectView.selectedId !== ""
				decorator: Component { IconButtonDecorator {} }
				iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

				onClicked: {
					collectionSelectView.clearSelection()
				}
			}

			Image {
				id: arrowIcon

				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter

				width: Style.iconSizeXS
				height: Style.iconSizeXS
				sourceSize.width: width
				sourceSize.height: height

				visible: collectionSelectView.changeable
				source: "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
			}
		}
	}

	Component {
		id: popupComp

		FilterableSelectPopup {
			id: popup

			dataProvider: itemsProvider
			itemWidth: collectionSelectView.controlItem ? collectionSelectView.controlItem.width : collectionSelectView.controlWidth
			itemHeight: collectionSelectView.popupItemHeight
			maxVisibleItems: collectionSelectView.popupMaxVisibleItems
			showSelectedGroup: collectionSelectView.clearable
			maxSelectedGroupItems: 1
			filterPlaceholder: collectionSelectView.filterPlaceholder
			preselectedIds: collectionSelectView.selectedId !== "" ? [collectionSelectView.selectedId] : []
			knownItems: collectionSelectView.selectedId !== "" ? [{ "id": collectionSelectView.selectedId, "title": collectionSelectView.selectedText }] : []

			delegate: Component {
				PopupMenuDelegate {
					id: rowDelegate

					width: ListView.view ? ListView.view.width : popup.itemWidth
					height: popup.itemHeight
					objectName: "FilterableSelectItem_" + model.index
					isSeparator: false
					text: ""

					// The row paints its own state below, so the decorator adds nothing.
					selected: false
					highlighted: false

					property int rowIndex: model.index

					Rectangle {
						anchors.fill: parent
						color: popup.rowBackgroundColor(rowDelegate.rowIndex, rowMouseArea.containsMouse,
							popup.dataProvider ? popup.dataProvider.isItemSelected(popup.getItemId(rowDelegate.rowIndex)) : false)
					}

					Column {
						z: 10
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						spacing: 2

						Text {
							width: parent.width
							text: popup.getItemText(rowDelegate.rowIndex)
							color: Style.textColor
							font.family: Style.fontFamily
							font.pixelSize: Style.fontSizeM
							elide: Text.ElideRight
						}

						Text {
							width: parent.width
							visible: collectionSelectView.descriptionField !== ""
							text: collectionSelectView.itemValue(popup.getItem(rowDelegate.rowIndex), collectionSelectView.descriptionField)
							color: Style.subtitleColor
							font.family: Style.fontFamily
							font.pixelSize: Style.fontSizeS
							elide: Text.ElideRight
						}
					}

					MouseArea {
						id: rowMouseArea

						anchors.fill: parent
						z: 20
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor

						onEntered: {
							popup.focusItem(rowDelegate.rowIndex)
						}

						onClicked: {
							popup.handleItemClick(popup.getItemId(rowDelegate.rowIndex), rowDelegate.rowIndex)
						}
					}
				}
			}

			onHeightChanged: {
				collectionSelectView.placePopup(popup)
			}

			onItemSelected: {
				// The row list may already be refetched here (selected group shown), the selection keeps the row.
				let selectedItems = popup.dataProvider.getSelectedItems()
				let item = selectedItems.length > 0 ? selectedItems[0] : popup.getItem(index)
				collectionSelectView.selectedId = itemId
				collectionSelectView.selectedText = item ? item.title : itemId
				collectionSelectView.itemSelected(itemId, item)
			}

			onSelectionChanged: {
				if (selectedIds.length === 0 && collectionSelectView.selectedId !== ""){
					collectionSelectView.clearSelection()
				}
			}

			Component.onDestruction: {
				collectionSelectView.isOpen = false
			}
		}
	}

	Component {
		id: errorComp

		Text {
			text: collectionSelectView.errorText
			color: Style.errorTextColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
		}
	}
}
