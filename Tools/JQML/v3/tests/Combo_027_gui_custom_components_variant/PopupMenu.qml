import QtQuick

Rectangle {
    id: root

    width: 190
    height: 100
    color: "#111827"
    visible: false

    property int itemCount: 4

    signal picked(int index)

    function dump(tag) {
        console.log(tag, "popupMenu", x, y, width, height, visible, itemCount)
    }

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        Repeater {
            model: root.itemCount
            delegate: Rectangle {
                width: 176
                height: 18
                color: "#1f2937"

                Text {
                    anchors.centerIn: parent
                    text: "Menu " + (index + 1)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.picked(index)
                }
            }
        }
    }
}