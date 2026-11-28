import QtQuick
Item {
    id: root
    property string source: "initial"
    property bool overrideValue: false
    property alias text: input.text
    TextInput { id: input; text: root.source }
    Binding { target: root; property: "text"; value: "filter"; when: root.overrideValue }
    Component.onCompleted: {
        console.log("initial", text)
        overrideValue = true
        source = "changed"
        console.log("active", text)
        overrideValue = false
        console.log("restored", text)
        Qt.quit()
    }
}
