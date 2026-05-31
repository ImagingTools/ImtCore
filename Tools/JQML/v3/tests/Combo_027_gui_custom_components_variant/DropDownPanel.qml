import QtQuick

Item {
    id: root
    width: 180
    height: 28

    property int currentIndex: 0

    signal picked(int index)

    function dump(tag) {
        console.log(tag, "dropDown", x, y, width, height, currentIndex)
    }

    Rectangle {
        anchors.fill: parent
        color: "#334155"

        Text {
            anchors.centerIn: parent
            text: "Select " + (root.currentIndex + 1)
        }
    }

    ListModel {
        id: ddModel
        ListElement {}
        ListElement {}
        ListElement {}
    }

    Loader {
        id: ddLoader
        x: 0
        y: root.height + 2
        active: true
        sourceComponent: Rectangle {
            width: root.width
            height: 70
            color: "#0f172a"

            ListView {
                anchors.fill: parent
                model: ddModel

                delegate: Rectangle {
                    width: root.width
                    height: 22
                    color: index === root.currentIndex ? "#1d4ed8" : "#1e293b"

                    Text {
                        anchors.centerIn: parent
                        text: "Select " + (index + 1)
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.currentIndex = index
                            root.picked(index)
                        }
                    }
                }
            }
        }
    }
}