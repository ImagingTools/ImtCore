import QtQuick 2.0

Item {
    id: container;
    width: 90;
    height: 30;
    property var model;
    property int currentIndex: listView.currentIndex;
    property color foneColor: "#4C514A";
    property color borderColor: "black";
    property color fontColor: "white";
    property int radiusValue: 5;

    Rectangle {
        id: mainRect;
        anchors.fill: parent;
        border.color: borderColor;
        color: foneColor;
        radius: radiusValue;

        Text {
            id: title_txt;
//            text: container.model.get(0).text;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;
            color: fontColor;
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                console.log("ComboBox clicked !");
                menu.visible = !menu.visible;
            }
        }
    }

    Rectangle {
        id: menu;
        width: container.width;
        height: listView.count * container.height < 150 ? listView.count * container.height: 150;
        anchors.top: container.bottom;
        color: "transparent";
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
                    color: foneColor;
                    border.color: borderColor;
                    radius: radiusValue;

                    Text {
                        id: titleModel;
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        text: model.text;
                        color: fontColor;
                    }

                    MouseArea {
                        id: mouseArea;
                        anchors.fill: parent;

                        onClicked: {
                            listView.currentIndex = model.index;
                            title_txt.text = model.text;
                            menu.visible = false;
                        }
                    }
                }
            }
        }
    }

}
