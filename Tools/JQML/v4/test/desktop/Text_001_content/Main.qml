import QtQuick

Item {
    id: root
    property int n: 4

    Text {
        id: label
        text: "n=" + root.n
        font.pixelSize: 16
    }

    Component.onCompleted: {
        console.log(label.text, label.font.pixelSize)
        Qt.quit()
    }
}
