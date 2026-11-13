import QtQuick

// ListModel.append must build the delegate and apply its bindings (including a Loader's
// sourceComponent) before the next line — the same path ModalDialogManager.openDialog uses
// so CustomTooltip can hide backgroundItem that onLoaded just assigned.
Item {
    id: root
    width: 200
    height: 200

    property Item loaded: null

    ListModel { id: dialogs }

    Repeater {
        model: dialogs
        delegate: Item {
            Loader {
                sourceComponent: model.comp
                onLoaded: root.loaded = item
            }
        }
    }

    Component {
        id: tip
        Rectangle {
            width: 40
            height: 20
            opacity: 0
            Component.onCompleted: opacity = 1
        }
    }

    // Append after construction (completing === 0), like a tooltip open from hover — not from onCompleted.
    Timer {
        interval: 1
        running: true
        onTriggered: {
            dialogs.append({ "comp": tip })
            console.log("sync", root.loaded !== null, root.loaded ? root.loaded.opacity : -1)
            Qt.quit()
        }
    }
}
