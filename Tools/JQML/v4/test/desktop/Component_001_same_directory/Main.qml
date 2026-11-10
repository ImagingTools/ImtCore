import QtQuick

Item {
    Box {
        id: box
    }

    Component.onCompleted: {
        console.log("box", box.width, box.mark)
        Qt.quit()
    }
}
