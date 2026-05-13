import QtQuick

Item {
    property bool attachedProperty: false

    Text {
        id: textElement
        anchors.centerIn: parent
        text: parent.attachedProperty ? "on" : "off"
    }

    Component.onCompleted: {
        console.log("before", textElement.text)
        attachedProperty = true
        console.log("after", textElement.text)
        Qt.quit()
    }
}
