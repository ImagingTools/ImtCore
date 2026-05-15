import QtQuick

Item {
    property string text: "Hello"

    Text {
        id: label
        text: parent.text + " world"
    }

    Component.onCompleted: {
        console.log("label.text =", label.text)
        console.log("startsWith", label.text.startsWith("Hello"))
        Qt.quit()
    }
}
