import QtQuick 2.0
import Acf 1.0

Item {
    id: comboBoxContainer;

    width: 90;
    height: 30;

    property var model;

    property color borderColor: Style.textColor; 
    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool textCentered: true;
    property bool menuVisible: false;

    property int radius: 5;
    property int currentIndex: -1;
    property int menuX;
    property int menuY;

    Component.onCompleted: {
        if (comboBoxContainer.textCentered){
            cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
        } else {
            cbTitleTxt.anchors.left = cbMainRect.left;
            cbTitleTxt.anchors.leftMargin = 10;
        }
    }

    function dialogResult(parameters){
        console.log("ComboBox dialogResult");

        comboBoxContainer.currentText = parameters["status"];
    }

    function openContextMenu(startX, startY) {
        var point = comboBoxContainer.mapToItem(thubnailDecoratorContainer, 0, comboBoxContainer.height);
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};

        parameters["model"] = comboBoxContainer.model;
        parameters["resultItem"] = comboBoxContainer;
        parameters["hasIcon"] = false;

        parameters["itemHeight"] = comboBoxContainer.height;
        parameters["itemWidth"] = comboBoxContainer.width;

        parameters["x"] = point.x;
        parameters["y"] = point.y;
        thubnailDecoratorContainer.openDialog(source, parameters);
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

            onClicked: {
                console.log("ComboBox clicked !");

//                comboBoxContainer.openContextMenu(0, 0);
                comboBoxContainer.menuVisible = !comboBoxContainer.menuVisible;
            }
        }
    }

    Rectangle {
        id: cbMenu;

        anchors.top: cbMainRect.bottom;

        width: comboBoxContainer.width;
        height: cbListView.count * comboBoxContainer.height;

        color: Style.baseColor;

        border.width: 1;
        border.color: comboBoxContainer.borderColor;

        radius: comboBoxContainer.radius;
        visible: comboBoxContainer.menuVisible;

        ListView {
            id: cbListView;

            anchors.fill: parent;

            model: comboBoxContainer.model;
            clip: true;
            currentIndex: 0;

            delegate: Item {
                id: cbListDelegate;

                width: cbListView.width;
                height: cbMainRect.height;

                Rectangle {
                    anchors.verticalCenter: cbListDelegate.verticalCenter;
                    anchors.horizontalCenter: cbListDelegate.horizontalCenter;

                    width: cbListDelegate.width - 2;
                    height: cbListDelegate.height - 2;

                    visible: comboBoxContainer.currentIndex === model.index;
                    color: Style.selectedColor;
                    radius: comboBoxContainer.radius;
                }

                Text {
                    id: cbTitleModel;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: comboBoxContainer.textCentered ? parent.horizontalCenter : null;
                    anchors.left: !comboBoxContainer.textCentered ? cbListDelegate.left : null;
                    anchors.leftMargin: 10;

                    text: model.text;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        comboBoxContainer.currentIndex = model.index;
                        cbTitleTxt.text = model.text;
                        comboBoxContainer.menuVisible = false;
                    }
                }


                Rectangle {
                    anchors.top: cbListDelegate.bottom;

                    width: cbListDelegate.width;
                    height: 1;

                    color: Style.textColor;
                    visible: model.index !== cbListView.count - 1;
                }
            }
        }
    }
}
