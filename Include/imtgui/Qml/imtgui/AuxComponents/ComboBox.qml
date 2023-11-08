import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

FocusScope {
	id: comboBoxContainer;

	width: 90;
	height: 30;

	property var model;

	property color borderColor: comboBoxContainer.focus ? Style.iconColorOnSelected : Style.borderColor;

	property color backgroundColor: Style.baseColor;

	property string currentText;

	property bool changeable: true;

	property bool isColor: false;

	property bool textCentered: false;
	property bool menuVisible: false;
	property bool hiddenBackground: true;
	property bool openST: false;
	property bool compTextCentered:  false;
	property bool visibleScrollBar: true;
	property string compMainColor: "transparent";
	property string compSelectedColor: Style.selectedColor;
	property bool moveToEnd: false;
	property int moveToIndex: -1;

	property int dialogsCountPrev: 1000;
	property int dialogsCount: modalDialogManager.count;
	property int shownItemsCount: 5;

	property int radius: 5;
	property int currentIndex: -1;

	property alias titleTxt: cbTitleTxt;
	property alias titleTxtColor: cbTitleTxt.color;

	property string placeHolderText: "";


	property real contentY;

	// ID for display in combo box delegates
	property string nameId: "Name";

    property int selectedIndex: -1;
    property bool hoverBlocked: true;

	property Component delegate: PopupMenuDelegate{
		width: comboBoxContainer.width;
		height: comboBoxContainer.itemHeight;
		textSize: comboBoxContainer.textSize;
		fontColor: comboBoxContainer.fontColor;
		textCentered: comboBoxContainer.compTextCentered;
		mainColor: comboBoxContainer.compMainColor;
		selectedColor: comboBoxContainer.compSelectedColor;
		rootItem: comboBoxContainer;
	   };

	property alias image: cbArrowIcon;
	property alias imageSource: cbArrowIcon.source;
	property alias imageWidth:  cbArrowIcon.width;
	property alias imageHeight: cbArrowIcon.height;
	property alias imageRotation: cbArrowIcon.rotation;
	property alias imageVisible: cbArrowIcon.visible;

	property alias containsMouse: cbMouseArea.containsMouse;

	property int textSize: Style.fontSize_common;
	property int itemHeight: 26;
	property string fontColor: Style.textColor;
	property string fontColorTitle: fontColor;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;
	property bool isOpen: false;

	signal clicked();
	signal finished(string commandId, int index);

    onOpenSTChanged: {
        selectedIndex = -1;
    }

	onModelChanged: {
		if (!comboBoxContainer.model){
			return;
		}

		if (comboBoxContainer.currentIndex > -1){
			comboBoxContainer.currentText = comboBoxContainer.model.GetData(comboBoxContainer.nameId);
		}
	}

	onCurrentIndexChanged: {
		console.log("ComboBox onCurrentIndexChanged", comboBoxContainer.currentIndex);
		if (!comboBoxContainer.model){
			return;
		}

		if (comboBoxContainer.currentIndex > -1){
			if (comboBoxContainer.model.ContainsKey(comboBoxContainer.nameId, comboBoxContainer.currentIndex)){
				let name = comboBoxContainer.model.GetData(comboBoxContainer.nameId, comboBoxContainer.currentIndex);
				comboBoxContainer.currentText = name;
			}
		}
		else{
			comboBoxContainer.currentText = "";
		}
	}

	Component {
		id: popupMenu;
		PopupMenuDialog {
			id: popup;

			delegate: comboBoxContainer.delegate;
			width: comboBoxContainer.width;
			itemHeight: comboBoxContainer.itemHeight;
			hiddenBackground: comboBoxContainer.hiddenBackground;
			textSize: comboBoxContainer.textSize;
			fontColor: comboBoxContainer.fontColor;
			shownItemsCount: comboBoxContainer.shownItemsCount;
			moveToEnd: comboBoxContainer.moveToEnd;
			moveToIndex: comboBoxContainer.moveToIndex;
			rootItem: comboBoxContainer;
			visibleScrollBar: comboBoxContainer.visibleScrollBar;
			onFinished: {
				comboBoxContainer.currentIndex = index;
				comboBoxContainer.isOpen = false;
                popup.root.closeDialog();
			}
			Component.onCompleted: {
				comboBoxContainer.finished.connect(popup.finished);
			}
		}
	}

	Component.onCompleted: {
		if (comboBoxContainer.textCentered){
			cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
		}
		else {
			cbTitleTxt.anchors.left = cbMainRect.left;
			cbTitleTxt.anchors.leftMargin = 10;
		}

	}

	onDialogsCountChanged: {
		comboBoxContainer.openST = comboBoxContainer.dialogsCount > comboBoxContainer.dialogsCountPrev;
		if(!comboBoxContainer.openST && comboBoxContainer.dialogsCountPrev < 1000){
			comboBoxContainer.dialogsCountPrev = 1000;
		}
	}


	function openPopupMenu(){
		comboBoxContainer.isOpen = true;
		comboBoxContainer.dialogsCountPrev = modalDialogManager.count;
		//var point = comboBoxContainer.mapToItem(thumbnailDecoratorContainer, 0, comboBoxContainer.height);
		var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
		modalDialogManager.openDialog(popupMenu, { "x":     point.x,
												   "y":     point.y,
												   "model": comboBoxContainer.model,
												   "width": comboBoxContainer.width,
												   "countVisibleItem": 5 });

	}

	function closePopupMenu(){
		comboBoxContainer.isOpen = false;
		modalDialogManager.closeDialog();
	}

	Rectangle {
		id: cbMainRect;

		anchors.fill: parent;

		border.color: comboBoxContainer.borderColor;
		border.width: 1;

		radius: comboBoxContainer.radius;
		color: comboBoxContainer.backgroundColor;

		gradient: Gradient {

			GradientStop { position: 0.0; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
			GradientStop { position: 0.97; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
			GradientStop { position: 0.98; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
			GradientStop { position: 1.0; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }

		}

		Text {
			id: cbTitleTxt;

			anchors.verticalCenter: parent.verticalCenter;
			anchors.left: parent.left;
            anchors.right: cbArrowIcon.left;
			anchors.rightMargin: 10;
			color: comboBoxContainer.fontColorTitle;
			text: comboBoxContainer.currentText !== "" ? comboBoxContainer.currentText : comboBoxContainer.placeHolderText;
			font.family: Style.fontFamily;
			font.pixelSize: comboBoxContainer.textSize;

			elide: Text.ElideRight;
		}

		Image {
			id: cbArrowIcon;

			anchors.right: cbMainRect.right;
			anchors.verticalCenter: cbMainRect.verticalCenter;
			anchors.rightMargin: 5;

			width: 12;
			height: 10;
			rotation: comboBoxContainer.isOpen ? 180 : 0
            source: "../../../" + Style.getIconPath("Icons/Down", "On", "Normal");

			sourceSize.width: width;
			sourceSize.height: height;
		}

		MouseArea {
			id: cbMouseArea;

			anchors.fill: parent;
			hoverEnabled: true;

			cursorShape: comboBoxContainer.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor;

			onClicked: {
//                comboBoxContainer.forceActiveFocus();
                comboBoxContainer.focus = true

                if (comboBoxContainer.changeable && comboBoxContainer.model !==undefined && comboBoxContainer.model !== null && comboBoxContainer.model.GetItemsCount() > 0){
					comboBoxContainer.openPopupMenu();
				}

				comboBoxContainer.clicked();
                console.log("comboBoxContainer.clicked()")
			}

			onPressed: {
				if(tooltip.text !== ""){
					tooltip.closeTooltip();
				}
			}

			onEntered: {
				if(tooltip.text !== ""){
					pauseTooltip.stop();
					pauseTooltip.start();

				}

			}

			onExited: {
				if(tooltip.text !== ""){
					pauseTooltip.stop();
					tooltip.closeTooltip();
				}
			}
		}

		CustomTooltip{
			id: tooltip;
		}

		PauseAnimation {
			id: pauseTooltip;

			duration: tooltip.waitingDuration;
			onFinished: {
				tooltip.openTooltip(cbMouseArea.mouseX, cbMouseArea.mouseY);
			}
		}
	}

    Shortcut {
        sequence: "Space";
        enabled: !comboBoxContainer.isOpen && comboBoxContainer.focus;
        onActivated: {
            openPopupMenu();
        }
    }

    Shortcut {
        sequence: "Down";
        enabled: !comboBoxContainer.isOpen && comboBoxContainer.focus;
        onActivated: {
            if(comboBoxContainer.currentIndex < comboBoxContainer.model.GetItemsCount() - 1){
                comboBoxContainer.currentIndex++;
            }

        }
    }

    Shortcut {
        sequence: "Up";
        enabled: !comboBoxContainer.isOpen && comboBoxContainer.focus;
        onActivated: {
            if(comboBoxContainer.currentIndex > 0){
                comboBoxContainer.currentIndex--;
            }
        }
    }
}
