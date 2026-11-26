import QtQuick
import QtTest

// A click on a row of a ListView that is not active gives the row the active focus (forceActiveFocus in onClicked):
// the arrow keys go to the Keys.onPressed of the row (the Table of the ControlsGallery)
Item {
    id: root
    width: 300; height: 300
    property int selected: -1
    property var log: []
    TestCase { id: tc; name: "focus"; when: windowShown
        function test_click_then_down() {
            tc.mouseClick(list, 50, 45)
            console.log("selected", root.selected, "row focus", list.currentItem !== null)
            tc.keyClick(Qt.Key_Down)
            console.log("after down", root.selected, JSON.stringify(root.log))
        }
    }
    ListView {
        id: list
        anchors.fill: parent
        model: 10
        delegate: Item {
            id: row
            width: 300; height: 30
            Keys.onPressed: (event) => {
                root.log = root.log.concat([index + ":" + event.key])
                if (event.key === Qt.Key_Down) { root.selected = index + 1; event.accepted = true }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: (mouse) => { root.selected = index; row.forceActiveFocus(); mouse.accepted = false }
            }
        }
    }
}
