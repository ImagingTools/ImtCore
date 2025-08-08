import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


ControlBase {
	id: comboBoxContainer;

	decorator: Style.comboBoxDecorator

	property var model: 0;

	property color borderColor: comboBoxContainer.activeFocus ? Style.iconColorOnSelected : Style.borderColor;

	property color backgroundColor: changeable ? Style.baseColor : Style.alternateBaseColor;

	property string currentText;

	property bool changeable: true;

	property bool isColor: false;

	property bool textCentered: false;
	property bool hiddenBackground: true;
	property bool openST: false;

	property bool visibleScrollBar: true;
	property bool visibleIcon: true;
	property string compMainColor: "transparent";
	property string compSelectedColor: Style.selectedColor;
	property bool moveToEnd: false;
	property int moveToIndex: currentIndex;
	property int contentLeftMargin: Style.marginM;

	property int shownItemsCount: 5;

	property int radius: Style.comboBoxRadius;
	property int currentIndex: -1;

	property string placeHolderText: "";

	property real contentY;

	// ID for display in combo box delegates
	property string nameId: "name";

	property bool hoverBlocked: true;

	property var popup: null;

	property Component delegate: Component {PopupMenuDelegate{
			width: comboBoxContainer.width;
			height: comboBoxContainer.itemHeight;
			contentLeftMargin: comboBoxContainer.contentLeftMargin;

			highlighted: comboBoxContainer.currentIndex == model.index
			text: model.item ? model.item[comboBoxContainer.nameId] : model[comboBoxContainer.nameId]

			selected: comboBoxContainer.popup ? comboBoxContainer.popup.selectedIndex == model.index : false;

			onClicked: {
				if (comboBoxContainer.popup){
					let resultId = model.Id || model.id || "";
					comboBoxContainer.popup.finished(resultId, model.index)
				}
			}

			onEntered: {
				if (comboBoxContainer.popup){
					comboBoxContainer.popup.selectedIndex = model.index;
				}
			}
		}
	}

	property alias containsMouse: cbMouseArea.containsMouse;
	property alias mouseArea: cbMouseArea;

	property int textSize: Style.fontSizeM;
	property int itemHeight: Style.controlHeightM;
	property string fontColor: Style.textColor;
	property string fontColorTitle: fontColor;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;
	property bool isOpen: false;

	signal accepted();
	signal activated();
	signal highlighted(int index);

	signal clicked();
	signal finished(string itemId, int index);

	onModelChanged: {
		if (!comboBoxContainer.model){
			return;
		}

		if (comboBoxContainer.currentIndex > -1){
			if (comboBoxContainer.model.containsKey(comboBoxContainer.nameId, comboBoxContainer.currentIndex)){
				comboBoxContainer.currentText = comboBoxContainer.model.getData(comboBoxContainer.nameId, comboBoxContainer.currentIndex);
			}
		}
	}

	onCurrentIndexChanged: {
		if (!comboBoxContainer.model){
			comboBoxContainer.currentText = "";
			return;
		}

		if (comboBoxContainer.currentIndex > -1){
			if (comboBoxContainer.model.containsKey(comboBoxContainer.nameId, comboBoxContainer.currentIndex)){
				let name = comboBoxContainer.model.getData(comboBoxContainer.nameId, comboBoxContainer.currentIndex);
				comboBoxContainer.currentText = name;
			}
		}
		else{
			comboBoxContainer.currentText = "";
		}
	}

	property Component popupMenuComp: Component {
		id: popupMenu;

		PopupMenuDialog {
			id: popup;

			opacity:  0;
			delegate: comboBoxContainer.delegate;
			width: comboBoxContainer.width;
			itemHeight: comboBoxContainer.itemHeight;
			hiddenBackground: comboBoxContainer.hiddenBackground;
			textSize: comboBoxContainer.textSize;
			fontColor: comboBoxContainer.fontColor;
			shownItemsCount: comboBoxContainer.shownItemsCount;
			moveToEnd: comboBoxContainer.moveToEnd;
			moveToIndex: comboBoxContainer.moveToIndex;
			visibleScrollBar: comboBoxContainer.visibleScrollBar;
			selectedIndex: comboBoxContainer.currentIndex;

			onHeightChanged: {
				popup.setY();
			}

			function setY(){
				if(popup.height == 0){
					return
				}
				let point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height)
				let y_ = point.y;
				//console.log("Y__", y_, popup.height, ModalDialogManager.activeView.height)
				if(y_ + popup.height > ModalDialogManager.activeView.height){
					popup.y = point.y - popup.height - comboBoxContainer.height;
					popup.isUpwards = true;
				}
				else {
					popup.y = point.y;
				}
				popup.opacity = 1;
			}

			onFinished: {
				if (index >= 0){
					comboBoxContainer.currentIndex = index;
				}

				comboBoxContainer.isOpen = false;

				comboBoxContainer.finished(commandId, index)
			}

			onStarted: {
				comboBoxContainer.popup = popup;
			}
		}
	}

	onDecoratorChanged: {
		bindCurrentIndex.target = decorator_
	}

	Binding {
		id: bindCurrentIndex;
		property: "currentIndex"
		value: comboBoxContainer.currentIndex;
	}

	function closePopupMenu(){
		if (comboBoxContainer.popup){
			comboBoxContainer.popup.close();
		}

		comboBoxContainer.isOpen = false;
	}

	function openPopupMenu(){
		console.log("openPopupMenu", comboBoxContainer.model);
		let point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
		ModalDialogManager.openDialog(popupMenuComp, {
										  "x":     point.x,
										  "model": comboBoxContainer.model,
										  "width": comboBoxContainer.width});

		comboBoxContainer.isOpen = true;
	}

	function onMouseAreaClicked(){
		if (!comboBoxContainer.model || !comboBoxContainer.changeable){
			return;
		}

		comboBoxContainer.focus = true;
		comboBoxContainer.forceActiveFocus();

		if (comboBoxContainer.model !== undefined && comboBoxContainer.model.getItemsCount() > 0){
			comboBoxContainer.openPopupMenu();
		}

		comboBoxContainer.clicked();
	}

	MouseArea {
		id: cbMouseArea;

		anchors.fill: parent;
		hoverEnabled: true;

		cursorShape: comboBoxContainer.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor;

		onClicked: {
			comboBoxContainer.onMouseAreaClicked()
		}

		onPressed: {
			if(tooltip.text !== ""){
				tooltip.closeTooltip();
			}
		}

		onPositionChanged: {
			if(tooltip.text !== ""){
				tooltip.show(mouseX, mouseY);
			}
		}

		onExited: {
			if(tooltip.text !== ""){
				tooltip.hide();
			}
		}
	}

	CustomTooltip{
		id: tooltip;

		fitToTextWidth: true;
	}

	Shortcut {
		sequence: "Space";
		enabled: !comboBoxContainer.isOpen && (comboBoxContainer.activeFocus) && comboBoxContainer.visible;
		onActivated: {
			comboBoxContainer.openPopupMenu();
		}
	}

	Shortcut {
		sequence: "Ctrl+Up";
		enabled: !comboBoxContainer.isOpen && comboBoxContainer.activeFocus && comboBoxContainer.visible;
		onActivated: {
			if(comboBoxContainer.model !==undefined && comboBoxContainer.model.getItemsCount() && comboBoxContainer.currentIndex > 0){
				comboBoxContainer.currentIndex--;
			}
		}
	}

	Shortcut {
		sequence: "Ctrl+Down";
		enabled: !comboBoxContainer.isOpen && comboBoxContainer.activeFocus && comboBoxContainer.visible;
		onActivated: {
			if(comboBoxContainer.model !==undefined && comboBoxContainer.model.getItemsCount() && comboBoxContainer.currentIndex < (comboBoxContainer.model.getItemsCount()-1)){
				comboBoxContainer.currentIndex++;
			}
		}
	}
}
