import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: popupContainer

	width: 300

	Column {
		id: messageColumn
		spacing: Style.sizeMainMargin
		anchors.bottom: parent.bottom

		Repeater {
			model: popupModel

			delegate: Rectangle {
				width: popupContainer.width
				height: 100
				color: model.type == "error" ? "#ffcccc" : model.type == "warning" ? "#fff4cc" : "#ccffcc"
				radius: 5
				border.color: Style.borderColor2

				Component.onCompleted: {
					if (model.closable){
						autoCloseTimer.start();
					}
				}

				Item {
					id: iconItem
					width: 50;
					height: parent.height;

					Image {
						id: icon;
						anchors.centerIn: parent;
						width: 20;
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
					anchors.rightMargin: Style.sizeMainMargin;
					text: model.text
					wrapMode: Text.WordWrap
				}

				ToolButton {
					id: closeButton;
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.margins: Style.sizeMainMargin
					width: 15
					height: width
					iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
					decorator: Component {
						ToolButtonDecorator {
							color: "transparent";
							icon.width: 12;
						}
					}

					onClicked: {
						popupModel.remove(model.index)
					}
				}

				Timer {
					id: autoCloseTimer
					interval: 5000
					onTriggered: {
						popupModel.remove(model.index)
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

		if (!id){
			id = ""
		}

		popupModel.insert(0, { "type": type, "text": text, "closable": autoClose, "id": id })
	}

	function removeMessage(index){
		popupModel.remove(index);
	}

	function findMessage(id){
		for (let i = 0; i < popupModel.count; i++){
			if (id == popupModel.get(i).id){
				return i;
			}
		}

		return -1;
	}
}

