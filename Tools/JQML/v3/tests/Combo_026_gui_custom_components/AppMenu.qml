import QtQuick

Rectangle {
    id: root

    width: 160
    height: 90
    color: "#1f2937"

    property alias title: titleText.text
    property int itemCount: 3

    signal itemTriggered(int index)

    function dump(tag) {
        console.log(tag, "menu", width, height, title, itemCount)
    }

    Text {
        id: titleText
        x: 8
        y: 6
        text: "Menu"
    }

    Column {
        id: itemColumn
        x: 8
        y: 24
        spacing: 4

        Repeater {
            model: root.itemCount
            delegate: Rectangle {
                width: 142
                height: 16
                color: "#374151"

                Text {
                    anchors.centerIn: parent
                    text: "Item " + (index + 1)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.itemTriggered(index)
                }
            }
        }
    }
}