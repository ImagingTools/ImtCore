import QtQuick 2.0
import Acf 1.0

Item {
    id: container;

    width: 90;
    height: 30;

    property var model;
    property int currentIndex: -1;
    property color borderColor: Style.textColor;
    property bool menuVisible: false;
    property color backgroundColor: Style.baseColor;
    property string currentText;
    property bool textCentered: true;
    property int radius: 5;

    Component.onCompleted: {
        if (container.textCentered){
            cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
           // cbTitleModel.anchors.horizontalCenter = cbListDelegate.horizontalCenter;
        } else {
            cbTitleTxt.anchors.left = cbMainRect.left;
            cbTitleTxt.anchors.leftMargin = 10;
        }
    }

    Rectangle {
        id: cbMainRect;

        anchors.fill: parent;

        border.color: container.borderColor;
        radius: container.radius;
        color: container.backgroundColor;

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
            text: container.currentText;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        MouseArea {
            id: cbMouseArea;

            anchors.fill: parent;

            onClicked: {
                console.log("ComboBox clicked !");
                console.log("text color ", Style.textColor);
                container.menuVisible = !container.menuVisible;
                console.log("menu width ", cbMenu.width);
                console.log("menu height ", cbMenu.height);
                console.log("menu visible ", cbMenu.visible);
                console.log("listview count  ", cbListView.count);
            }
        }

        Canvas {
            id: cbArrowIcon;

            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.rightMargin: 5;

            height: 5;
            width: 10;

            onPaint:{
                var ctx = getContext('2d');
                ctx.beginPath();
                ctx.moveTo(0,0);
                ctx.lineTo(width,0);
                ctx.lineTo(width * 0.5,height);
                ctx.lineTo(0,0);
                ctx.fillStyle = Style.textColor;
                ctx.fill();
            }
        }
    }

    Rectangle {
        id: cbMenu;

        anchors.top: cbMainRect.bottom;

        width: container.width;
        height: cbListView.count * container.height;


        color: Style.baseColor;
        border.color: container.borderColor;
        radius: container.radius;
        visible: container.menuVisible;

        ListView {
            id: cbListView;

            anchors.fill: parent;

            model: container.model;
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

                    visible: container.currentIndex === model.index;
                    color: Style.selectedColor;
                    radius: container.radius;
                }

                Text {
                    id: cbTitleModel;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: container.textCentered ? parent.horizontalCenter : null;
                    anchors.left: !container.textCentered ? cbListDelegate.left : null;
                    anchors.leftMargin: 10;

                    text: model.text;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        container.currentIndex = model.index;
                        cbTitleTxt.text = model.text;
//                            menu.visible = false;
                        container.menuVisible = false;
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
