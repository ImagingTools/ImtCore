import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: popupContainer

	width: Style.sizeHintS
	z: 10000

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

			readonly property string accentColor: messageType === "error" ? Style.popupErrorAccentColor
					: messageType === "warning" ? Style.popupWarningAccentColor
					: messageType === "success" ? Style.popupSuccessAccentColor
					: Style.popupInfoAccentColor

			readonly property string panelColor: messageType === "error" ? Style.popupErrorBackgroundColor
					: messageType === "warning" ? Style.popupWarningBackgroundColor
					: messageType === "success" ? Style.popupSuccessBackgroundColor
					: Style.popupInfoBackgroundColor

			readonly property string iconSource: messageType === "warning"
					? "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal)
					: messageType === "error"
					? "../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal)
					: messageType === "success"
					? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
					: messageType === "info"
					? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
					: ""

			width: parent ? parent.width : Style.sizeHintS
			height: Math.max(Style.sizeHintBXS, contentRow.height + 2 * Style.marginL)
			radius: Style.radiusM + 2
			color: defaultDelegateRoot.panelColor
			border.width: 1
			border.color: Style.borderColor
			clip: true

			opacity: 0
			x: 24

			Component.onCompleted: {
				enterOpacity.start()
				enterX.start()
			}

			NumberAnimation {
				id: enterOpacity
				target: defaultDelegateRoot
				property: "opacity"
				from: 0
				to: 1
				duration: 200
			}

			NumberAnimation {
				id: enterX
				target: defaultDelegateRoot
				property: "x"
				from: 24
				to: 0
				duration: 200
			}

			Rectangle {
				id: accentRail
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				width: 4
				color: defaultDelegateRoot.accentColor
			}

			Row {
				id: contentRow
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.marginL + accentRail.width
				anchors.rightMargin: Style.marginS
				spacing: Style.marginM

				Item {
					id: iconArea
					width: Style.iconSizeMedium
					height: Style.iconSizeMedium
					anchors.verticalCenter: parent.verticalCenter

					Rectangle {
						anchors.fill: parent
						radius: width / 2
						color: defaultDelegateRoot.accentColor
						opacity: Style.popupIconChipOpacity
					}

					Image {
						anchors.centerIn: parent
						width: Style.iconSizeM
						height: width
						sourceSize.height: height
						sourceSize.width: width
						source: defaultDelegateRoot.iconSource
					}
				}

				BaseText {
					id: messageTextItem
					width: Math.max(0, contentRow.width - iconArea.width - closeButton.width - 2 * contentRow.spacing)
					anchors.verticalCenter: parent.verticalCenter
					text: defaultDelegateRoot.messageText
					wrapMode: Text.WordWrap
					color: Style.textColor
					font.pixelSize: Style.fontSizeNormal
				}

				Rectangle {
					id: closeButton
					width: 28
					height: 28
					radius: 14
					anchors.verticalCenter: parent.verticalCenter
					color: closeMouseArea.containsMouse ? Style.popupCloseHoverColor : Style.popupCloseIdleColor

					BaseText {
						anchors.centerIn: parent
						text: "✕"
						font.pixelSize: Style.fontSizeSmall
						color: Style.subtitleColor
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
	}

	ListView {
		id: messageListView
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		height: contentHeight
		spacing: Style.marginM
		interactive: false

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
