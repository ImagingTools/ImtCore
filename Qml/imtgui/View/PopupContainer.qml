import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: popupContainer

	width: Style.sizeHintXS

	property int autoCloseInterval: 5000
	property int maxPopupCount: 5

	property int __nextAutoId: 0
	property var __customComponents: ({})
	property var __customProperties: ({})

	Component {
		id: __defaultDelegate

		Rectangle {
			id: defaultDelegateRoot

			property string messageId: ""
			property string messageType: ""
			property string messageText: ""
			property bool closable: false
			property var popupContainer: null

			width: parent ? parent.width : Style.sizeHintXS
			height: Style.sizeHintBXS
			color: messageType === "error" ? "#ffcccc" : messageType === "warning" ? "#fff4cc" : messageType === "info" ? "#cce5ff" : "#ccffcc"
			radius: Style.radiusM
			border.color: Style.borderColor2

			Item {
				id: iconItem
				width: 50
				height: parent.height

				Image {
					id: icon
					anchors.centerIn: parent
					width: Style.iconSizeM
					height: width
					sourceSize.height: height
					sourceSize.width: width
					source: defaultDelegateRoot.messageType === "warning" ? "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType === "error" ? "../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType === "success" ? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType === "info" ? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) : ""
				}
			}

			BaseText {
				id: messageTextItem
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: iconItem.right
				anchors.right: closeArea.left
				anchors.rightMargin: Style.marginM
				text: defaultDelegateRoot.messageText
				wrapMode: Text.WordWrap
			}

			Rectangle {
				id: closeArea
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.marginS
				width: 24
				height: 24
				radius: 12
				color: closeMouseArea.containsMouse ? "#dddddd" : "transparent"

				BaseText {
					anchors.centerIn: parent
					text: "✕"
					font.pixelSize: 14
				}

				MouseArea {
					id: closeMouseArea
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						if (defaultDelegateRoot.popupContainer){
							defaultDelegateRoot.popupContainer.removeMessageById(defaultDelegateRoot.messageId)
						}
					}
				}
			}
		}
	}

	ListView {
		id: messageListView
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: contentHeight
		spacing: Style.marginM
		interactive: false
		verticalLayoutDirection: ListView.BottomToTop

		model: popupModel

		delegate: Item {
			id: delegateWrapper

			property string messageId: model.id

			width: messageListView.width
			height: contentLoader.item ? contentLoader.item.height : Style.sizeHintBXS

			Component.onCompleted: {
				if (model.closable){
					autoCloseTimer.restart()
				}
			}

			Loader {
				id: contentLoader
				anchors.left: parent.left
				anchors.right: parent.right

				sourceComponent: popupContainer.__customComponents[model.id]
								 ? popupContainer.__customComponents[model.id]
								 : __defaultDelegate

				onLoaded: {
					if (item){
						if (item.hasOwnProperty("messageId"))
							item.messageId = model.id
						if (item.hasOwnProperty("messageType"))
							item.messageType = model.type
						if (item.hasOwnProperty("messageText"))
							item.messageText = model.text
						if (item.hasOwnProperty("closable"))
							item.closable = model.closable
						if (item.hasOwnProperty("popupContainer"))
							item.popupContainer = popupContainer

						// Inject extra custom properties from addCustomMessage
						var extra = popupContainer.__customProperties[model.id]
						if (extra){
							var keys = Object.keys(extra)
							for (var k = 0; k < keys.length; k++){
								var key = keys[k]
								if (item.hasOwnProperty(key)){
									item[key] = extra[key]
								}
							}
						}
					}
				}
			}

			Timer {
				id: autoCloseTimer
				interval: popupContainer.autoCloseInterval
				onTriggered: {
					popupContainer.removeMessageById(delegateWrapper.messageId)
				}
			}
		}
	}

	ListModel {
		id: popupModel
	}

	function __enforceMaxCount(){
		while (popupModel.count > maxPopupCount){
			let lastIndex = popupModel.count - 1
			let lastId = popupModel.get(lastIndex).id
			if (__customComponents.hasOwnProperty(lastId)){
				delete __customComponents[lastId]
			}
			if (__customProperties.hasOwnProperty(lastId)){
				delete __customProperties[lastId]
			}
			popupModel.remove(lastIndex)
		}
	}

	function addMessage(type, text, autoClose, id){
		if (!autoClose){
			autoClose = false
		}

		if (!id || id === ""){
			id = "__auto_" + __nextAutoId
			__nextAutoId++
		}

		let existingIndex = findMessage(id)
		if (existingIndex >= 0){
			popupModel.remove(existingIndex)
		}

		popupModel.insert(0, { "type": type, "text": text, "closable": autoClose, "id": id })
		__enforceMaxCount()
	}

	function addCustomMessage(id, customComponent, properties){
		if (!id || id === ""){
			id = "__auto_" + __nextAutoId
			__nextAutoId++
		}

		let type = properties && properties.type ? properties.type : ""
		let text = properties && properties.text ? properties.text : ""
		let autoClose = properties && properties.autoClose ? properties.autoClose : false

		let existingIndex = findMessage(id)
		if (existingIndex >= 0){
			popupModel.remove(existingIndex)
		}

		__customComponents[id] = customComponent
		__customProperties[id] = properties
		popupModel.insert(0, { "type": type, "text": text, "closable": autoClose, "id": id })
		__enforceMaxCount()
	}

	function replaceMessage(id, type, text, autoClose){
		if (!autoClose){
			autoClose = false
		}

		let existingIndex = findMessage(id)
		if (existingIndex >= 0){
			popupModel.remove(existingIndex)
			popupModel.insert(existingIndex, { "type": type, "text": text, "closable": autoClose, "id": id })
		}
	}

	function removeMessage(index){
		if (index >= 0 && index < popupModel.count){
			let id = popupModel.get(index).id
			popupModel.remove(index)
			if (__customComponents.hasOwnProperty(id)){
				delete __customComponents[id]
			}
			if (__customProperties.hasOwnProperty(id)){
				delete __customProperties[id]
			}
		}
	}

	function removeMessageById(id){
		let index = findMessage(id)
		if (index >= 0){
			removeMessage(index)
		}
	}

	function findMessage(id){
		for (let i = 0; i < popupModel.count; i++){
			if (id === popupModel.get(i).id){
				return i
			}
		}

		return -1
	}

	function clear(){
		popupModel.clear()
		__customComponents = {}
		__customProperties = {}
	}
}

