import QtQuick

Item {
    id: root
    width: 260
    height: 180

    Text {
        id: label
        width: 150
        text: "ellipsis candidate line one line two line three"
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }

    Component.onCompleted: {
        console.log("wd136", "text-elide", label.contentWidth, label.contentHeight, label.width, label.height)
        Qt.quit()
    }
}
