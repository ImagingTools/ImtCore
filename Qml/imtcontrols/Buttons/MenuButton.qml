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
	property int itemHeight: Style.controlHeightM;
	property string displayId: "name"

	signal accepted(string buttonId);

	onClicked: {
		accepted(menuModel.getData("id", 0))
	}

	onAccepted: {
		console.log("ACCEPTED::", buttonId)
	}


	property Component delegate: Component {PopupMenuDelegate{
			id: popupDelegate

			decorator: Component{
				PopupItemDelegateDecorator{
					maxTextWidth: Style.sizeHintS
					onDecoratorWidthChanged: {
						if(decoratorWidth > menuButton.popupMenuWidth){
							menuButton.popupMenuWidth = decoratorWidth
						}
					}
					property real popupMenuWidth: menuButton.popupMenuWidth
					onPopupMenuWidthChanged: {
						if(popupMenuWidth > width){
							setDecoratorWidth(menuButton.popupMenuWidth)
						}
					}
				}
			}

			property real popupMenuWidth: menuButton.popupMenuWidth
			onPopupMenuWidthChanged: {
				if(popupMenuWidth > width){
					width = popupMenuWidth
				}
			}

			width: menuButton.popupMenuWidth;
			widthFromDecorator: true;
			height: menuButton.itemHeight;
			contentLeftMargin: Style.marginM

			highlighted: menuButton.currentIndex == model.index
			text: model.item ? model.item[menuButton.displayId] : model[menuButton.displayId]

			selected: menuButton.popup ? menuButton.popup.selectedIndex == model.index : false;

			onClicked: {
				if (menuButton.popup){
					let resultId = model.Id || model.id || model.item.m_id || "";
					//menuButton.popup.finished(resultId, model.index)
					menuButton.accepted(id)
					menuButton.close()
				}
			}

			onEntered: {
				if (menuButton.popup){
					menuButton.popup.selectedIndex = model.index;
				}
			}

		}
	}

	function open(){
		let point = menuButton.mapToItem(null, 0, menuButton.height);
		ModalDialogManager.openDialog(popupMenuComp, {
										  "x":     point.x,
										  "model": menuButton.menuModel})

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

	property Component popupMenuComp: Component {
		id: popupMenu;

		PopupMenuDialog {
			id: popup;

			opacity:  0;
			delegate: menuButton.delegate;
			width: menuButton.popupMenuWidth;
			itemHeight: menuButton.itemHeight;
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
				menuButton.popup = popup;
			}
		}
	}

}
