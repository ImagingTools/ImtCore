import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl

// PlaceholderText of QtQuick.Controls.impl: the horizontal alignment of the text input it is in, followed when it changes
Item {
    width: 300; height: 200
    TextInput {
        id: input
        width: 200; height: 30
        horizontalAlignment: TextInput.AlignRight
        PlaceholderText { id: inInput; anchors.fill: parent; text: "type"; horizontalAlignment: Text.AlignHCenter }
    }
    Item {
        PlaceholderText { id: inItem; text: "free"; horizontalAlignment: Text.AlignRight; elide: Text.ElideRight }
    }
    TextField { id: field; placeholderText: "centered"; horizontalAlignment: TextInput.AlignHCenter }
    Component.onCompleted: {
        console.log(inInput.horizontalAlignment, inItem.horizontalAlignment, inItem.elide, inInput.text)
        input.horizontalAlignment = TextInput.AlignHCenter
        console.log(inInput.horizontalAlignment)
        let p = null
        for (let i = 0; i < field.children.length; i++) if (field.children[i].text === "centered") p = field.children[i]
        console.log(p !== null, p && p.horizontalAlignment, p && p.visible)
        field.horizontalAlignment = TextInput.AlignLeft
        console.log(p && p.horizontalAlignment)
        Qt.quit()
    }
}
