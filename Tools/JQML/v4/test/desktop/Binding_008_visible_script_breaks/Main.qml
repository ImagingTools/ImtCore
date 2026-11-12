import QtQuick

// A script write to `visible` replaces its binding (Qt). CustomTooltip.open relies on this
// when it sets ModalDialogManager.backgroundItem.visible = false over
// `visible: modalDialogs.visible` in DialogManagerView.
Item {
    id: root
    property bool flag: true

    Item {
        id: overlay
        width: 10
        height: 10
        visible: root.flag
    }

    Component.onCompleted: {
        overlay.visible = false
        root.flag = true
        console.log("visible", overlay.visible)
        Qt.quit()
    }
}
