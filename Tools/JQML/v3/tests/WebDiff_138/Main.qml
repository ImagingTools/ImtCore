import QtQuick

// Reproduces the issue: imperative assignment in onHeightChanged breaks
// the declarative binding "height: inner.height" in JS/Web runtime.
// After the first imperative write, the binding is severed and subsequent
// height changes from inner are no longer reflected in outer.

Item {
    id: root

    property string text: "Hello"

    Text {
        id: textElement
        text: qsTr("text: %1").arg(root.text)
    }

    Component.onCompleted: {
        console.log("wd138", textElement.text)
        root.text = "World"
        console.log("wd138", textElement.text)
        Qt.quit()
    }
}
