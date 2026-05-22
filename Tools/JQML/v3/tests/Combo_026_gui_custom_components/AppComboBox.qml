import QtQuick

Item {
    id: root

    width: 170
    height: 30

    property int currentIndex: 0
    property string currentText: "Option 1"
    property bool opened: false

    signal changed(int index)

    function toggle() {
        opened = !opened
    }

    function selectIndex(i) {
        currentIndex = i
        currentText = "Option " + (i + 1)
        opened = false
        changed(i)
    }

    function dump(tag) {
        console.log(tag, "combo", width, height, currentIndex, currentText, opened)
    }

    Rectangle {
        id: face
        anchors.fill: parent
        color: "#334155"

        Text {
            anchors.centerIn: parent
            text: root.currentText
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.toggle()
        }
    }

    ListModel {
        id: comboModel
        ListElement {}
        ListElement {}
        ListElement {}
    }

    Loader {
        id: popupLoader
        x: 0
        y: root.height + 2
        active: root.opened
        sourceComponent: Rectangle {
            width: root.width
            height: 70
            color: "#0f172a"

            ListView {
                id: list
                anchors.fill: parent
                model: comboModel

                delegate: Rectangle {
                    width: root.width
                    height: 22
                    color: index === root.currentIndex ? "#1d4ed8" : "#1e293b"

                    Text {
                        anchors.centerIn: parent
                        text: "Option " + (index + 1)
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.selectIndex(index)
                    }
                }
            }
        }
    }
}