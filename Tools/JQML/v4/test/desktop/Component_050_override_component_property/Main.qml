import QtQuick

Item {
    width: 100
    height: 100

    property var obj: null

    Component {
        id: dialogComp

        Base {
            title: "Message"
            contentComp: Component { Item { width: 200; height: 50 } }
            backgroundColor: "white"
        }
    }

    Component.onCompleted: {
        obj = dialogComp.createObject(null)
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("title", obj.title)
            console.log("contentComp set", obj.contentComp !== null && obj.contentComp !== undefined)
            console.log("background", obj.backgroundColor)
            console.log("lazy", obj.lazyComp !== null && obj.lazyComp !== undefined)
            console.log("topPanelComp", obj.topPanelComp !== null && obj.topPanelComp !== undefined)
            Qt.quit()
        }
    }
}
