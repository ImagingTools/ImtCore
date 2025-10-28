import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Button{
	id: menuButton;

	isMenuButton: true

	property var menuModel: null
	property int currentIndex: -1
	property bool isOpen: false;
	property var popup: null;

	property real popupMenuWidth: width

	signal accepted(string buttonId);

	onClicked: {
		accepted(menuModel.getData("id", 0))
	}

	onAccepted: {
		console.log("ACCEPTED::", buttonId)
	}

	property Component delegate: Component {Button{
			width: menuButton.width
			text: model.name

			property string id: model.id

			onClicked: {
				menuButton.accepted(id)
				menuButton.close()
			}
		}
	}

	function open(){
		let point = menuButton.mapToItem(null, 0, menuButton.height);
		ModalDialogManager.openDialog(popupMenuComp, {
										  "x":     point.x,
										  "model": menuButton.menuModel,
										  "width": menuButton.width});

		isOpen = true;

	}

	function close(){
		ModalDialogManager.closeByComp(menuButton.popupMenuComp)

		menuButton.isOpen = false;

	}

	MouseArea{
		id: openMenuMA

		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		width: Style.iconSizeXS + Style.marginM
		z: 20

		hoverEnabled: true;
		cursorShape: Qt.PointingHandCursor

		onClicked: {
			menuButton.open();
			mouse.accepted = true;
		}
	}


	Component.onCompleted: {
		let index
		index = testModel.insertNewItem()
		testModel.setData("id", "button1", index)
		testModel.setData("name", "Button1", index)
		index = testModel.insertNewItem()
		testModel.setData("id", "button2", index)
		testModel.setData("name", "Button2", index)
		index = testModel.insertNewItem()
		testModel.setData("id", "button3", index)
		testModel.setData("name", "Button3", index)

		menuModel = testModel

	}

	TreeItemModel{
		id: testModel;
	}

	property Component popupMenuComp: Component {
		id: popupMenu;

		PopupMenuDialog {
			id: popup;

			opacity:  0;
			hasShadow: false
			color: "transparent"
			delegate: menuButton.delegate;
			width: menuButton.width;
			itemHeight: menuButton.height;
			hiddenBackground: true;
			shownItemsCount: 5;
			visibleScrollBar: false;
			selectedIndex: menuButton.currentIndex;

			onHeightChanged: {
				popup.setY();
			}

			function setY(){
				if(popup.height == 0){
					return
				}
				let point = menuButton.mapToItem(null, 0, menuButton.height)
				let y_ = point.y;
				// console.log("Y__", y_, popup.height, ModalDialogManager.activeView.height)
				if(y_ + popup.height > ModalDialogManager.activeView.height){
					popup.y = point.y - popup.height - menuButton.height;
					popup.isUpwards = true;
				}
				else {
					popup.y = point.y;
				}
				popup.opacity = 1;
			}

			onFinished: {
				if (index >= 0){
					menuButton.currentIndex = index;
				}

				menuButton.isOpen = false;

				//menuButton.finished(commandId, index)
			}

			onStarted: {
				//menuButton.popup = popup;
			}
		}
	}

}
