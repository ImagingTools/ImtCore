import QtQuick
import QtQuick.Layouts

Item {
    GridLayout {
        id: grid
        columns: 2
        columnSpacing: 4
        rowSpacing: 6

        // row 0, col 0
        Rectangle {
            color: "red"
            Layout.preferredWidth: 50
            Layout.preferredHeight: 30
        }
        // row 0, col 1
        Rectangle {
            color: "green"
            Layout.preferredWidth: 80
            Layout.preferredHeight: 30
        }
        // row 1, col 0
        Rectangle {
            color: "blue"
            Layout.preferredWidth: 50
            Layout.preferredHeight: 60
        }
        // row 1, col 1 — fillWidth
        Rectangle {
            color: "orange"
            Layout.fillWidth: true
            Layout.preferredHeight: 60
        }
    }

    Component.onCompleted: {
        console.log('grid.width',  grid.width)
        console.log('grid.height', grid.height)
        console.log('r0.x', grid.children[0].x)
        console.log('r0.y', grid.children[0].y)
        console.log('r0.width',  grid.children[0].width)
        console.log('r0.height', grid.children[0].height)
        console.log('r1.x', grid.children[1].x)
        console.log('r1.y', grid.children[1].y)
        console.log('r1.width',  grid.children[1].width)
        console.log('r1.height', grid.children[1].height)
        console.log('r2.x', grid.children[2].x)
        console.log('r2.y', grid.children[2].y)
        console.log('r2.width',  grid.children[2].width)
        console.log('r2.height', grid.children[2].height)
        console.log('r3.x', grid.children[3].x)
        console.log('r3.y', grid.children[3].y)
        console.log('r3.width',  grid.children[3].width)
        console.log('r3.height', grid.children[3].height)
        Qt.quit()
    }
}
