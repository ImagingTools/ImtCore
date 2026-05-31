import QtQuick

// Reusable component used several times in Main.qml. Every instance shares the
// same internal `id: boxRoot`, which is what makes stable id generation for
// reused components and Repeater delegates non-trivial.
Rectangle {
    id: boxRoot

    property string label: ""

    width: 10
    height: 10

    Text {
        id: caption
        text: boxRoot.label
    }
}
