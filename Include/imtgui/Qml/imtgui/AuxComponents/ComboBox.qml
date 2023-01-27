import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: comboBoxContainer;

    width: 90;
    height: 30;

    property var model;

    property color borderColor: comboBoxContainer.focus ? Style.iconColorOnSelected : Style.alternateBaseColor;

    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool textCentered: false;
    property bool menuVisible: false;
    property bool hiddenBackground: true;
    property bool openST: false;
    property bool compTextCentered:  false;
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

    property real contentY;


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
    property alias containsMouse: cbMouseArea.containsMouse;

    property int textSize: Style.fontSize_common;
    property int itemHeight: 26;
    property string fontColor: Style.textColor;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    signal clicked();
    signal finished(string commandId, int index);

    onModelChanged: {
        if (comboBoxContainer.currentIndex > -1){
            comboBoxContainer.currentText = comboBoxContainer.model.GetData("Name");
        }
    }

    onCurrentIndexChanged: {
        console.log("ComboBox onCurrentIndexChanged", comboBoxContainer.currentIndex);
        if (comboBoxContainer.currentIndex > -1){
            console.log("model", JSON.stringify(model));
            let name = comboBoxContainer.model.GetData("Name", comboBoxContainer.currentIndex);
            console.log("name", name);
            comboBoxContainer.currentText = name;
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
            onFinished: {
                comboBoxContainer.currentIndex = index;
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
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }

        Text {
            id: cbTitleTxt;

            anchors.verticalCenter: parent.verticalCenter;

            color: comboBoxContainer.fontColor;
            text: comboBoxContainer.currentText;
            font.family: Style.fontFamily;
            font.pixelSize: comboBoxContainer.textSize;
        }

        Image {
            id: cbArrowIcon;

            anchors.right: cbMainRect.right;
            anchors.verticalCenter: cbMainRect.verticalCenter;
            anchors.rightMargin: 5;

            width: 12;
            height: 10;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

            sourceSize.width: width;
            sourceSize.height: height;
        }

        MouseArea {
            id: cbMouseArea;

            anchors.fill: parent;
            hoverEnabled: true;

            cursorShape: Qt.PointingHandCursor;

            onClicked: {
                comboBoxContainer.openPopupMenu();
                comboBoxContainer.clicked();
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
}
