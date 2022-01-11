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
    property color backgroundColor: Style.backgroundColor;
    property bool textCentered: true;
    property int radius: 5;

    Component.onCompleted: {
        if (textCentered){
            cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
           // cbTitleModel.anchors.horizontalCenter = cbListDelegate.horizontalCenter;
        } else {
            cbTitleTxt.anchors.left = cbMainRect.left;
            cbTitleTxt.anchors.leftMargin = 10;

//            cbTitleModel.anchors.left = cbListDelegate.left;
//            cbTitleModel.anchors.leftMargin = 10;

        }
    }

    Rectangle {
        id: cbMainRect;
        anchors.fill: parent;
        border.color: container.borderColor;
        radius: container.radius;
        color: container.backgroundColor;

        Text {
            id: cbTitleTxt;
            anchors.verticalCenter: parent.verticalCenter;


            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

        }

        MouseArea {
            id: cbMouseArea;
            anchors.fill: parent;
            onClicked: {
                console.log("ComboBox clicked !");
                console.log("text color ", Style.textColor);
//                menu.visible = !menu.visible;
                container.menuVisible = !container.menuVisible;
                console.log("menu width ", cbMenu.width);
                console.log("menu height ", cbMenu.height);
                console.log("menu visible ", cbMenu.visible);
                console.log("listview count  ", cbListView.count);
//                container.menuVisible = true;
//                console.log("mouseX", mouseX, "mouseY", mouseY);
            }
        }

        Canvas {
            id: cbArrowIcon;
            height: 5;
            width: 10;

            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.rightMargin: 5;

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
        width: container.width;
       // height: cbListView.count * container.height < 150 ? cbListView.count * container.height: 150;
        height: cbListView.count * container.height;
//        anchors.top: container.bottom;
        anchors.top: cbMainRect.bottom;
//        color: "blue";
//        color: Style.baseColor;
//        color: "#c0c0c0";
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
                //height: container.height;
                height: cbMainRect.height;

                Rectangle {
                    width: cbListDelegate.width - 2;
                    height: cbListDelegate.height - 2;
//                    anchors.fill: parent;
                    anchors.verticalCenter: cbListDelegate.verticalCenter;
                    anchors.horizontalCenter: cbListDelegate.horizontalCenter;
                    visible: container.currentIndex === model.index;
                    color: Style.selectedColor;
                    radius: container.radius;
                }

                Text {
                    id: cbTitleModel;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: container.textCentered ? parent.horizontalCenter : "";
                    anchors.left: !container.textCentered ? cbListDelegate.left : "";
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
                    width: cbListDelegate.width;
                    height: 1;
                    anchors.top: cbListDelegate.bottom;
                    color: Style.textColor;
                    visible: model.index !== cbListView.count - 1;
                }

//                Rectangle {
//                    anchors.fill: parent;
//                    color: container.currentIndex === model.index ? Style.selectedColor : "transparent";
//                   // radius: 5;
//                    border.color: cbMainRect.border.color;

//                    Text {
//                        id: cbTitleModel;
//                        anchors.verticalCenter: parent.verticalCenter;
//                        anchors.horizontalCenter: parent.horizontalCenter;
//                        text: model.text;
//                        color: Style.textColor;
//                    }

//                    MouseArea {
//                        anchors.fill: parent;

//                        onClicked: {
//                            container.currentIndex = model.index;
//                            cbTitleTxt.text = model.text;
////                            menu.visible = false;
//                            container.menuVisible = false;
//                        }
//                    }
//                }

//                Rectangle {
//                    width: cbListDelegate.width;
//                    height: 1;
//                    anchors.top: cbListDelegate.bottom;
//                    color: "black";
//                }
            }
        }
    }
}
