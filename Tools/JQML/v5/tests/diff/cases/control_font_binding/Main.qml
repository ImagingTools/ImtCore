import QtQuick

Item {
    property bool sel: true
    Btn { id: a; text: "a"; checkable: true; checked: sel; font.bold: checked }
    Btn { id: b; text: "b"; checkable: true; checked: !sel; font.bold: checked }
    Component.onCompleted: {
        console.log(a.font.bold, a.contentItem.font.bold, b.font.bold, b.contentItem.font.bold, a.contentItem.font.pixelSize)
        sel = false
        console.log(a.font.bold, a.contentItem.font.bold, b.font.bold, b.contentItem.font.bold)
        a.font.pixelSize = 20
        console.log(a.contentItem.font.pixelSize, a.contentItem.font.bold)
        Qt.quit()
    }
}
