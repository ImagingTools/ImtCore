import QtQuick

Item {
    id: appWindow
    objectName: "win"
    width: 200; height: 100
    Panel { id: panel }
    Component.onCompleted: {
        for (let i = 0; i < panel.children.length; i++) if (panel.children[i].text !== undefined) console.log(panel.children[i].text)
        appWindow.width = 50
        for (let i = 0; i < panel.children.length; i++) if (panel.children[i].text !== undefined) console.log(panel.children[i].text)
        Qt.quit()
    }
}
