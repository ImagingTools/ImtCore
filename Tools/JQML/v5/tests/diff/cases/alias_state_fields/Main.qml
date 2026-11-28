import QtQuick

Item {
    id: root
    property int horizontal: 5
    property int vertical: 6
    property int offset: 20

    Box {
        id: box
        fieldX: root.horizontal
        fieldY: root.vertical

        states: [
            State {
                name: "left"
                PropertyChanges { target: box; fieldX: root.offset }
            },
            State {
                name: "both"
                PropertyChanges { target: box; fieldX: root.offset; fieldY: 30 }
            }
        ]
    }

    Component.onCompleted: {
        box.state = "left"
        root.offset = 21
        root.horizontal = 7
        root.vertical = 8
        console.log("left", box.fieldX, box.fieldY)
        box.state = "both"
        root.offset = 22
        root.vertical = 9
        console.log("both", box.fieldX, box.fieldY)
        box.state = ""
        console.log("restored", box.fieldX, box.fieldY)
        Qt.quit()
    }
}
