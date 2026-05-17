import QtQuick

Item {
    property alias textValue: label.text

    Text {
        id: label
        text: "initial"
    }

    Component.onCompleted: {
        console.log("before", label.text)
        textValue = "updated"
        console.log("after", label.text)
        Qt.quit()
    }
}
