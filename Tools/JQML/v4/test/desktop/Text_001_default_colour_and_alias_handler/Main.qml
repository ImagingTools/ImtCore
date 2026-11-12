import QtQuick

Item {
    width: 200
    height: 100

    Text { id: plain; text: "x" }
    Rectangle { id: rect }

    Item {
        id: holder
        property alias label: field.text
        property int changes: 0
        onLabelChanged: changes++

        Item {
            id: field
            property string text: ""
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("text colour black", Qt.colorEqual(plain.color, "black"))
            console.log("rect colour white", Qt.colorEqual(rect.color, "white"))
            console.log("alias changes at start", holder.changes)
            field.text = "typed"
            console.log("alias after typing", holder.label)
            Qt.quit()
        }
    }
}
