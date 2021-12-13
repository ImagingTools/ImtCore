import QtQuick 2.0
import Acf 1.0
import QtGraphicalEffects 1.0

Item {
    id: container;
    width: 90;
    height: 30;
    property var model;
    property int currentIndex: -1;
    property color borderColor: "black";

    Rectangle {
        id: mainRect;
        anchors.fill: parent;
        border.color: container.borderColor;

        color: Style.baseColor;

        Text {
            id: title_txt;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;
            color: Style.textColor;
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                console.log("ComboBox clicked !!!");
                menu.visible = !menu.visible;
                console.log(menu);
                console.log(menu.visible);
            }
        }

        Canvas {
            id:arrowIcon;
            height: 5;
            width: height;

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

    DropShadow {
        anchors.fill: menu;
        verticalOffset: 3;
        radius: 8.0;
        samples: 17;
        color: "#808080";
        source: menu;
        visible: menu.visible;
    }

    Rectangle {
        id: menu;
        width: container.width;
        height: listView.count * container.height < 150 ? listView.count * container.height: 150;
        anchors.top: container.bottom;
//        color: "blue";
        color: Style.baseColor;
        radius: 5;
        visible: false;
        ListView {
            id: listView;
            anchors.fill: parent;
            model: container.model;
            clip: true;
            currentIndex: 0;

            delegate: Item {
                id: listDelegate;
                width: listView.width;
                height: container.height;

                Rectangle {
                    anchors.fill: parent;
                    color: container.currentIndex == index ? Style.selectedColor : "transparent";
//                    border.color: container.borderColor;

                    Text {
                        id: titleModel;
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        text: model.text;
                        color: Style.textColor;
                    }

                    MouseArea {
                        id: mouseArea;
                        anchors.fill: parent;

                        onClicked: {
                            container.currentIndex = model.index;
                            title_txt.text = model.text;
                            menu.visible = false;
                        }
                    }
                }
            }
        }
    }
}
