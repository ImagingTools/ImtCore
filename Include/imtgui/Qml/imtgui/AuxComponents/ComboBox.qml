import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: comboBoxContainer;

    width: 90;
    height: 30;

    property var model;

    property color borderColor: comboBoxContainer.focus ? Style.iconColorOnSelected : Style.alternateBaseColor;

    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool textCentered: false;
    property bool menuVisible: false;

    property int radius: 5;
    property int currentIndex: -1;

    signal clicked();

    onModelChanged: {
        if (comboBoxContainer.currentIndex > -1){
            comboBoxContainer.currentText = comboBoxContainer.model.GetData("Name");
        }
    }

    onCurrentIndexChanged: {
        console.log("ComboBox onCurrentIndexChanged", comboBoxContainer.currentIndex);
        if (comboBoxContainer.currentIndex > -1){
            let name = comboBoxContainer.model.GetData("Name", comboBoxContainer.currentIndex);
            console.log("name", name);
            comboBoxContainer.currentText = name;
        }
    }

    Component {
        id: popupMenu;
        PopupMenuDialog {
            onFinished: {
                comboBoxContainer.currentIndex = index;
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

    function openPopupMenu(){
        var point = comboBoxContainer.mapToItem(thumbnailDecoratorContainer, 0, comboBoxContainer.height);
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                                   "y":     point.y,
                                                   "model": comboBoxContainer.model,
                                                   "width": comboBoxContainer.width,
                                                   "countVisibleItem": 5 });
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

            color: Style.textColor;
            text: comboBoxContainer.currentText;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
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

            cursorShape: Qt.PointingHandCursor;

            onClicked: {
                console.log("ComboBox clicked !");

                openPopupMenu();
                comboBoxContainer.clicked();
            }
        }
    }
}
