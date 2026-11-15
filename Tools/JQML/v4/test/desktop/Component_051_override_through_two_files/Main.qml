import QtQuick

Item {
    width: 100
    height: 100

    Component {
        id: dialogComp

        Dlg {
            title: "Message"
            contentComp: Component { Item { width: 200; height: label.height + 20; Text { id: label; text: "x" } } }
            backgroundColor: "white"
            radius: 2
        }
    }

    Component.onCompleted: {
        var obj = dialogComp.createObject(null)
        console.log("title", obj.title)
        console.log("contentComp set", obj.contentComp !== null && obj.contentComp !== undefined)
        console.log("topPanelComp", obj.topPanelComp !== null && obj.topPanelComp !== undefined)
        console.log("radius", obj.radius, "background", obj.backgroundColor)
        Qt.quit()
    }
}
