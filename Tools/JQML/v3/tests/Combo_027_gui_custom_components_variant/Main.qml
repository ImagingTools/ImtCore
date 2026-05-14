import QtQuick

Item {
    id: root
    width: 520
    height: 320

    Rectangle {
        anchors.fill: parent
        color: "#0a1020"
    }

    Row {
        id: headerRow
        x: 14
        y: 12
        spacing: 10

        ToolButton {
            id: menuBtn
            text: "Menu"
            onTriggered: {
                popup.visible = !popup.visible
                console.log("combo27-menu-toggle", popup.visible)
            }
        }

        ToolButton {
            id: refreshBtn
            text: "Refresh"
            onTriggered: console.log("combo27-refresh")
        }
    }

    Column {
        id: sideColumn
        x: 14
        y: 50
        spacing: 12

        DropDownPanel {
            id: dropDown
            onPicked: console.log("combo27-picked", index)
        }
    }

    PopupMenu {
        id: popup
        x: 220
        y: 50
        onPicked: console.log("combo27-popup-picked", index)
    }

    Component.onCompleted: {
        menuBtn.dump("combo27-geo")
        refreshBtn.dump("combo27-geo")
        dropDown.dump("combo27-geo")
        popup.dump("combo27-geo")
        console.log("combo27-layout", headerRow.x, headerRow.y, sideColumn.x, sideColumn.y)

        dropDown.picked(2)
        popup.picked(1)
        menuBtn.triggered()
        popup.dump("combo27-popup-open")
        menuBtn.triggered()
        popup.dump("combo27-popup-close")

        Qt.quit()
    }
}