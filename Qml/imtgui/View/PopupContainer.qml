import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: popupContainer

	width: Style.sizeHintXS

	property int autoCloseInterval: 5000

	property Component delegate: __defaultDelegate

	property int __nextAutoId: 0
	property var __customComponents: ({})

	Component {
		id: __defaultDelegate

		Rectangle {
			id: defaultDelegateRoot

			property string messageId
			property string messageType
			property string messageText
			property bool closable
			property var popupContainer

			width: popupContainer ? popupContainer.width : Style.sizeHintXS
			height: Style.sizeHintBXS
			color: messageType == "error" ? "#ffcccc" : messageType == "warning" ? "#fff4cc" : messageType == "info" ? "#cce5ff" : "#ccffcc"
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
					source: defaultDelegateRoot.messageType == "warning" ? "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType == "error" ? "../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType == "success" ? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) :
							defaultDelegateRoot.messageType == "info" ? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) : ""
				}
			}

			BaseText {
				id: messageText
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: iconItem.right
				anchors.right: closeButton.left
				anchors.rightMargin: Style.marginM
				text: defaultDelegateRoot.messageText
				wrapMode: Text.WordWrap
			}

			ToolButton {
				id: closeButton
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.marginM
				width: Style.buttonWidthXS
				height: width
				iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent"
						icon.width: Style.iconSizeXS
					}
				}

				onClicked: {
					if (defaultDelegateRoot.popupContainer){
						defaultDelegateRoot.popupContainer.removeMessageById(defaultDelegateRoot.messageId)
					}
				}
			}
		}
	}

	Column {
		id: messageColumn
		spacing: Style.marginM
		anchors.bottom: parent.bottom

		Repeater {
			model: popupModel

			delegate: Item {
				id: delegateWrapper

				property string messageId: model.id

				width: popupContainer.width
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

					sourceComponent: popupContainer.__getComponent(model.id)

					onLoaded: {
						if (item){
							item.messageId = model.id
							item.messageType = model.type
							item.messageText = model.text
							item.closable = model.closable
							item.popupContainer = popupContainer
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
	}

	ListModel {
		id: popupModel
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
		popupModel.insert(0, { "type": type, "text": text, "closable": autoClose, "id": id })
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
				__customComponents[id] = undefined
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
	}

	function __getComponent(id){
		if (__customComponents.hasOwnProperty(id) && __customComponents[id]){
			return __customComponents[id]
		}
		return popupContainer.delegate
	}
}

