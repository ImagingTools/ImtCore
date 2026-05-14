import QtQuick

Item {
    id: root
    width: 480
    height: 300

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#0b1220"
    }

    Row {
        id: topRow
        x: 12
        y: 12
        spacing: 12

        AppButton {
            id: openBtn
            text: "Open"
            onClicked: {
                popup.visible = !popup.visible
                console.log("combo26-open-click", popup.visible)
            }
        }

        AppButton {
            id: actionBtn
            text: "Action"
            onClicked: console.log("combo26-action-click")
        }
    }

    Column {
        id: leftColumn
        x: 12
        y: 56
        spacing: 10

        AppMenu {
            id: menu
            title: "Main Menu"
            onItemTriggered: console.log("combo26-menu-item", index)
        }

        AppComboBox {
            id: combo
            onChanged: console.log("combo26-combo-changed", index, currentText)
        }
    }

    AppPopup {
        id: popup
        x: 210
        y: 56
        title: "Info Popup"
        visible: false
    }

    Component.onCompleted: {
        openBtn.dump("combo26-geo")
        actionBtn.dump("combo26-geo")
        menu.dump("combo26-geo")
        combo.dump("combo26-geo")
        popup.dump("combo26-geo")
        console.log("combo26-layout", topRow.x, topRow.y, leftColumn.x, leftColumn.y)

        menu.itemTriggered(1)
        combo.toggle()
        combo.dump("combo26-opened")
        combo.selectIndex(2)
        combo.dump("combo26-selected")

        openBtn.clicked()
        popup.dump("combo26-popup-after-open")
        openBtn.clicked()
        popup.dump("combo26-popup-after-close")

        Qt.quit()
    }
}