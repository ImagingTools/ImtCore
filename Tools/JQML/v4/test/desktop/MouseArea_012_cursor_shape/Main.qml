import QtQuick

// cursorShape is assignable and readable as a Qt::CursorShape value.
Item {
    width: 40
    height: 40

    MouseArea {
        id: area
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
    }

    Component.onCompleted: {
        console.log("hand", area.cursorShape, Qt.PointingHandCursor)
        area.cursorShape = Qt.IBeamCursor
        console.log("ibeam", area.cursorShape, Qt.IBeamCursor)
        area.cursorShape = Qt.SizeHorCursor
        console.log("split", area.cursorShape, Qt.SizeHorCursor)
        Qt.quit()
    }
}
