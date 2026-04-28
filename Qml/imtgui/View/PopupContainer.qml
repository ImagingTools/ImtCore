import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: popupContainer

	width: Style.sizeHintXS

	property int __nextAutoId: 0

	Column {
		id: messageColumn
		spacing: Style.marginM
		anchors.bottom: parent.bottom

		Repeater {
			model: popupModel

			delegate: Rectangle {
				id: delegateRoot

				property string messageId: model.id

				width: popupContainer.width
				height: Style.sizeHintBXS
				color: model.type == "error" ? "#ffcccc" : model.type == "warning" ? "#fff4cc" : "#ccffcc"
				radius: Style.radiusM
				border.color: Style.borderColor2

				Component.onCompleted: {
					if (model.closable){
						autoCloseTimer.restart();
					}
				}

				Item {
					id: iconItem
					width: 50;
					height: parent.height;

					Image {
						id: icon;
						anchors.centerIn: parent;
						width: Style.iconSizeM;
						height: width;
						sourceSize.height: height;
						sourceSize.width: width;
						source: model.type == "warning" ? "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal) :
								model.type == "error" ? "../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal) :
								model.type == "success" ? "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal) : "";
					}
				}

				BaseText {
					id: messageText
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: iconItem.right;
					anchors.right: parent.right;
					anchors.rightMargin: Style.marginM;
					text: model.text
					wrapMode: Text.WordWrap
				}

				ToolButton {
					id: closeButton;
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.margins: Style.marginM
					width: Style.buttonWidthXS
					height: width
					iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
					decorator: Component {
						ToolButtonDecorator {
							color: "transparent";
							icon.width: Style.iconSizeXS;
						}
					}

					onClicked: {
						popupContainer.removeMessageById(delegateRoot.messageId)
					}
				}

				Timer {
					id: autoCloseTimer
					interval: 5000
					onTriggered: {
						popupContainer.removeMessageById(delegateRoot.messageId)
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
		popupModel.remove(index);
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
				return i;
			}
		}

		return -1;
	}
}

