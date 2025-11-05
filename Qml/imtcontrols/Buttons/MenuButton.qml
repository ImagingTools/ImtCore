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
	property string elementIdName: "id"
	property bool mainClickByModel: true

	signal accepted(string buttonId);

	onClicked: {
		let id = ""
		if(menuModel && mainClickByModel){
			id = menuModel.get(0).item !== undefined  ? menuModel.get(0).item[elementIdName] : menuModel.getData(elementIdName,0) !==undefined ? menuModel.getData(elementIdName,0) : "";
		}
		accepted(id)
	}

	onAccepted: {
	}


	property Component delegate: Component {PopupMenuDelegate{
			id: popupDelegate

			decorator: Component{
				PopupItemDelegateDecorator{
					maxTextWidth: Style.sizeHintS
					onDecoratorWidthChanged: {
						if(popupDelegate.visible){
							setPopupWidth()
						}
					}
					onPopupDelegateVisibleChanged: {
						if(popupDelegateVisible){
							setPopupWidth()
						}
					}
					property real popupMenuWidth: menuButton.popupMenuWidth
					property bool popupDelegateVisible : popupDelegate.visible
					onPopupMenuWidthChanged: {
						if(popupMenuWidth > width){
							setDecoratorWidth(menuButton.popupMenuWidth)
						}
					}

					function setPopupWidth(){
						if(decoratorWidth > menuButton.popupMenuWidth){
							menuButton.popupMenuWidth = decoratorWidth
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
			visible: (model.visible == undefined && model.item == undefined && model.item.m_visible == undefined) ? true : model.visible !== undefined ? model.visible : model.item.m_visible
			height: visible ?  menuButton.itemHeight : 0;
			contentLeftMargin: Style.marginM

			highlighted: menuButton.currentIndex == model.index
			text: model.item ? model.item[menuButton.displayId] : model[menuButton.displayId]

			selected: menuButton.popup ? menuButton.popup.selectedIndex == model.index : false;

			onClicked: {
				if (menuButton.popup){
					let resultId = model.item ? model.item[menuButton.elementIdName] : model[menuButton.elementIdName] ;
					//menuButton.popup.finished(resultId, model.index)
					menuButton.accepted(resultId)
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
